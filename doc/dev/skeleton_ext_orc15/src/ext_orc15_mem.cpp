

#include "ext_orc15_mem.h"
#include "ext_orc15_mem_stream.h"
#include "ext_orc15_time.h"
#include "LoggerGlobal.h"


#ifdef __cplusplus
extern "C" {
#endif

#include "postgres.h"
#include "utils/builtins.h"


PG_FUNCTION_INFO_V1(ext_orc_mem_test);



#ifdef __cplusplus
}
#endif

//#include "fmgr.h"

#include "orc/ColumnPrinter.hh"
//#include "orc/OrcFile.hh"
//#include "orc/MemoryPool.hh"


#include <unistd.h>
#include <fstream>
#include <sys/time.h>
//#include <sstream>
//#include <memory>
#include <string>
#include <vector>
#include <algorithm>






char gDelimiter = ',';

std::string extractColumn(std::string s, uint64_t colIndex)
{
    uint64_t col = 0;
    size_t start = 0;
    size_t end = s.find(gDelimiter);
    while (col < colIndex && end != std::string::npos)
    {
        start = end + 1;
        end = s.find(gDelimiter, start);
        ++col;
    }
    return col == colIndex ? s.substr(start, end - start) : "";
}

void fillLongValues(const std::vector<std::string>& data,
                    orc::ColumnVectorBatch* batch,
                    uint64_t numValues,
                    uint64_t colIndex)
{
    orc::LongVectorBatch* longBatch = dynamic_cast<orc::LongVectorBatch*> (batch);
    bool hasNull = false;
    for (uint64_t i = 0; i < numValues; ++i)
    {
        std::string col = extractColumn(data[i], colIndex);
        if (col.empty())
        {
            batch->notNull[i] = 0;
            hasNull = true;
        }
        else
        {
            batch->notNull[i] = 1;
            longBatch->data[i] = atoll(col.c_str());
        }
    }
    longBatch->hasNulls = hasNull;
    longBatch->numElements = numValues;
}

void fillStringValues(const std::vector<std::string>& data,
                      orc::ColumnVectorBatch* batch,
                      uint64_t numValues,
                      uint64_t colIndex,
                      orc::DataBuffer<char>& buffer,
                      uint64_t& offset)
{
    orc::StringVectorBatch* stringBatch = dynamic_cast<orc::StringVectorBatch*> (batch);
    bool hasNull = false;
    for (uint64_t i = 0; i < numValues; ++i)
    {
        std::string col = extractColumn(data[i], colIndex);
        if (col.empty())
        {
            batch->notNull[i] = 0;
            hasNull = true;
        }
        else
        {
            batch->notNull[i] = 1;
            if (buffer.size() - offset < col.size())
            {
                buffer.reserve(buffer.size() * 2);
            }
            memcpy(buffer.data() + offset, col.c_str(), col.size());
            stringBatch->data[i] = buffer.data() + offset;
            stringBatch->length[i] = static_cast<int64_t> (col.size());
            offset += col.size();
        }
    }
    stringBatch->hasNulls = hasNull;
    stringBatch->numElements = numValues;
}

void fillDoubleValues(const std::vector<std::string>& data,
                      orc::ColumnVectorBatch* batch,
                      uint64_t numValues,
                      uint64_t colIndex)
{
    orc::DoubleVectorBatch* dblBatch = dynamic_cast<orc::DoubleVectorBatch*> (batch);
    bool hasNull = false;
    for (uint64_t i = 0; i < numValues; ++i)
    {
        std::string col = extractColumn(data[i], colIndex);
        if (col.empty())
        {
            batch->notNull[i] = 0;
            hasNull = true;
        }
        else
        {
            batch->notNull[i] = 1;
            dblBatch->data[i] = atof(col.c_str());
        }
    }
    dblBatch->hasNulls = hasNull;
    dblBatch->numElements = numValues;
}

// parse fixed point decimal numbers

void fillDecimalValues(const std::vector<std::string>& data,
                       orc::ColumnVectorBatch* batch,
                       uint64_t numValues,
                       uint64_t colIndex,
                       size_t scale,
                       size_t precision)
{


    orc::Decimal128VectorBatch* d128Batch = ORC_NULLPTR;
    orc::Decimal64VectorBatch* d64Batch = ORC_NULLPTR;
    if (precision <= 18)
    {
        d64Batch = dynamic_cast<orc::Decimal64VectorBatch*> (batch);
        d64Batch->scale = static_cast<int32_t> (scale);
    }
    else
    {
        d128Batch = dynamic_cast<orc::Decimal128VectorBatch*> (batch);
        d128Batch->scale = static_cast<int32_t> (scale);
    }
    bool hasNull = false;
    for (uint64_t i = 0; i < numValues; ++i)
    {
        std::string col = extractColumn(data[i], colIndex);
        if (col.empty())
        {
            batch->notNull[i] = 0;
            hasNull = true;
        }
        else
        {
            batch->notNull[i] = 1;
            size_t ptPos = col.find('.');
            size_t curScale = 0;
            std::string num = col;
            if (ptPos != std::string::npos)
            {
                curScale = col.length() - ptPos - 1;
                num = col.substr(0, ptPos) + col.substr(ptPos + 1);
            }
            orc::Int128 decimal(num);
            while (curScale != scale)
            {
                curScale++;
                decimal *= 10;
            }
            if (precision <= 18)
            {
                d64Batch->values[i] = decimal.toLong();
            }
            else
            {
                d128Batch->values[i] = decimal;
            }
        }
    }
    batch->hasNulls = hasNull;
    batch->numElements = numValues;
}

void fillBoolValues(const std::vector<std::string>& data,
                    orc::ColumnVectorBatch* batch,
                    uint64_t numValues,
                    uint64_t colIndex)
{
    orc::LongVectorBatch* boolBatch =
            dynamic_cast<orc::LongVectorBatch*> (batch);
    bool hasNull = false;
    for (uint64_t i = 0; i < numValues; ++i)
    {
        std::string col = extractColumn(data[i], colIndex);
        if (col.empty())
        {
            batch->notNull[i] = 0;
            hasNull = true;
        }
        else
        {
            batch->notNull[i] = 1;
            std::transform(col.begin(), col.end(), col.begin(), ::tolower);
            if (col == "true" || col == "t")
            {
                boolBatch->data[i] = true;
            }
            else
            {
                boolBatch->data[i] = false;
            }
        }
    }
    boolBatch->hasNulls = hasNull;
    boolBatch->numElements = numValues;
}

// parse date string from format YYYY-mm-dd

void fillDateValues(const std::vector<std::string>& data,
                    orc::ColumnVectorBatch* batch,
                    uint64_t numValues,
                    uint64_t colIndex)
{
    orc::LongVectorBatch* longBatch = dynamic_cast<orc::LongVectorBatch*> (batch);
    bool hasNull = false;
    for (uint64_t i = 0; i < numValues; ++i)
    {
        std::string col = extractColumn(data[i], colIndex);
        if (col.empty())
        {
            batch->notNull[i] = 0;
            hasNull = true;
        }
        else
        {
            batch->notNull[i] = 1;
            struct tm tm;
            memset(&tm, 0, sizeof (struct tm));
            strptime(col.c_str(), "%Y-%m-%d", &tm);
            time_t t = mktime(&tm);
            time_t t1970 = 0;
            double seconds = difftime(t, t1970);
            int64_t days = static_cast<int64_t> (seconds / (60 * 60 * 24));
            longBatch->data[i] = days;
        }
    }
    longBatch->hasNulls = hasNull;
    longBatch->numElements = numValues;
}

// parse timestamp values in seconds

void fillTimestampValues(const std::vector<std::string>& data,
                         orc::ColumnVectorBatch* batch,
                         uint64_t numValues,
                         uint64_t colIndex)
{
    struct tm timeStruct;
    orc::TimestampVectorBatch* tsBatch = dynamic_cast<orc::TimestampVectorBatch*> (batch);
    bool hasNull = false;
    for (uint64_t i = 0; i < numValues; ++i)
    {
        std::string col = extractColumn(data[i], colIndex);
        if (col.empty())
        {
            batch->notNull[i] = 0;
            hasNull = true;
        }
        else
        {
            memset(&timeStruct, 0, sizeof (timeStruct));
            char *left = strptime(col.c_str(), "%Y-%m-%d %H:%M:%S", &timeStruct);
            if (left == nullptr)
            {
                batch->notNull[i] = 0;
            }
            else
            {
                batch->notNull[i] = 1;
                tsBatch->data[i] = timegm(&timeStruct);
                char *tail;
                double d = strtod(left, &tail);
                if (tail != left)
                {
                    tsBatch->nanoseconds[i] = static_cast<long> (d * 1000000000.0);
                }
                else
                {
                    tsBatch->nanoseconds[i] = 0;
                }
            }
        }
    }
    tsBatch->hasNulls = hasNull;
    tsBatch->numElements = numValues;
}


/*
static const char* GetDate(void)
{
    const int   len = 256;
    static char buf[len];

    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    struct tm p;
    if (localtime_r(&(ts.tv_sec), &p) == NULL) return "";

    int ret = strftime(buf, len, "[%Y-%m-%d %H:%M:%S", &p);
    if (ret == 0) return "";

    snprintf(&buf[ret], len-ret, ".%09ld] - ", ts.tv_nsec);

    return buf;
}
*/

ORC_UNIQUE_PTR<orc::Writer> MemTestWriter(orc::Type &ftype, MemOutputStream *outStream)
{
    uint64_t             stripeSize  = (128 << 20); // 128M
    uint64_t             blockSize   = (64 << 10);  // 64K
    orc::CompressionKind compression = orc::CompressionKind_ZLIB;

    orc::WriterOptions options;
    options.setStripeSize(stripeSize);
    options.setCompressionBlockSize(blockSize);
    options.setCompression(compression);

    return orc::createWriter(ftype, outStream, options);
}

class FileBatchLoader
{
public:
    FileBatchLoader(const std::string &fname)
        : _f(fname.c_str())
        , _isEof(false)
    {
        if (!_f)
        {
            LOG_LINE_GLOBAL("ERROR", "Unable to open %s", fname.c_str());
            _isEof = true;
        }
    }

    std::vector<std::string> LoadBatch(uint64_t batchSize)
    {
        std::vector<std::string> data;
        std::string              line;

        for (uint64_t i = 0; i < batchSize; ++i)
        {
            if (!std::getline(_f, line))
            {
                _isEof = true;
                break;
            }
            data.push_back(line);
        }

        return std::move(data);
    }

    static void dump (const std::string &msg, std::vector<std::string> &data)
    {
        LOG_LINE_GLOBAL("MemTest", "Data Dump: %s", msg.c_str());
        for (const auto &ln : data) LOG_LINE_GLOBAL("MemTest", "%s", ln.c_str());
    }


    bool IsEof() { return _isEof; }

private :

    std::ifstream  _f;
    bool           _isEof = true;
};


void MemTestLoad(const std::string &input, std::unique_ptr<MemOutputStream> &outStream)
{
    char cwd[256];

	LOG_LINE_GLOBAL("MemTest", "--->");
	LOG_LINE_GLOBAL("cwd", "%s", getcwd(cwd, 256));


    uint64_t                   batchSize   = 1024;

    orc::DataBuffer<char>      buffer(*orc::getDefaultPool(), 4 * 1024 * 1024);

    std::string                schema = "struct<a:bigint,b:string,c:double>";
    std::unique_ptr<orc::Type> fileType = orc::Type::buildTypeFromString("struct<col1:bigint,col2:string,col3:double>");

    std::unique_ptr<orc::Writer>               writer      = MemTestWriter(*fileType, outStream.get());
    std::unique_ptr<orc::ColumnVectorBatch>    rowBatch    = writer->createRowBatch(batchSize);


    CallMeasure  callElapses;

    LOG_LINE_GLOBAL("MemTest", "Open csv %s", input.c_str());
    FileBatchLoader  finput(input.c_str());

    LOG_LINE_GLOBAL("MemTest", "Begin looping");

    while (/*!eof*/ !finput.IsEof())
    {
        uint64_t numValues    = 0;   // num of lines read in a batch
        uint64_t bufferOffset = 0;   // current offset in the string buffer

        memset(rowBatch->notNull.data(), 1, batchSize);

        std::vector<std::string> data = std::move(finput.LoadBatch(batchSize));
        numValues += data.size();
        FileBatchLoader::dump("CSV File", data);

        if (numValues != 0)
        {
            orc::StructVectorBatch* structBatch = dynamic_cast<orc::StructVectorBatch*>(rowBatch.get());
            structBatch->numElements = numValues;

            for (uint64_t i = 0; i < structBatch->fields.size(); ++i)
            {
                const orc::Type* subType = fileType->getSubtype(i);
                switch (subType->getKind())
                {
                    case orc::BYTE:
                    case orc::INT:
                    case orc::SHORT:
                    case orc::LONG:      fillLongValues      (data, structBatch->fields[i], numValues, i); break;
                    case orc::STRING:
                    case orc::CHAR:
                    case orc::VARCHAR:
                    case orc::BINARY:    fillStringValues    (data, structBatch->fields[i], numValues, i, buffer, bufferOffset); break;
                    case orc::FLOAT:
                    case orc::DOUBLE:    fillDoubleValues    (data, structBatch->fields[i], numValues, i); break;
                    case orc::DECIMAL:   fillDecimalValues   (data, structBatch->fields[i], numValues, i, subType->getScale(), subType->getPrecision()); break;
                    case orc::BOOLEAN:   fillBoolValues      (data, structBatch->fields[i], numValues, i); break;
                    case orc::DATE:      fillDateValues      (data, structBatch->fields[i], numValues, i); break;
                    case orc::TIMESTAMP: fillTimestampValues (data, structBatch->fields[i], numValues, i); break;
                    case orc::STRUCT:
                    case orc::LIST:
                    case orc::MAP:
                    case orc::UNION:
                        throw std::runtime_error(subType->toString() + " is not supported yet.");
                }
            }


            LOG_LINE_GLOBAL("MemTest", "Write");
            callElapses += CallMeasureFrame([&](){writer->add(*rowBatch);});
        }

    }


    LOG_LINE_GLOBAL("MemTest", "Close");
    callElapses += CallMeasureFrame([&](){writer->close();});


    LOG_LINE_GLOBAL("MemTest", "Finish importing Orc file." );
    LOG_LINE_GLOBAL("MemTest", "Total writer elasped time: %f ns.", callElapses.elapses);
    LOG_LINE_GLOBAL("MemTest", "Total writer CPU time: %f ns."    , callElapses.elapsesCPU);
}



void TestMemReader(void *buff, uint64_t len)
{
    try
    {
        LOG_LINE_GLOBAL("MemTest", "----> buff=%p, len=%ul", buff, len);

        orc::ReaderOptions              readerOpts;
        std::unique_ptr<orc::Reader>    reader     = orc::createReader(std::unique_ptr<orc::InputStream> (new MemInputStream ("TestMemStream",  (MemIOStream::byte_t *)buff, len)), readerOpts);;

        orc::RowReaderOptions           rowReaderOpts;
        std::unique_ptr<orc::RowReader> rowReader  = reader->createRowReader(rowReaderOpts);;

        std::unique_ptr<orc::ColumnVectorBatch> batch = rowReader->createRowBatch(1000);

        std::string line;
        std::unique_ptr<orc::ColumnPrinter> printer = createColumnPrinter(line, &rowReader->getSelectedType());

        while (rowReader->next(*batch))
        {
            printer->reset(*batch);
            for(unsigned long i = 0; i < batch->numElements; ++i)
            {
                line.clear();
                printer->printRow(i);

				LOG_LINE_GLOBAL("MemTest", "%s", line.c_str());
            }
        }

        LOG_LINE_GLOBAL("MemTest", "----<");
    }
    catch(...)
    {
        LOG_LINE_GLOBAL("ERROR", ".... Something CATHED ....");
    }
}


Datum ext_orc_mem_test(PG_FUNCTION_ARGS)
{
    elog(LOG, "ext_orc_mem_test 0.0.1");
    
    std::string sname;
    text *snameText = PG_GETARG_TEXT_PP(0);
    if (snameText != nullptr && VARSIZE_ANY_EXHDR(snameText) > 0)
        sname = std::string(VARDATA(snameText), VARSIZE_ANY_EXHDR(snameText));
    else 
        sname = std::string("/home/postgres/sock_ext_orc15");
    
    LogLineGlbSocketName (sname.c_str());
    LOG_LINE_GLOBAL("Init", "VER  0.0.1\n");


    LOG_LINE_GLOBAL("ext_orc_mem_test", "---->");

    std::unique_ptr<MemOutputStream> outStream(new MemOutputStream("MemOStream"));

    //MemTestLoad("/home/postgres/projects/filikadb/doc/dev/skeleton_ext_orc/TestCSVFileImport.test10rows.csv", outStream);
    MemTestLoad("/home/postgres/projects/orc/examples/TestCSVFileImport.test10rows.csv", outStream);

    std::string dmp = outStream->dump();
    LOG_LINE_GLOBAL("DUMP", "\n%s", dmp.c_str());


    TestMemReader(outStream->Ptr(), outStream->Idx());

    LOG_LINE_GLOBAL("ext_orc_mem_test", "----<");
    PG_RETURN_TEXT_P(cstring_to_text(sname.c_str()));
}


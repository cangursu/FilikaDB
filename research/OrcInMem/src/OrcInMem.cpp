

#include "OrcInMem.h"
#include "OrcInMemStream.h"
#include "OrcInMemTime.h"
#include "LoggerGlobal.h"
#include "OrcInMemGlobals.h"



#ifdef __cplusplus
extern "C" {
#endif

#include "postgres.h"
#include "utils/builtins.h"

PG_MODULE_MAGIC;


PG_FUNCTION_INFO_V1(orc_inmem_test);



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
#include <sstream>
#include <vector>
#include <algorithm>
#include <dirent.h>
#include <stdexcept>



#define GETARG_TEXT(pidx, defVal)  getarg_text(PG_GETARG_TEXT_PP(pidx), (defVal))


std::string getarg_text(text *textVal, const char *defVal)
{
    if (textVal != nullptr && VARSIZE_ANY_EXHDR(textVal) > 0)
        return std::move(std::string(VARDATA(textVal), VARSIZE_ANY_EXHDR(textVal)));
    else
        return defVal;
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



std::string extractColumn(std::string s, uint64_t colIndex)
{
    uint64_t col = 0;
    size_t start = 0;
    size_t end = s.find(g_delimiter);
    while (col < colIndex && end != std::string::npos)
    {
        start = end + 1;
        end = s.find(g_delimiter, start);
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


ORC_UNIQUE_PTR<orc::Writer> MemTestWriter(orc::Type &ftype, orc::OutputStream *outStream)
{
    uint64_t             stripeSize  = (128 << 20); // 128M
    uint64_t             blockSize   = (64 << 10);  // 64K
    orc::CompressionKind compression = orc::CompressionKind_NONE;//orc::CompressionKind_ZLIB;

    orc::WriterOptions options;
    options.setStripeSize(stripeSize);
    options.setCompressionBlockSize(blockSize);
    options.setCompression(compression);

    return orc::createWriter(ftype, outStream, options);
}


class BatchLoader
{
    public:
        virtual ~BatchLoader()
        {
        }
        virtual const std::vector<std::string> &LoadBatch(uint64_t batchSize) = 0;
        virtual bool IsEqual(const std::vector<std::string> val) const
        {
            return true;
        }
};

class FileBatchLoader : public BatchLoader
{
public:
    virtual ~FileBatchLoader()
    {
        _data.clear();
    }

    FileBatchLoader(const std::string &fname)
        : _f(fname.c_str())
        , _isEof(false)
    {
        if (!_f)
        {
            LOG_LINE_GLOBAL("ERROR", "Unable to open ", fname);
            elog (LOG, "ERROR : Unable to open %s", fname.c_str());
            _isEof = true;
        }
    }

    virtual const std::vector<std::string> &LoadBatch(uint64_t batchSize)
    {
        _data.clear();
        std::string              line;

        for (uint64_t i = 0; i < batchSize; ++i)
        {
            if (!_f.good() || !std::getline(_f, line))
            {
                _isEof = true;
                break;
            }
            _data.push_back(line);
        }
        return Data();
    }

    virtual bool IsEqual(const std::vector<std::string> val) const
    {
          return ( _data.size() < val.size() ) ?
               std::equal(_data.cbegin(), _data.cend(),   val.cbegin()) :
               std::equal(val.cbegin(),     val.cend(), _data.cbegin()) ;

    }

    static void dump (const std::string &msg, const std::vector<std::string> &data)
    {
        LOG_LINE_GLOBAL("MemTest", "Data Dump: ", msg);
        for (const auto &ln : data) LOG_LINE_GLOBAL("MemTest", ln);
    }


    const std::vector<std::string> &Data() { return _data;  }
    bool  IsEof()                          { return _isEof; }

private :

    std::ifstream            _f;
    bool                     _isEof = true;
    std::vector<std::string> _data;
};



class RandomBatchLoader  : public BatchLoader
{
    public:
        RandomBatchLoader(int startIndex, int increment, const std::string typestr)
            : _startIdx(startIndex)
            , _inc(increment)
        {
            //std::vector<std::pair<std::string, Type*> > res = TypeImpl::parseType(input, 0, input.size());
        }

        virtual const std::vector<std::string> &LoadBatch(uint64_t batchSize)
        {
            return _data;
        }
        std::vector<std::string> _data;
    private :
        int _startIdx = 1;
        int _idx      = 1;
        int _inc      = 1;
};





void MemTestLoad(const std::string &msg, const std::string &input, std::unique_ptr<orc::OutputStream> &outStream)
{
    uint64_t                                batchSize   = 1024;

    orc::DataBuffer<char>                   buffer(*orc::getDefaultPool(), 4 * 1024 * 1024);

    std::unique_ptr<orc::Type>              fileType = orc::Type::buildTypeFromString("struct<col1:bigint,col2:string,col3:double>");
    std::unique_ptr<orc::Writer>            writer   = MemTestWriter(*fileType, outStream.get());
    std::unique_ptr<orc::ColumnVectorBatch> rowBatch = writer->createRowBatch(batchSize);


    CallMeasure  callElapses;

    LOG_LINE_GLOBAL("MemTest", msg, " - source:", input);
    FileBatchLoader  finput(input.c_str());

    try
    {
        while (/*!eof*/ !finput.IsEof())
        {
            uint64_t numValues    = 0;   // num of lines read in a batch
            uint64_t bufferOffset = 0;   // current offset in the string buffer

            memset(rowBatch->notNull.data(), 1, batchSize);
            const std::vector<std::string> &data = finput.LoadBatch(batchSize);
            numValues += data.size();
    //        FileBatchLoader::dump("CSV File", data);
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
                callElapses += CallMeasureFrame([&](){writer->add(*rowBatch);});
            }

        }

        callElapses += CallMeasureFrame([&](){writer->close();});
    }
    catch(std::exception &ex)
    {
        LOG_LINE_GLOBAL("ERROR", "Excaption CATHED ", ex.what());
    }
    catch(...)
    {
        LOG_LINE_GLOBAL("ERROR", ".... Something CATHED ....");
    }


    LOG_LINE_GLOBAL("MemTest", "Total writer elasped time: ", callElapses.elapses,    " ms.");
    LOG_LINE_GLOBAL("MemTest", "Total writer CPU time    : ", callElapses.elapsesCPU, " ms.");
}


bool TestMemReader(const std::string &msg, const std::string &fname, std::unique_ptr<orc::InputStream> &inStream)
{
    CallMeasure  callMsr;
    int  lnCmp   = 0;
    bool isNext  = true;

    try
    {
        LOG_LINE_GLOBAL("MemTest", msg, " - source:", fname);
        //LOG_LINE_GLOBAL("MemTest", "----> buff=", buff, ", len=", len, ", fname=", fname);

        std::ifstream fexpected(fname);
        if (!fexpected.good())
        {
            LOG_LINE_GLOBAL("**ERROR**", "Unable to open file ", fname);
            lnCmp = 1;
        }
        else
        {
            orc::ReaderOptions                      readerOpts;
            std::unique_ptr<orc::Reader>            reader;
            orc::RowReaderOptions                   rowReaderOpts;
            std::unique_ptr<orc::RowReader>         rowReader;
            std::unique_ptr<orc::ColumnVectorBatch> batch;
            std::unique_ptr<orc::ColumnPrinter>     printer;

            std::string lineMem;
            char        lineFile[g_buffLength];

            //rowReaderOpts.include({0,1,2});

            callMsr += CallMeasureFrame([&](){reader    = orc::createReader(std::unique_ptr<orc::InputStream> (inStream.release()), readerOpts);});
            callMsr += CallMeasureFrame([&](){rowReader = reader->createRowReader(rowReaderOpts);});
            callMsr += CallMeasureFrame([&](){batch     = rowReader->createRowBatch(512);});
            callMsr += CallMeasureFrame([&](){printer   = createColumnPrinter(lineMem, &rowReader->getSelectedType());});
            while ((lnCmp == 0) && isNext)
            {
                callMsr += CallMeasureFrame([&](){isNext = rowReader->next(*batch);});
                if (isNext)
                {
                    printer->reset(*batch);
                    for(unsigned long i = 0; (lnCmp == 0) && (i < batch->numElements); ++i)
                    {
                        lineMem.clear();
                        callMsr += CallMeasureFrame([&](){printer->printRow(i);});

                        fexpected.getline(lineFile, g_buffLength);
                        lnCmp = lineMem.compare(lineFile);

                        //LOG_LINE_GLOBAL("MemTest", "line    : ", lineMem);
                        //LOG_LINE_GLOBAL("MemTest", "expect  : ", lineFile);
                        //LOG_LINE_GLOBAL("MemTest", "lnCmp   : ", lnCmp);

                    }
                }
            }

            if (fexpected.good())
            {
                //LOG_LINE_GLOBAL("MemTest", ".out file still have data");
                lnCmp = 1;
            }
        }

        LOG_LINE_GLOBAL("MemTest", "Total reader elasped time: ", callMsr.elapses,    " ms.");
        LOG_LINE_GLOBAL("MemTest", "Total reader CPU     time: ", callMsr.elapsesCPU, " ms.");

        //LOG_LINE_GLOBAL("MemTest", "----<");
    }
    catch(std::exception &ex)
    {
        lnCmp = 1;
        LOG_LINE_GLOBAL("ERROR", "Excaption CATHED : ", ex.what());
    }
    catch(...)
    {
        lnCmp = 1;
        LOG_LINE_GLOBAL("ERROR", ".... Something CATHED ....");
    }

    return lnCmp == 0;
}


bool OrcInmemTest(const std::string &fname)
{
    bool result = true;

    std::string in  = fname;
    std::string out = fname;
    std::string orc = fname;
    in  += ".in";
    out += ".out";
    orc += ".orc";

    {
        std::unique_ptr<orc::OutputStream>   outStreamMem (new MemOutputStream("MemOStream"));
        MemTestLoad("InMem Interface", in, outStreamMem);
        //LOG_LINE_GLOBAL("MemTest", "Size : ", outStreamMem->Size() ,", Len : ", outStreamMem->Idx());

        MemOutputStream *p = dynamic_cast<MemOutputStream*>(outStreamMem.get());
        std::unique_ptr<orc::InputStream> inStreamMem  (new MemInputStream ("TestMemStream",  (MemIOStream::byte_t *)p->Ptr(), p->Idx()));
        result &= TestMemReader("InMem Interface", out, inStreamMem);
    }

    {
        std::unique_ptr<orc::OutputStream> outStreamFile = orc::writeLocalFile(orc);
        MemTestLoad("File Interface", in, outStreamFile);

        std::unique_ptr<orc::InputStream> inStreamFile = orc::readFile(orc);
        result &= TestMemReader("File Interface", out, inStreamFile);
        //LOG_LINE_GLOBAL("MemTest", "result = ", result?"true":"false");
    }

    return result;
}

Datum orc_inmem_test(PG_FUNCTION_ARGS)
{
    elog(LOG, "orc_inmem_test - ver:0.0.1");

    const std::string logs = GETARG_TEXT(0, g_defLogSocket);
    const std::string path = GETARG_TEXT(1, g_defTestDataPath);
    elog(LOG, "orc_inmem_test - Log socket:%s", logs.c_str());

    LogLineGlbSocketName (logs.c_str());

    LOG_LINE_GLOBAL("Init", "VER  0.0.3");
    LOG_LINE_GLOBAL("orc_inmem_test", "---->");
    LOG_LINE_GLOBAL("orc_inmem_test", "logs : ", logs.c_str());
    LOG_LINE_GLOBAL("orc_inmem_test", "Path : ", path.c_str());

    bool  isPass = true;
    DIR *dir = opendir(path.c_str());
    if (dir == NULL)
    {
        LOG_LINE_GLOBAL("orc_inmem_test", "Unable to access path");
        isPass = false;
    }
    else
    {
        dirent *de = nullptr;
        while (nullptr != (de = readdir (dir)) && (true == isPass))
        {
            if (   (de->d_type && DT_REG)
                && (0 == strncmp(de->d_name, "TestData", 8))
                && (0 == strncmp(de->d_name + (strlen(de->d_name)-3), ".in", 8)) )
            {
                std::string fname(de->d_name, 0, (strlen(de->d_name)-3));
                LOG_LINE_GLOBAL("orc_inmem_test", " ");
                LOG_LINE_GLOBAL("orc_inmem_test", "FName   = ", fname.c_str());

                isPass = OrcInmemTest(path + fname);
                LOG_LINE_GLOBAL("orc_inmem_test", "isPass = ", isPass?"true":"false");
            }
        }

        closedir(dir);

        //isPass = OrcInmemTest("/home/postgres/projects/filikadb/research/OrcInMem/test/sql/dataxxx/TestData5000");
    }


    LOG_LINE_GLOBAL("orc_inmem_test", "----<  ", isPass ? "orc_inmem_test SUCCEED" : "orc_inmem_test FAILED");
    PG_RETURN_TEXT_P(cstring_to_text(isPass ? "orc_inmem_test SUCCEED" : "orc_inmem_test FAILED"));
}


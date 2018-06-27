



#ifdef __cplusplus
extern "C" {
#endif

#include "postgres.h"
#include "utils/builtins.h"

PG_FUNCTION_INFO_V1(ext_orc_scan);
PG_FUNCTION_INFO_V1(ext_orc_content);


#ifdef __cplusplus
}
#endif



#include "fmgr.h"

#include "orc/ColumnPrinter.hh"
#include "orc/OrcFile.hh"
#include "orc/MemoryPool.hh"

#include <algorithm>
#include <vector>
#include <sys/time.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <string.h>
#include <memory>










void Print(std::ostream &out, const std::string &name, const std::string &value = "")
{
    out.width(18);
    out << std::left << name << ":  " << value << std::endl;
}

void Print(std::ostream &out, const std::string &name, long value)
{
    Print(out, name, std::move(std::to_string(value)));
}

void ORCPrintContents(std::ostream &out, const std::string &filename, const orc::RowReaderOptions& rowReaderOpts)
{
    Print(out, "ORCPrintContents");
    Print(out, "filename ", filename);

    try
    {
        orc::ReaderOptions              readerOpts;
        std::unique_ptr<orc::Reader>    reader;
        std::unique_ptr<orc::RowReader> rowReader;

        reader     = orc::createReader(orc::readLocalFile(filename), readerOpts);
        rowReader  = reader->createRowReader(rowReaderOpts);

        std::unique_ptr<orc::ColumnVectorBatch> batch = rowReader->createRowBatch(1000);

        std::string line;
        std::unique_ptr<orc::ColumnPrinter> printer = createColumnPrinter(line, &rowReader->getSelectedType());

        int idx = 0;
        while (rowReader->next(*batch))
        {
            printer->reset(*batch);
            for(unsigned long i=0; i < batch->numElements; ++i)
            {
                line.clear();
                printer->printRow(i);

                Print(out, std::string("Line") + std::to_string(idx++) + std::string(": "), line);
            }
        }
    }
    catch(...)
    {
        out << "catched ...." << std::endl;
    }
}




void ORCScanFile(std::ostream &out, const std::string &filename, uint64_t batchSize)
{
    Print(out, "ORCScanFile");
    Print(out, "filename", filename);

    char cwd[1024] = "";
    Print(out, "cwd", getcwd(cwd, sizeof(cwd)) ? cwd : "NA");

    try
    {
        std::unique_ptr<orc::InputStream>       file = orc::readLocalFile(filename);

        orc::ReaderOptions                      readerOpts;
        std::unique_ptr<orc::Reader>            reader    = orc::createReader(orc::readLocalFile(filename), readerOpts);
        std::unique_ptr<orc::RowReader>         rowReader = reader->createRowReader();
        std::unique_ptr<orc::ColumnVectorBatch> batch     = rowReader->createRowBatch(batchSize);


        unsigned long rows    = 0;
        unsigned long batches = 0;
        while (rowReader->next(*batch))
        {
            batches += 1;
            rows += batch->numElements;
        }

        Print(out, "Rows"    , std::to_string(rows   ));
        Print(out, "Batches" , std::to_string(batches));

  }
  catch(...)
  {
      out << "catched ...." << std::endl;
  }
}

Datum ext_orc_scan(PG_FUNCTION_ARGS)
{
    text *orcFile = PG_GETARG_TEXT_PP(0);

    if (orcFile == nullptr || VARSIZE_ANY_EXHDR(orcFile) < 1)
    {
        ereport(ERROR,
            (
             errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
             errmsg("negative values are not allowed"),
             //errdetail("value %s is negative", orcFile),
             errhint("make it positive")
            )
        );
    }

    std::stringstream ss;
    Print (ss, "ext_orc_sca");
    Print (ss, "orcFile"    , VARDATA(orcFile)          );
    Print (ss, "orcFile Len", VARSIZE_ANY_EXHDR(orcFile));

    ORCScanFile(ss, std::move(std::string(VARDATA(orcFile), VARSIZE_ANY_EXHDR(orcFile))), 128);

    PG_RETURN_TEXT_P(cstring_to_text(ss.str().c_str()));
}

Datum ext_orc_content(PG_FUNCTION_ARGS)
{
    text *orcFile = PG_GETARG_TEXT_PP(0);

    if (orcFile == nullptr || VARSIZE_ANY_EXHDR(orcFile) < 1)
    {
        ereport(ERROR,
            (
             errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
             errmsg("Invalid usage"),
             //errdetail("value %s is negative", orcFile),
             errhint("Usage : SELECT ext_orc_scan('/home/postgres/projects/ext_orc/TestOrcFile.test1.orc');")
            )
        );
    }



    std::stringstream ss;
    Print (ss, "ext_orc_content");
    Print (ss, "orcFile"    , VARDATA(orcFile)          );
    Print (ss, "orcFile Len", VARSIZE_ANY_EXHDR(orcFile));

    orc::RowReaderOptions rowReaderOpts;
    ORCPrintContents(ss, std::move(std::string(VARDATA(orcFile), VARSIZE_ANY_EXHDR(orcFile))), rowReaderOpts);


    PG_RETURN_TEXT_P(cstring_to_text(ss.str().c_str()));
}



#ifdef __cplusplus
extern "C" {
#endif

#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"


PG_MODULE_MAGIC;


Datum base36_encode(PG_FUNCTION_ARGS);


PG_FUNCTION_INFO_V1(base36_encode);


class Base34SAMPLE
{
    public :
        char operator [](unsigned int idx) { return _base36[idx]; }

    private :
        char _base36[37] = "0123456789abcdefghijklmnopqrstuvwxyz";
};

Datum base36_encode(PG_FUNCTION_ARGS)
{
    int32 arg = PG_GETARG_INT32(0);

    if (arg < 0)
        ereport(ERROR,
            (
             errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
             errmsg("negative values are not allowed"),
             errdetail("value %d is negative", arg),
             errhint("make it positive")
            )
        );

    Base34SAMPLE b36;

    const int len = 7; /* max 6 char + '\0' */
    char *buffer = (char *)palloc(len * sizeof(char));

    unsigned int offset = len;//sizeof(buffer);
    buffer[--offset] = '\0';

    do {
        buffer[--offset] = b36[arg % 36];
    } while (arg /= 36);


    PG_RETURN_TEXT_P(cstring_to_text(&buffer[offset]));
}


#ifdef __cplusplus
}
#endif

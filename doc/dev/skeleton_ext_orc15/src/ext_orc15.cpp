#include "ext_orc15.h"

#ifdef __cplusplus
extern "C" { 
#endif

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(ext_orc15_in);
PG_FUNCTION_INFO_V1(ext_orc15_out);

#ifdef __cplusplus
} 
#endif


const char ext_orc15_digits[37] = "0123456789abcdefghijklmnopqrstuvwxyz";


Datum
ext_orc15_in(PG_FUNCTION_ARGS)
{
    long result;
    char *bad;
    char *str = PG_GETARG_CSTRING(0);
    result = strtol(str, &bad, 36);
    if (result < INT_MIN || result > INT_MAX)
        EXT_ORC15_OUTOFRANGE_ERROR(str, "ext_orc15");

    if (bad[0] != '\0' || strlen(str)==0)
        EXT_ORC15_SYNTAX_ERROR(str,"ext_orc15");

    PG_RETURN_INT32((int32)result);
}


Datum
ext_orc15_out(PG_FUNCTION_ARGS)
{
    char buffer[8];
    unsigned int offset = sizeof(buffer);

    int32 arg = PG_GETARG_INT32(0);
    bool  neg = false;

    if (arg < 0)
    {
        arg = -arg;
        neg = true;
    }

    /* max 6 char + '\0' + sign*/
    buffer[--offset] = '\0';

    do {
        buffer[--offset] = ext_orc15_digits[arg % 36];
    } while (arg /= 36);

    if (neg)
        buffer[--offset] = '-';

    PG_RETURN_CSTRING(pstrdup(&buffer[offset]));
}


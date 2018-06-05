#include "ext_orc15.h"



#ifdef __cplusplus
extern "C" { 
#endif

PG_FUNCTION_INFO_V1(big_ext_orc15_in);
PG_FUNCTION_INFO_V1(big_ext_orc15_out);


#ifdef __cplusplus
} 
#endif






Datum
big_ext_orc15_in(PG_FUNCTION_ARGS)
{
    long result;
    char *bad;
    char *str = PG_GETARG_CSTRING(0);
    result = strtol(str, &bad, 36);

    if (result == LONG_MIN || result == LONG_MAX)
        EXT_ORC15_OUTOFRANGE_ERROR(str, "big_ext_orc15");

    if (bad[0] != '\0' || strlen(str)==0)
        EXT_ORC15_SYNTAX_ERROR(str,"big_ext_orc15");

    PG_RETURN_INT64((int64)result);
}


Datum
big_ext_orc15_out(PG_FUNCTION_ARGS)
{
    char buffer[15];
    unsigned int offset = sizeof(buffer);
    
    int64 arg = PG_GETARG_INT64(0);
    bool  neg = false;
    
    if (arg <= LONG_MIN || arg >= LONG_MAX)
        ereport(ERROR,
        (
         errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
         errmsg("big_ext_orc15 out of range")
        ));

    if (arg < 0)
    {
        arg = -arg;
        neg = true;
    }

    /* max 13 char + '\0' + sign */

    buffer[--offset] = '\0';

    do {
        buffer[--offset] = ext_orc15_digits[arg % 36];
    } while (arg /= 36);

    if (neg)
        buffer[--offset] = '-';

    PG_RETURN_CSTRING(pstrdup(&buffer[offset]));
}
#ifndef EXT_ORC15_H
#define EXT_ORC15_H

#ifdef __cplusplus
extern "C" { 
#endif

#include "postgres.h"

#ifdef __cplusplus
} 
#endif


#include "utils/builtins.h"
#include "utils/int8.h"
#include "libpq/pqformat.h"
#include <limits.h>

extern const char ext_orc15_digits[37];

#define EXT_ORC15_OUTOFRANGE_ERROR(_str, _typ)                  \
  do {                                                          \
    ereport(ERROR,                                              \
      (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),             \
        errmsg("value \"%s\" is out of range for type %s",      \
          _str, _typ)));                                        \
  } while(0)                                                    \


#define EXT_ORC15_SYNTAX_ERROR(_str, _typ)                      \
  do {                                                          \
    ereport(ERROR,                                              \
      (errcode(ERRCODE_SYNTAX_ERROR),                           \
      errmsg("invalid input syntax for %s: \"%s\"",             \
             _typ, _str)));                                     \
  } while(0)                                                    \


#endif // EXT_ORC15_H

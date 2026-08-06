#include <stdint.h>

/* An infinte loop (for loop with no init, condition, or increment)*/
#define loop for(;;)

/* Visual sugar for C23's __auto_type */
#define var __auto_type

/* Visual sugar for C23's __auto_type (constant) */
#define let const __auto_type

/* An 8 bit integer */
typedef int8_t           i8;

/* A 16 bit integer */
typedef int16_t          i16;

/* A 32 bit integer */
typedef int32_t          i32;

/* A 64 bit integer */
typedef int64_t          i64;

/* An 8 bit unsigned integer */
typedef uint8_t          u8;

/* A 16 bit unsigned integer */
typedef uint16_t         u16;

/* A 32 bit unsigned integer */
typedef uint32_t         u32;

/* A 64 bit unsigned integer */
typedef uint64_t         u64;

/* A signed pointer-sized integer */
typedef intptr_t         isize;

/* An unsigned pointer-sized integer */
typedef uintptr_t        usize;

/* An at least 8 bit integer */
typedef int_least8_t     i8l;

/* An at least 16 bit integer */
typedef int_least16_t    i16l;

/* An at least 32 bit integer */
typedef int_least32_t    i32l;

/* An at least 64 bit integer */
typedef int_least64_t    i64l;

/* An unsigned at least 8 bit integer */
typedef uint_least8_t    u8l;

/* An unsigned at least 16 bit integer */
typedef uint_least16_t   u16l;

/* An unsigned at least 32 bit integer */
typedef uint_least32_t   u32l;

/* An unsigned at least 64 bit integer */
typedef uint_least64_t   u64l;

/* Fastest available integer type, at least 8 bits */
typedef int_fast8_t      i8f;

/* Fastest available integer type, at least 16 bits */
typedef int_fast16_t     i16f;

/* Fastest available integer type, at least 32 bits */
typedef int_fast32_t     i32f;

/* Fastest available integer type, at least 64 bits */
typedef int_fast64_t     i64f;

/* Fastest available unsigned integer type, at least 8 bits */
typedef uint_fast8_t     u8f;

/* Fastest available unsigned integer type, at least 16 bits */
typedef uint_fast16_t    u16f;

/* Fastest available unsigned integer type, at least 32 bits */
typedef uint_fast32_t    u32f;

/* Fastest available unsigned integer type, at least 64 bits */
typedef uint_fast64_t    u64f;
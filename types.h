#ifndef typesH
#define typesH

//typedef unsigned char uint8_t;
//typedef signed char int8_t;
//typedef unsigned short uint16_t;
//typedef signed short int16_t;
//typedef unsigned long uint32_t;
//typedef signed long int32_t;
typedef unsigned __int64 u64;
typedef unsigned __int64 uint64_t;

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;

typedef union
{
 struct
 {
  u32 l;
  u32 h;
 };
 struct
 {
  u64 u;
 };
}u64l;

#endif

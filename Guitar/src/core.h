#pragma once
#include <stdint.h>

typedef struct { float x; float y; } fvec2;
typedef struct { int x; int y; } ivec2;

// Eddie ate dynamite, goob bye eddie :(
enum StringName
{
    E, A, D, G, B, e, NUMBER_OF_STRINGS
};

#define internal static
#define true	 1
#define false	 0

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t  i8;

typedef float   f32;
typedef double  f64;

typedef char    byte;

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 4096
#define NUMBER_OF_FRETS 24


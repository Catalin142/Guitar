#pragma once
#include "core.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    u32 frequency;
    u32 length;

    u32 currentIndex;
    f32 previousValue;

    float* wavetable;
    u8 isRinging;

} GuitarString;

GuitarString createString(u32 freq);
void destroyString(GuitarString string);

float lowPassFilterNext(GuitarString* string);

u32 getStringByName(char name);
char getStringByNumber(u32 name);
#pragma once
#include "guitar_string.h"

#include <math.h>
#include "tabulature.h"

enum Technique
{
    STRUMMING,
    PICKING
};

enum Tunings
{
    STANDARD,
    ONE_HALF_STEP_DOWN
};

typedef struct
{
    GuitarString strings[NUMBER_OF_STRINGS];
    f32 randomTable[SAMPLE_RATE / 82]; // 82 = lowest frequency

    u32 frequencyTable[6];
    f32 fretPosition[NUMBER_OF_FRETS + 1];
    fvec2 stringSection[NUMBER_OF_STRINGS];

    f32 masterVolume;

    u32 scale;
    u32 fretboardHeightInPixels;

    u8 technique;
    u32 currentString;
    u8 currentStrumIndex;

    u32 tuning;

} Guitar;

Guitar createGuitar();
void destroyGuitar(Guitar guitar);

void scaleFretsToResolution(Guitar* guitar, u32 width);

void pluckString(Guitar* guitar, u32 stringName, u32 fret);
void muteString(Guitar* guitar, u32 stringName);

void updateGuitar(Guitar* guitar);

void playTab(Guitar* guitar, Tabulature* tab, f32 deltaTime);
#include "guitar.h"

u32 getFreqency(u32 string, u32 tuning)
{
    static const u32 standardFreqencyTable[6] = { 82, 110, 147, 196, 247, 330 };
    // program breaks if the frequency is lower than 81
    static const u32 OHSDfreqencyTable[6]     = { 82, 103, 138, 184, 233, 311 };
    switch (tuning)
    {
    case STANDARD:
        return standardFreqencyTable[string];
    case ONE_HALF_STEP_DOWN:
        return OHSDfreqencyTable[string];
    }

    return 82;
}

Guitar createGuitar()
{
    Guitar guitar;
    guitar.tuning = STANDARD;

    // generating a random table, at the start, so no need to regenerate new numbers every time
    for (u32 i = 0; i < 44100 / getFreqency(E, guitar.tuning); i++) guitar.randomTable[i] = ((float)rand() / (float)RAND_MAX) - 0.5f;

    guitar.strings[E] = createString(getFreqency(E, guitar.tuning));
    guitar.strings[A] = createString(getFreqency(A, guitar.tuning));
    guitar.strings[D] = createString(getFreqency(D, guitar.tuning));
    guitar.strings[G] = createString(getFreqency(G, guitar.tuning));
    guitar.strings[B] = createString(getFreqency(B, guitar.tuning));
    guitar.strings[e] = createString(getFreqency(e, guitar.tuning));

    guitar.scale = 645;
    guitar.fretboardHeightInPixels = 150;

    guitar.currentString = 0;
    guitar.currentStrumIndex = 0;

    guitar.fretPosition[0] = 0.0f;
    {
        float nutConstant = 17.817f;
        float fretScale = (float)guitar.scale;
        float prevDistance = 0.0f;

        for (i32 i = 1; i <= NUMBER_OF_FRETS; i++)
        {
            float distanceToNextFret = fretScale / nutConstant;
            guitar.fretPosition[i] = prevDistance + fretScale / nutConstant;
            prevDistance = guitar.fretPosition[i];
            fretScale -= distanceToNextFret;
        }

        for (i32 i = 1; i <= NUMBER_OF_FRETS; i++)
            guitar.fretPosition[i] /= guitar.fretPosition[NUMBER_OF_FRETS];
    }

    {
        f32 stringHeight = guitar.fretboardHeightInPixels / (f32)NUMBER_OF_STRINGS;
        f32 stringStart = -3.0f;

        for (int i = NUMBER_OF_STRINGS - 1; i >= 0; i--)
        {
            guitar.stringSection[i].x = stringStart;
            guitar.stringSection[i].y = stringStart + stringHeight;

            stringStart += stringHeight + 1;
        }
    }

    return guitar;
}

void destroyGuitar(Guitar guitar)
{
    for (int i = 0; i < NUMBER_OF_STRINGS; i++)
        destroyString(guitar.strings[i]);
}

void pluckString(Guitar* guitar, u32 stringName, u32 fret)
{
    GuitarString* string = &guitar->strings[stringName];
    memcpy(string->wavetable, guitar->randomTable, string->length * sizeof(f32));
    string->currentIndex    = 0;
    string->isRinging       = true;
    string->previousValue   = 0.1f;
    string->frequency       = (u32)((f64)getFreqency(stringName, guitar->tuning) * pow(2.0, (f64)fret / 12.0));
    string->length          = SAMPLE_RATE / string->frequency;
}

void muteString(Guitar* guitar, u32 stringName)
{
    GuitarString* string = &guitar->strings[stringName];
    string->currentIndex = 0;
    string->isRinging = false;
    string->previousValue = 0.1f;
}

void updateGuitar(Guitar* guitar)
{
    for (int i = 0; i < NUMBER_OF_STRINGS; i++)
    {
        GuitarString* string = &guitar->strings[i];
        double absValue = fabs(string->previousValue);
        if (absValue < 0.0000001 && string->isRinging)
            string->isRinging = false;
    }
}

void scaleFretsToResolution(Guitar* guitar, u32 width)
{
    for (i32 i = 1; i <= NUMBER_OF_FRETS; i++)
        guitar->fretPosition[i] *= width;
}

internal pluckSectionString(Guitar* guitar, Tabulature* tab, u32 string)
{
    u32 currentTabSection = tab->currentSection;

    StringSection section = tab->stringFrets[string];
    u8 fret = section.frets[currentTabSection];
    if (fret != NO_SOUND_TAB && currentTabSection < section.numberOfSections)
        pluckString(guitar, string, fret);
}

void playTab(Guitar* guitar, Tabulature* tab, f32 deltaTime)
{
    f32 secBetweenBeats = 60.0f / (f32)tab->bpm;

    if (tab->currentTime <= secBetweenBeats)
    {
        tab->currentTime += deltaTime;
        return;
    }

    u8 emptyStrum = true;
    for (i32 i = 0; i < NUMBER_OF_STRINGS; i++)
        if (tab->stringFrets[i].frets[tab->currentSection] != NO_SOUND_TAB)
            emptyStrum = false;

    if (emptyStrum)
    {
        tab->currentTime = secBetweenBeats;
        tab->currentSection += 1;
        tab->currentSection %= tab->maxNumberOfSections;
        return;
    }

    switch (guitar->technique)
    {
    case STRUMMING:
        pluckSectionString(guitar, tab, guitar->currentString);
        
        u8 strumIndex = guitar->currentStrumIndex;
        if (tab->strummingPattern[strumIndex] == DOWN)
            guitar->currentString++;
        else
            guitar->currentString--;
        
        if (guitar->currentString >= NUMBER_OF_STRINGS)
        {
            tab->currentTime = 0.0f;
            tab->currentSection += 1;
            tab->currentSection %= tab->maxNumberOfSections;

            u8 oldStrum = tab->strummingPattern[guitar->currentStrumIndex];

            guitar->currentStrumIndex++;
            guitar->currentStrumIndex %= tab->numberOfStrums;

            u8 strumIndex = guitar->currentStrumIndex;
            if (tab->strummingPattern[strumIndex] == DOWN)
                guitar->currentString = E;
            else
                guitar->currentString = e;
            
            u8 currentStrum = tab->strummingPattern[guitar->currentStrumIndex];
            if (oldStrum != currentStrum)
                tab->currentTime = secBetweenBeats / 2.0f;
        }

        break;

    case PICKING:
        for (i32 i = 0; i < NUMBER_OF_STRINGS; i++)
            pluckSectionString(guitar, tab, i);

        tab->currentTime = 0.0f;
        tab->currentSection += 1;
        tab->currentSection %= tab->maxNumberOfSections;

        break;

    default:
        break;
    }
}

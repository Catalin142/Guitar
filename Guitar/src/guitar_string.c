#include "guitar_string.h"

GuitarString createString(u32 freq)
{
    GuitarString string;
    string.frequency = freq;
    string.length = SAMPLE_RATE / freq;
    string.currentIndex = 0;
    string.previousValue = 0.0f;
    string.wavetable = (float*)malloc(string.length * sizeof(float));
    string.isRinging = false;

    return string;
}

void destroyString(GuitarString string)
{
    free(string.wavetable);
}

float lowPassFilterNext(GuitarString* string)
{
    float* output = &string->wavetable[string->currentIndex];
    float valueToReturn = *output;

    *output = ((*output + string->previousValue) * 0.5f);
    string->previousValue = valueToReturn;
    string->currentIndex++;
    string->currentIndex %= string->length;

    return valueToReturn;
}

u32 getStringByName(char name)
{
	switch (name)
	{
	case 'E': return E;
	case 'A': return A;
	case 'D': return D;
	case 'G': return G;
	case 'B': return B;
	case 'e': return e;
	}
}

char getStringByNumber(u32 name)
{
	switch (name)
	{
	case E: return 'E';
	case A: return 'A';
	case D: return 'D';
	case G: return 'G';
	case B: return 'B';
	case e: return 'e';
	}
}
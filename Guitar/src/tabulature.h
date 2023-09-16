#pragma once
#include "core.h"

#define MAX_NUMBER_OF_SECTIONS 300
#define NO_SOUND_TAB NUMBER_OF_FRETS + 1

typedef struct
{
	u8 frets[MAX_NUMBER_OF_SECTIONS];
	u32 numberOfSections;

} StringSection;

enum StrumDirection
{
	DOWN, UP
};

typedef struct 
{
	StringSection stringFrets[NUMBER_OF_STRINGS];
	u8 letRing[MAX_NUMBER_OF_SECTIONS];
	u32 maxNumberOfSections;
	u32 bpm;
	f32 currentTime;
	u32 currentSection;

	u8 strummingPattern[100];
	u8 numberOfStrums;

} Tabulature;

Tabulature loadTabs(const char* filepath);


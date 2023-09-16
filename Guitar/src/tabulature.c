#include "tabulature.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "guitar.h"

internal 
void readStringTab(Tabulature* tab, u32 string, char* line)
{
	size_t lineSize = strlen(line);
	u32 currentSection = 0;
	for (size_t i = 2; i < lineSize - 1; i++)
	{
		if (line[i] == ' ')
			continue;

		if (line[i] == '-')
			tab->stringFrets[string].frets[currentSection++] = NO_SOUND_TAB;
		else
		{
			char fst = line[i];
			char scd = line[i + 1];
			if (fst != '-' && scd != '-')
			{
				tab->stringFrets[string].frets[currentSection++] = NO_SOUND_TAB;
				tab->stringFrets[string].frets[currentSection++] = (fst - '0') * 10 + (scd - '0'), i++;
			}
			else
				tab->stringFrets[string].frets[currentSection++] = (fst - '0');
		}
	}

	tab->stringFrets[string].numberOfSections = currentSection;
}

Tabulature loadTabs(const char* filepath)
{
	Tabulature tab;
	tab.maxNumberOfSections	= 0;
	tab.currentSection		= 0;
	tab.currentTime			= 0.0f;

	FILE* fp = fopen(filepath, "r");
	if (fp == NULL) 
	{
		perror("Unable to open file!");
		exit(1);
	}

	char line[500];

	while (fgets(line, sizeof(line), fp)) {
		if (line[0] == 'b')
		{
			size_t lineSize = strlen(line);
			u32 bpm = 0;
			for (size_t i = 2; i < lineSize - 1; i++)
				bpm = bpm * 10 + (line[i] - '0');

			tab.bpm = bpm;
		}

		else if (line[0] == 's')
		{
			size_t lineSize = strlen(line);
			tab.numberOfStrums = 0;
			for (int i = 2; i < lineSize; i++)
			{
				if (line[i] == 'D')
					tab.strummingPattern[i - 2] = DOWN;
				else
					tab.strummingPattern[i - 2] = UP;
				tab.numberOfStrums++;
			}
		}

		else readStringTab(&tab, getStringByName(line[0]), line);
	}

	for (i32 i = 0; i < NUMBER_OF_STRINGS; i++)
		if (tab.stringFrets[i].numberOfSections > tab.maxNumberOfSections)
			tab.maxNumberOfSections = tab.stringFrets[i].numberOfSections;

	fclose(fp);
	return tab;
}

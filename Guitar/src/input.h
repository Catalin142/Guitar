#pragma once
#include "core.h"
#include "SDL.h"

typedef struct
{
    u8 keys, old_keys;
    ivec2 mousePosition;
    u8 closeEvent;

} Input;

enum UsedKeys
{
    NUM_1,
    NUM_2,
    NUM_3,
    NUM_4,
    NUM_5,
    NUM_6,
    LMB,
    RMB
};

u8 pressed(Input input, u32 key);
u8 released(Input input, u32 key);

void setState(u8* keys, u32 key, u8 value);
u8 isPressed(u8 keys, u32 key);
void handleKeys(u32 key, u8* keys, u8 value);
void handleEvents(Input* input);
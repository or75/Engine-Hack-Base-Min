#ifndef __SVC_MESSAGE_H__
#define __SVC_MESSAGE_H__

// SVC Hook by _or_75

#include "Main.h"

class AutoOffset;

extern int* MSG_ReadCount;
extern int* MSG_CurrentSize;
extern int* MSG_BadRead;
extern int MSG_SavedReadCount;

typedef void (*TEmptyCallback)();

typedef int (*HL_MSG_ReadByte)();
typedef int (*HL_MSG_ReadShort)();
typedef int (*HL_MSG_ReadLong)();
typedef float ( *HL_MSG_ReadFloat )( );
typedef char* (*HL_MSG_ReadString)();

typedef void (*HL_MSG_CBuf_AddText)(char* text);

extern HL_MSG_ReadByte MSG_ReadByte;
extern HL_MSG_ReadShort MSG_ReadShort;
extern HL_MSG_ReadLong MSG_ReadLong;
extern HL_MSG_ReadFloat MSG_ReadFloat;
extern HL_MSG_ReadString MSG_ReadString;

extern HL_MSG_CBuf_AddText CBuf_AddText_Orign;

extern TEmptyCallback SVC_StuffText_Orign;
extern TEmptyCallback SVC_SendCvarValue_Orign;
extern TEmptyCallback SVC_SendCvarValue2_Orign;
extern TEmptyCallback SVC_Director_Orign;
extern TEmptyCallback SVC_Sound_Orign;

void MSG_SaveReadCount();
void MSG_RestoreReadCount();

TEmptyCallback HookServerMsg(const unsigned Index, void* CallBack,AutoOffset* Offset);

bool IsCvarGood(const char *str);
bool IsCommandGood(const char *str);
bool SanitizeCommands(char *str);

void CBuf_AddText(char* text);
void SVC_StuffText();
void SVC_SendCvarValue();
void SVC_SendCvarValue2();
void SVC_Director();
void SVC_Sound();

#endif
#include "SvcMessage.h"

#pragma warning(disable:4996)

// SVC Hook by _or_75

// ���� ��������� �����/������� , ������ �� �� ������ � ��� ���������
static const char* blockList[] = 
{
	"exit","quit","bind","unbind","unbindall","kill","exec","alias",
	"clear","motdfile","motd_write","writecfg","cd","echo","cmd",
	"developer","fps_max","fps_modem","disconnect","connect","motd","name",
	"csx_setcvar","set","screenshot"
};

// ���� ��������� ����� , ������ �������� �� �� ������
static const char* blockListCvar[] = 
{
	"developer","fps_max","fps_modem"
};

#define MAX_CMD_LINE 1024

int* MSG_ReadCount = nullptr;
int* MSG_CurrentSize = nullptr;
int* MSG_BadRead = nullptr;
int MSG_SavedReadCount = 0;

char com_token[1024];

HL_MSG_ReadByte MSG_ReadByte = nullptr;
HL_MSG_ReadShort MSG_ReadShort = nullptr;
HL_MSG_ReadLong MSG_ReadLong = nullptr;
HL_MSG_ReadFloat MSG_ReadFloat = nullptr;
HL_MSG_ReadString MSG_ReadString = nullptr;

HL_MSG_CBuf_AddText CBuf_AddText_Orign = nullptr;

TEmptyCallback SVC_StuffText_Orign = nullptr;
TEmptyCallback SVC_SendCvarValue_Orign = nullptr;
TEmptyCallback SVC_SendCvarValue2_Orign = nullptr;
TEmptyCallback SVC_Director_Orign = nullptr;
TEmptyCallback SVC_Sound_Orign = nullptr;

TEmptyCallback HookServerMsg(const unsigned Index, void* CallBack,AutoOffset* Offset)
{
	TEmptyCallback Call = (TEmptyCallback)Offset->SVCBase[Index]->Callback;
	Offset->SVCBase[Index]->Callback = (DWORD)CallBack;
	return Call;
}

void MSG_SaveReadCount()
{
	MSG_SavedReadCount = *MSG_ReadCount;
}

void MSG_RestoreReadCount()
{
	*MSG_ReadCount = MSG_SavedReadCount;
}

void CBuf_AddText(char* text)
{
	char str[1024];
	strncpy(str, text, sizeof(str));
	str[sizeof(str) - 1] = 0;

	if (SanitizeCommands(str))
		return;

	CBuf_AddText_Orign(text);
}

void SVC_StuffText() // server send cmd
{
	MSG_SaveReadCount();
	char* commands = MSG_ReadString();

	char str[1024];
	strncpy(str, commands, sizeof(str));
	str[sizeof(str) - 1] = 0;

	if (SanitizeCommands(str))
		return;

	MSG_RestoreReadCount();
	SVC_StuffText_Orign();
}

void SVC_SendCvarValue()
{
	MSG_SaveReadCount();
	char* cvar = MSG_ReadString();
	
	bool isGood = IsCvarGood(cvar);
	if (!isGood)
	{
		g_Engine.Con_Printf("\t\t\t\t\t\t\t\t\t[HLR] Server tried to request blocked cvar: %s\n", cvar);
		return;
	}
	else
		g_Engine.Con_Printf("\t\t\t\t\t\t\t\t\t[HLR] Server tried to request not blocked cvar: %s\n", cvar);

	MSG_RestoreReadCount();	
	SVC_SendCvarValue_Orign();
}

void SVC_SendCvarValue2()
{
	MSG_SaveReadCount();
	char* cvar = MSG_ReadString();

	bool isGood = IsCvarGood(cvar);
	if (!isGood)
	{
		g_Engine.Con_Printf("\t\t\t\t\t\t\t\t\t[HLR] Server tried to request blocked cvar: %s\n", cvar);
		return;
	}
	else
		g_Engine.Con_Printf("\t\t\t\t\t\t\t\t\t[HLR] Server tried to request not blocked cvar: %s\n", cvar);

	MSG_RestoreReadCount();	
	SVC_SendCvarValue2_Orign();
}

void SVC_Director()
{
	MSG_SaveReadCount();
	int msglen = MSG_ReadByte();
	int msgtype = MSG_ReadByte();
	char* commands = MSG_ReadString();
	if ( msgtype == 10 ) // DRC_CMD_STUFFTEXT
	{
		g_Engine.Con_Printf( "\t\t\t\t\t\t\t\t\t[HLR] Server tried to execute command by cls exec: %s\n" , commands );
		return;
	}
	MSG_RestoreReadCount();
	SVC_Director_Orign();
}

void SVC_Sound()
{
	MSG_SaveReadCount();

	int Flag = MSG_ReadByte();
	int Volume = MSG_ReadByte();
	int Attenuation = MSG_ReadByte();
	int Channel = MSG_ReadByte();
	int EntityIndex = MSG_ReadByte();
	int SoundIndex = MSG_ReadByte();

	int x = MSG_ReadByte() * 8;
	int y = MSG_ReadByte() * 8 * 1;
	int z = MSG_ReadByte() * 8 * 2;

	g_Engine.Con_Printf( "SVC_Sound: [%i,%i,%i]\n" , x , y , z );

	MSG_RestoreReadCount();
	SVC_Sound_Orign();
}

char ToUpper(char ch)
{
    return (char)(ch+32);
}

bool ParseListCvar(const char* str)
{
	DWORD temp = 0;
	DWORD len = sizeof(str)-1;
	for(DWORD i = 0;i < _countof(blockListCvar);i++)
	{
		for(DWORD x = 0;x < len;x++)
		{
			if ( blockListCvar[i][x] == str[x] || blockListCvar[i][x] == ToUpper(str[x]) )
			{
				temp++;
				if ( temp == len )
					return true;
			}
			else temp = 0;
		}
	}
	return false;
}

bool ParseList(const char* str)
{
	DWORD temp = 0;
	DWORD len = sizeof(str)-1;
	for(DWORD i = 0;i < _countof(blockList);i++)
	{
		for(DWORD x = 0;x < len;x++)
		{
			if ( blockList[i][x] == str[x] || blockList[i][x] == ToUpper(str[x]) )
			{
				temp++;
				if ( temp == len )
					return true;
			}
			else temp = 0;
		}
	}
	return false;
}

bool IsCvarGood(const char *str)
{
	if (str[0] == 0)
		return true;

	if (ParseListCvar(str))
		return false;

	return true;
}

bool IsCommandGood(const char *str)
{
	char *ret = g_Engine.COM_ParseFile((char *)str, com_token);
	if (ret == NULL || com_token[0] == 0)
		return true;
	if ((ParseList(com_token)))
		return false;
	return true;
}

bool SanitizeCommands(char *str)
{
	bool changed = false;
	char *text = str;
	char command[MAX_CMD_LINE];
	int i, quotes;
	int len = strlen(str);

	while (text[0] != 0)
	{
		quotes = 0;
		for (i = 0; i < len; i++)
		{
			if (text[i] == '\"') quotes++;
			if (!(quotes & 1) && text[i] == ';')
				break;
			if (text[i] == '\n')
				break;
			if (text[i] == 0x00)
				break;
		}
		if (i >= MAX_CMD_LINE)
			i = MAX_CMD_LINE;

		strncpy(command, text, i);
		command[i] = 0;

		bool isGood = IsCommandGood(command);

		int log = 1;
		if (log > 0)
		{
			char *c = command;
			char *a = isGood ? 
				"\t\t\t\t\t\t\t\t\t[HLR] Server execute command: %s\n": 
				"\t\t\t\t\t\t\t\t\t[HLR] Server blocked command: %s\n";
			g_Engine.Con_Printf(a,c);
		}
		len -= i;
		if (!isGood)
		{
			strncpy(text, text + i, len);
			text[len] = 0;
			text++;
			changed = true;
		}
		else
		{
			text += i + 1;
		}
	}
	return changed;
}
#include <windows.h>
#include <stdio.h>
#pragma pack(1)


HINSTANCE hLThis = 0;
HINSTANCE hL = 0;
FARPROC p[21] = {0};

unsigned char buf[9];
char* str;
int ebxBkp;

BOOL WINAPI DllMain(HINSTANCE hInst,DWORD reason,LPVOID)
	{
	if (reason == DLL_PROCESS_ATTACH)
		{
		hLThis = hInst;
		hL = LoadLibrary(".\\HIDApi_original.dll");
		if (!hL) return false;

		p[0] = GetProcAddress(hL,"CloseUSB");
		p[1] = GetProcAddress(hL,"DecodeSecrecy_V11");
		p[2] = GetProcAddress(hL,"EncodeSecrecy_V11");
		p[3] = GetProcAddress(hL,"Get2Version");
		p[4] = GetProcAddress(hL,"GetFeature");
		p[5] = GetProcAddress(hL,"GetInformation");
		p[6] = GetProcAddress(hL,"GetPCTimer");
		p[7] = GetProcAddress(hL,"GetSN_Code");
		p[8] = GetProcAddress(hL,"InitUSB");
		p[9] = GetProcAddress(hL,"LoadPCtimer");
		p[10] = GetProcAddress(hL,"OpenUSB");
		p[11] = GetProcAddress(hL,"ReadUSB");
		p[12] = GetProcAddress(hL,"SavePCtimerWithInDLL");
		p[13] = GetProcAddress(hL,"SavePctimer");
		p[14] = GetProcAddress(hL,"SetFeature");
		p[15] = GetProcAddress(hL,"SetReport");
		p[16] = GetProcAddress(hL,"SetSN_Code");
		p[17] = GetProcAddress(hL,"WriteUSB");
		p[18] = GetProcAddress(hL,"_OpenHIDDevice");
		p[19] = GetProcAddress(hL,"change_PCTimer");
		p[20] = GetProcAddress(hL,"setBufferLength");

		OutputDebugString("Hook library loaded!");

		}
	if (reason == DLL_PROCESS_DETACH)
		{
		FreeLibrary(hL);
		}

	return 1;
	}

// CloseUSB
extern "C" __declspec(naked) void __stdcall __E__0__()
	{
	__asm
		{
		jmp p[0*4];
		}
	}

// DecodeSecrecy_V11
extern "C" __declspec(naked) void __stdcall __E__1__()
	{
	__asm
		{
		jmp p[1*4];
		}
	}

extern "C" void printBuf() {
	str = (char*)malloc(1000);
	sprintf(str, "sendreport %02X %02X %02X %02X %02X %02X %02X %02X %02X", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8]);
	OutputDebugString(str);
	free(str);
}

// EncodeSecrecy_V11
extern "C" __declspec(naked) void __stdcall __E__2__()
	{
	__asm
		{
		mov ebxBkp, ebx;
		mov ebx, dword ptr [esp+0x4];
		mov eax, [ebx];
		mov buf[0], al;
		mov eax, [ebx+0x1];
		mov buf[1], al;
		mov eax, [ebx+0x2];
		mov buf[2], al;
		mov eax, [ebx+0x3];
		mov buf[3], al;
		mov eax, [ebx+0x4];
		mov buf[4], al;
		mov eax, [ebx+0x5];
		mov buf[5], al;
		mov eax, [ebx+0x6];
		mov buf[6], al;
		mov eax, [ebx+0x7];
		mov buf[7], al;
		mov eax, [ebx+0x8];
		mov buf[8], al;
		mov ebx, ebxBkp;
		}
	printBuf();
	__asm
		{
		jmp p[2*4];
		}
	}

// Get2Version
extern "C" __declspec(naked) void __stdcall __E__3__()
	{
	__asm
		{
		jmp p[3*4];
		}
	}

// GetFeature
extern "C" __declspec(naked) void __stdcall __E__4__()
	{
	__asm
		{
		jmp p[4*4];
		}
	}

// GetInformation
extern "C" __declspec(naked) void __stdcall __E__5__()
	{
	__asm
		{
		jmp p[5*4];
		}
	}

// GetPCTimer
extern "C" __declspec(naked) void __stdcall __E__6__()
	{
	__asm
		{
		jmp p[6*4];
		}
	}

// GetSN_Code
extern "C" __declspec(naked) void __stdcall __E__7__()
	{
	__asm
		{
		jmp p[7*4];
		}
	}

// InitUSB
extern "C" __declspec(naked) void __stdcall __E__8__()
	{
	__asm
		{
		jmp p[8*4];
		}
	}

// LoadPCtimer
extern "C" __declspec(naked) void __stdcall __E__9__()
	{
	__asm
		{
		jmp p[9*4];
		}
	}

// OpenUSB
extern "C" __declspec(naked) void __stdcall __E__10__()
	{
	__asm
		{
		jmp p[10*4];
		}
	}

// ReadUSB
extern "C" __declspec(naked) void __stdcall __E__11__()
	{
	__asm
		{
		jmp p[11*4];
		}
	}

// SavePCtimerWithInDLL
extern "C" __declspec(naked) void __stdcall __E__12__()
	{
	__asm
		{
		jmp p[12*4];
		}
	}

// SavePctimer
extern "C" __declspec(naked) void __stdcall __E__13__()
	{
	__asm
		{
		jmp p[13*4];
		}
	}

// SetFeature
extern "C" __declspec(naked) void __stdcall __E__14__()
	{
	__asm
		{
		jmp p[14*4];
		}
	}

// SetReport
extern "C" __declspec(naked) void __stdcall __E__15__()
	{
	__asm
		{
		jmp p[15*4];
		}
	}

// SetSN_Code
extern "C" __declspec(naked) void __stdcall __E__16__()
	{
	__asm
		{
		jmp p[16*4];
		}
	}

// WriteUSB
extern "C" __declspec(naked) void __stdcall __E__17__()
	{
	__asm
		{
		jmp p[17*4];
		}
	}

// _OpenHIDDevice
extern "C" __declspec(naked) void __stdcall __E__18__()
	{
	__asm
		{
		jmp p[18*4];
		}
	}

// change_PCTimer
extern "C" __declspec(naked) void __stdcall __E__19__()
	{
	__asm
		{
		jmp p[19*4];
		}
	}

// setBufferLength
extern "C" __declspec(naked) void __stdcall __E__20__()
	{
	__asm
		{
		jmp p[20*4];
		}
	}


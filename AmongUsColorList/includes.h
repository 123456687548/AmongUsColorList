#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <TlHelp32.h>
#include <comdef.h>
#include <string>
#include "kiero/kiero.h"
#include "imgui/imgui.h"
#include "imgui/imgui_memory_editor.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "il2cpp.h"

typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef uintptr_t PTR;

typedef void (__cdecl* RpcSetPet)(uintptr_t playerControl, int petId);
extern RpcSetPet _rpcSetPet;

typedef PlayerControl_o*(__cdecl* GetPlayerById)(void*, byte id);
extern GetPlayerById _getPlayerById;

typedef GameData_PlayerInfo_o* (__cdecl* GetPlayerInfo)(void*, byte id);
extern GetPlayerInfo _getPlayerInfo;

typedef System_Collections_Generic_List_GameData_PlayerInfo__o* PlayerListPtr;
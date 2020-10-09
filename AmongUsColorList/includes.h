#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <TlHelp32.h>
#include <comdef.h>
#include "kiero/kiero.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "il2cpp.h"

typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

typedef System_Collections_Generic_List_GameData_PlayerInfo__o* PlayerListPtr;
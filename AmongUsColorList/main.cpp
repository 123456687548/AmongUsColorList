#include "includes.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

uintptr_t gameAssemblyBase;
HMODULE hDllModule;
Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

DWORD amongUsClientAddress;

bool showMenu = true;
bool init = false;
bool rb_colorRight = false;

enum colors {
	RED, BLUE, GREEN, PINK, ORANGE, YELLOW, BLACK, WHITE, PURPLE, BROWN, CYAN, LIME
} colorIDs;

uintptr_t GetModuleBaseAddress(DWORD procId, const char* modName);
DWORD patternScan(const char* pattern, const char* mask, DWORD startaddress, DWORD endaddress);
void InitImGui();
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
void getColorString(int colorId, char* colorName, size_t bufferSize);
DWORD WINAPI MainThread(HMODULE hModule);

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved) {
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		//DisableThreadLibraryCalls(hMod);
		hDllModule = hMod;
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, nullptr, 0, nullptr);
		break;
	}
	return TRUE;
}

DWORD WINAPI MainThread(HMODULE hModule) {
	gameAssemblyBase = GetModuleBaseAddress(GetCurrentProcessId(), "GameAssembly.dll");
	amongUsClientAddress = *(DWORD*)(patternScan("\x74\x39\xA1\x00\x00\x00\x00\x8B\x40\x5C", "xxx????xxx", gameAssemblyBase, gameAssemblyBase + 0x1000000) + 0x3);

	bool init_hook = false;
	do {
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
			kiero::bind(8, (void**)&oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);

	while (true) {
		if (GetAsyncKeyState(VK_INSERT) & 1) {
			showMenu = !showMenu;
		}
		Sleep(100);
	}

	kiero::shutdown();
	FreeLibraryAndExitThread(hDllModule, 0);//crashes game, no idea why
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
	if (!init) {
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice))) {
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			init = true;
		}
		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}
	if (!showMenu) {
		return oPresent(pSwapChain, SyncInterval, Flags);
	}

	auto pClient = *reinterpret_cast<AmongUsClient_c**>(amongUsClientAddress); //neuste 21399616 / 0x1468840
	auto gamedata = pClient->static_fields->Instance->fields.GameDataPrefab->klass->static_fields->Instance;
	PlayerListPtr playerList = nullptr;
	if (gamedata != nullptr)
		playerList = gamedata->fields.AllPlayers;


	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Playerlist", 0, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Options")) {
			if (ImGui::MenuItem("Swap Name and Color position")) {
				rb_colorRight = !rb_colorRight;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	if (playerList != nullptr) {
		for (int i = 0; i < playerList->fields._size; i++) {
			auto player = playerList->fields._items->m_Items[i];
			if (player == nullptr) continue;
			auto name = player->fields.PlayerName->fields.wString;
			auto colorId = player->fields.ColorId;
			if (name == nullptr) continue;
			_bstr_t bstr(name);
			const char* szName = bstr;
			char colorName[16];
			getColorString(colorId, colorName, sizeof(colorName));

			if (rb_colorRight) {
				ImGui::Text(szName);
				ImGui::SameLine(80);
				ImGui::Text(" - ");
				ImGui::SameLine(110);
				ImGui::Text(colorName);
			}
			else {
				ImGui::Text(colorName);
				ImGui::SameLine(60);
				ImGui::Text(" - ");
				ImGui::SameLine(90);
				ImGui::Text(szName);
			}
		}
	}
	ImGui::End();

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

void getColorString(int colorId, char* colorName, size_t bufferSize) {
	switch (colorId) {
		case RED:
			strcpy_s(colorName, bufferSize, "Red");
			break;
		case BLUE:
			strcpy_s(colorName, bufferSize, "Blue");
			break;
		case GREEN:
			strcpy_s(colorName, bufferSize, "Green");
			break;
		case PINK:
			strcpy_s(colorName, bufferSize, "Pink");
			break;
		case ORANGE:
			strcpy_s(colorName, bufferSize, "Orange");
			break;
		case YELLOW:
			strcpy_s(colorName, bufferSize, "Yellow");
			break;
		case BLACK:
			strcpy_s(colorName, bufferSize, "Black");
			break;
		case WHITE:
			strcpy_s(colorName, bufferSize, "White");
			break;
		case PURPLE:
			strcpy_s(colorName, bufferSize, "Purple");
			break;
		case BROWN:
			strcpy_s(colorName, bufferSize, "Brown");
			break;
		case CYAN:
			strcpy_s(colorName, bufferSize, "Cyan");
			break;
		case LIME:
			strcpy_s(colorName, bufferSize, "Lime");
			break;
		default:
			strcpy_s(colorName, bufferSize, "Unknown Color");
	}
}

void InitImGui() {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

uintptr_t GetModuleBaseAddress(DWORD procId, const char* modName) {
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry)) {
			do {
				if (!_stricmp(modEntry.szModule, modName)) {
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

DWORD patternScan(const char* pattern, const char* mask, DWORD startaddress, DWORD endaddress) {
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	if (startaddress == 0) {
		startaddress = (DWORD)(si.lpMinimumApplicationAddress);
	}
	if (endaddress == 0) {
		endaddress = (DWORD)(si.lpMaximumApplicationAddress);
	}

	MEMORY_BASIC_INFORMATION mbi{ 0 };
	DWORD protectflags = (PAGE_GUARD | PAGE_NOCACHE | PAGE_NOACCESS);

	DWORD patternLength = (DWORD)strlen(mask);

	for (DWORD i = startaddress; i < endaddress - patternLength; i++) {
		if (VirtualQuery((LPCVOID)i, &mbi, sizeof(mbi))) {
			if (mbi.Protect & protectflags || !(mbi.State & MEM_COMMIT)) {
				//std::cout << "Bad Region! Region Base Address: " << mbi.BaseAddress << " | Region end address: " << std::hex << (int)((DWORD)mbi.BaseAddress + mbi.RegionSize) << std::endl;
				i += mbi.RegionSize;
				continue;
			}
			//std::cout << "Good Region! Region Base Address: " << mbi.BaseAddress << " | Region end address: " << std::hex << (int)((DWORD)mbi.BaseAddress + mbi.RegionSize) << std::endl;
			for (DWORD k = (DWORD)mbi.BaseAddress; k < (DWORD)mbi.BaseAddress + mbi.RegionSize - patternLength; k++) {
				for (DWORD j = 0; j < patternLength; j++) {
					if (mask[j] != '?' && pattern[j] != *(char*)(k + j)) {
						break;
					}
					if (j + 1 == patternLength && (char*)k != pattern) {
						//std::cout << "Address found at: " << std::hex << k << std::endl;
						return k;
					}
				}
			}
			i = (DWORD)mbi.BaseAddress + mbi.RegionSize;
		}
	}
	return NULL;
}
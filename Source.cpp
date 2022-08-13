#include <string_view>
#include <bitset>
#include <array>
#include <algorithm>
#include <vector>
#include <thread>

#include "def.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef min
#undef max

constexpr auto MAX_MODULE_COUNT{ 8 };

using _module_call = void(__fastcall*)();
using _module_call_int = void(__fastcall*)(int);

std::array<_module_call, MAX_MODULE_COUNT> on_audio_tick{};
std::array<_module_call, MAX_MODULE_COUNT> on_audio_tick_ingame{};
std::array<_module_call_int, MAX_MODULE_COUNT> on_mode_change{};
std::array<_module_call, MAX_MODULE_COUNT> on_menu_init{};

#include "init.h"

#include "modules/modules.h"

DWORD __stdcall DllMain(
	HINSTANCE hinstDLL,
	DWORD fdwReason,
	LPVOID lpReserved) {

	if (fdwReason == DLL_PROCESS_ATTACH)
		std::thread{ start_up::init }.detach();

	return 1;
}
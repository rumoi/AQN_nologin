#pragma once

#include "mem.h"

#include <string>

#include "osu_def.h"

using _osufunc = void(__fastcall*)();
using _osufunc_mouse_handler = void(__fastcall*)(osu_mouse_handler*);

using _osufunc_vec2 = void(__fastcall*)(vec2);

using _osufunc_GetKeyboardState = bool(__fastcall*)(u8*);
using _osufunc_ChangeMode = void(__fastcall*)(u32 newMode, u8 force);
using _osufunc_get_ForceNonExclusive = bool(__fastcall*)();

sprite_manager *volatile* sprite_cursor_layer;
_osufunc_mouse_handler OSU_MouseManager_ApplyHandler{};

_osufunc OSU_UpdateButtonsLocalPlayer{};
_osufunc_vec2 OSU_set_MousePosition;
_osufunc_vec2 OSU_SetCursorHandlerPositions;
_osufunc OSU_Player_UpdateIsPlaying;
_osufunc_GetKeyboardState OSU_GetKeyboardState;
_osufunc_ChangeMode OSU_ChangeMode;
_osufunc_get_ForceNonExclusive OSU_get_ForceNonExclusive;

struct {

	osu_GameMode *volatile* running_gamemode;

	pTexture *volatile* white_texture;

	u32 volatile* mode;
	int volatile* play_mode;
	int volatile* time;

	u8 volatile* is_paused;

	net_drawing_rectangle *volatile* client_bounds;

	double mod_play_speed{ 100. };
	u8 force_restart;

	u32 keybinding_ptr;

	struct {

		char K1{ 'Z' }, K2{ 'X' };

	} binding{};

	void update_keybinds() {

		u32 t = *(u32*)keybinding_ptr;
		t = *(u32*)(t + 8);

		binding.K1 = *(int*)(t + 0x14);
		binding.K2 = *(int*)(t + 0x24);

	}

} osu_data{};

namespace osu {

	void __fastcall MouseManager_SetPosition(vec2 pos) {

		OSU_set_MousePosition(pos);
		OSU_SetCursorHandlerPositions(pos);

	}

}

struct {

	bool active;

	vec2 pos;
	vec2 raw_pos;

} virtual_mouse;

struct {

	u32 active:1,
		K1 : 1, K2 : 1, smoke:1,		
		escape : 1, restart : 1;

} virtual_keyboard;

#include "osu_window.h"

#include "osu_tick.h"

void __fastcall mouse_hook(osu_mouse_handler* handler) {

	if (handler) {
		virtual_mouse.raw_pos.x = handler->position[0];
		virtual_mouse.raw_pos.y = handler->position[1];
	}

	OSU_MouseManager_ApplyHandler(handler);
}

bool __fastcall __GetKeyboardState(u8* state) {

	const auto ret = OSU_GetKeyboardState(state);
	
	if (state == 0)
		return 0;

	u8* v = state + 8;

	if (virtual_keyboard.active) {

		if (virtual_keyboard.escape)
			v[VK_ESCAPE] = 128;

		if (virtual_keyboard.K1)
			v[osu_data.binding.K1] = 128;

		if (virtual_keyboard.K2)
			v[osu_data.binding.K2] = 128;

	}

	if (v[VK_HOME] & 128) {

		if (AQM::toggle_held == 0 && *osu_data.mode != 2) {
			AQM::toggle_held = 1;
			AQM::toggle_menu(0, sprite_cursor_layer[0]->get_sprite(*AQM::menu_button));
		}

	} else AQM::toggle_held = 0;

	return ret; // osu checks for null instead of 0, so this does not really matter lmao
}

bool __fastcall __get_ForceNonExclusive() {

	on_menu_change();

	return OSU_get_ForceNonExclusive();

}

namespace start_up {

	namespace on_load {

		// Will all be JIT'ed by the title menu.

		constexpr auto sprite_manager_cursor = TO_AOB("74 0f 8b 0d ? ? ? ? 8b d3 39 09 e8 ? ? ? ? 6a 00 6a 00");

		constexpr auto mouse_state = TO_AOB("55 8b ec a1 ? ? ? ? a3 ? ? ? ? 83 3d ? ? ? ? 01 75");

		constexpr auto keyboard_hook = TO_AOB("33 f6 8b 5f 04 3b f3 73 ? f6 44 37 08 80");

		constexpr auto pause_check = TO_AOB("02 75 0e 80 3d ? ? ? ? 00 75 05 e9");

		constexpr auto osu_tick = TO_AOB("ff 15 ? ? ? ? 8d 7d ? 8b 35 ? ? ? ? 83 c6 04 f3 0f 7e 06");

		constexpr auto time_hook = TO_AOB("74 12 a1 ? ? ? ? 2b 05 ? ? ? ? a3");

		constexpr auto loaded_queue_mode = TO_AOB("89 45 f0 a1 ? ? ? ? a3 ? ? ? ? a1");

		constexpr auto handle_cursor_input = TO_AOB("d9 1c 24 ff 15 ? ? ? ? 8b 4d 98 ff 15");

		constexpr auto update_client_bounds = TO_AOB("8b 15 ? ? ? ? 83 c2 04 39 09 e8 ? ? ? ? 59");

		constexpr auto keyboard_get_state = TO_AOB("e8 ? ? ? ? 33 f6 8b 5f 04 3b f3 73 ? f6 44 37 08 80");

		constexpr auto update_frame = TO_AOB("ff 15 ? ? ? ? 0f b6 05 ? ? ? ? a2 ? ? ? ? 80 3d");

		constexpr auto mouse_apply_handler = TO_AOB("d9 c1 d9 1c 24 d9 5d ? d9 5d ? ff 15");
		constexpr auto pause_aob = TO_AOB("02 75 0e 80 3d ? ? ? ? 00 75 05 e9");

		constexpr auto change_mode_aob = TO_AOB("8b 0d ? ? ? ? ba 01 00 00 00 ff 15 ? ? ? ? e9");

		constexpr auto keybinding_aob = TO_AOB("ff 15 ? ? ? ? 89 ? ? 8d ? ? 50 8b 0d ? ? ? ? 8b 55 ? ? ? ff");

		constexpr auto ptexture_fromrawbytes = TO_AOB("89 41 08 6a 01 ba 01 00 00 00 ff 15");

	}

	void init() {

		sprite_cursor_layer = 
			*(sprite_manager ***)((size_t)(mem::find_ERWP_cached(0, on_load::sprite_manager_cursor) + 4));

		{
			auto t = mem::find_ERWP_cached(0, on_load::update_client_bounds);
			osu_data.client_bounds = (net_drawing_rectangle**)(*(size_t*)(t + 2));
			osu_window::resize();
		}

		{
			auto t = mem::find_ERWP_cached(0, on_load::time_hook);
			t += 0x23;
			osu_data.time = (int*)(*(u32*)(t + 1));
		}

		{
			auto t = mem::find_ERWP_cached(0, on_load::change_mode_aob);
			OSU_ChangeMode = **(_osufunc_ChangeMode**)(t + 13);
		}

		{
			auto t = mem::find_ERWP_cached(0, on_load::pause_aob);
			osu_data.is_paused = *(u8**)(t + 5);
			osu_data.play_mode = (int*)(osu_data.is_paused - 0x21);
		}

		{
			auto t = mem::find_ERWP_cached(0, on_load::ptexture_fromrawbytes);
			osu_data.white_texture = *(pTexture***)(t + 0x12);
		}

		{
			auto t = mem::find_ERWP_cached(0, on_load::keybinding_aob);
			osu_data.keybinding_ptr = *(u32*)(t + 0xF);
			osu_data.update_keybinds();
		}

		{
			auto t = mem::find_ERWP_cached(0, on_load::loaded_queue_mode);
			{
				const auto jmp = t - 4;

				OSU_get_ForceNonExclusive = _osufunc_get_ForceNonExclusive(RJMP_REBASE(jmp));

				*(int*)jmp = int(__get_ForceNonExclusive) - int(jmp + 4);
			}
			t += 0x12;
			osu_data.mode = (u32*)*(size_t*)(t + 1);
			osu_data.running_gamemode = (osu_GameMode**)*(size_t*)(t + 7);
		}

		{		
			auto t = mem::find_ERWP_cached(0, on_load::keyboard_get_state) + 1;
		
			OSU_GetKeyboardState = (_osufunc_GetKeyboardState)RJMP_REBASE(t);
		
			*(int*)t = int(__GetKeyboardState) - int(t + 4);		
		}

		{		
			auto t = mem::find_ERWP_cached(0, on_load::mouse_apply_handler) + 0xd;
			OSU_set_MousePosition = **(_osufunc_vec2**)t;
			t += 0xf;
			while (*(u16*)t != 0x15FF)++t;
			OSU_SetCursorHandlerPositions = **(_osufunc_vec2**)(t + 2);
		}

		{		
			auto t = mem::find_ERWP_cached(0, on_load::update_frame);

			while (*(u16*)t != 0x15FF || *(u16*)(t + 6) != 0x15FF)
				++t;
			t += 2;
			OSU_Player_UpdateIsPlaying = **(_osufunc**)t;			
			**(u32**)t = (u32)__update_is_playing;		
		}

		{		
			auto tick = *(u32**)(mem::find_ERWP_cached(0, on_load::handle_cursor_input) + 0xe);
			
			OSU_MouseManager_ApplyHandler = *(_osufunc_mouse_handler*)tick;

			*tick = (u32)mouse_hook;

		}

	}

}
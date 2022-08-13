#pragma once

#include "AQM.h"

void __fastcall __update_is_playing(){	

	virtual_mouse.active = 0;

	AQM::init();

	for (auto f : on_audio_tick)
		if (f) f();

	if (osu_data.force_restart) {

		OSU_ChangeMode(2, 1);
		osu_data.force_restart = 0;

	}
	else if (*osu_data.mode == 2) {

		for (auto f : on_audio_tick_ingame)
			if (f) f();

		if (AQM::menu_is_active)
			AQM::toggle_menu(0, sprite_cursor_layer[0]->get_sprite(*AQM::menu_button));

		if (AQM::menu_button_hidden == 0) {
			AQM::menu_button_hidden = 1;
			sprite_cursor_layer[0]->get_sprite(*AQM::menu_button)->bypass = 1;
		}

	}

	if (virtual_mouse.active)
		osu::MouseManager_SetPosition(virtual_mouse.pos);

	AQM::update_slider(virtual_mouse.raw_pos);

	OSU_Player_UpdateIsPlaying();
}

void __fastcall on_menu_change() {

	const auto queued_mode = *(osu_data.mode - 1);

	osu_window::resize();// could add this to the window on change event.
	
	ZeroMemory(&virtual_keyboard, sizeof(virtual_keyboard));

	osu_data.update_keybinds();

	if (AQM::menu_button_hidden) {
		AQM::menu_button_hidden = 0;
		sprite_cursor_layer[0]->get_sprite(*AQM::menu_button)->bypass = 0;
	}

	for (auto f : on_mode_change)
		if (f) f(queued_mode);
	
}



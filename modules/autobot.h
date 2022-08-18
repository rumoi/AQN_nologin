#pragma once

namespace autobot {

	constexpr static u32 MODULE_ID{1};

	u8 active{};
	u32 last_top_note{};

	void auto_bot(osu_GameMode_Player* gamemode) {

		virtual_mouse.active = 0;
		virtual_keyboard.active = 1;

		const int time = *osu_data.time;

		const auto* hit_manager = gamemode->hitobject_manager;

		const int press_delta = hit_manager->hit_window_300 - 1;

		auto* top_note = hit_manager->get_top_note();

		if (top_note == 0 || (press_delta + time - top_note->time[0]) < 0)
			return;

		vec2 target_pos{ top_note->pos };

		if (top_note->type & Slider) {

			auto* slider_ball = ((osu_Hitobject_SliderOsu*)top_note)->slider_ball;

			if (slider_ball) {
				target_pos.x = slider_ball->position.x;
				target_pos.y = slider_ball->position.y;
			}

		}else if (top_note->type & Spinner) {

			constexpr static float spin_r = 0.001f;
			const auto t = float(time & 63) / 63.f;

			target_pos.x += cos(-3.141592f + 2.f * 3.141592f * t) * spin_r;
			target_pos.y += sin(-3.141592f + 2.f * 3.141592f * t) * spin_r;

		}

		if (last_top_note != u32(top_note)) {

			last_top_note = u32(top_note);

			if (virtual_keyboard.K1) {
				virtual_keyboard.K1 = 0;
				virtual_keyboard.K2 = 1;
			}
			else {
				virtual_keyboard.K1 = 1;
				virtual_keyboard.K2 = 0;
			}

		}

		virtual_mouse.active = 1;

		virtual_mouse.pos = osu_window::field_to_display(target_pos);

	}

	void __fastcall update() {

		if (active == 0 || *osu_data.play_mode != 0)
			return;

		const auto gamemode = (osu_GameMode_Player*)osu_data.running_gamemode[0];
		
		if (gamemode->async_load_complete == 0 || gamemode->game->is_unsafe() || gamemode->hitobject_manager == 0)
			return;

		auto_bot(gamemode);

	}

	void __fastcall menu_init() {

		auto& menu = AQM::module_menu[MODULE_ID];

		menu.sprite_list.reserve(64);

		menu.name = "Autobot"sv;
		menu.icon = FontAwesome::keyboard_o;

		menu.colour = _col{ 16, 74, 168 , 255 };

		{
			menu_object mo{};

			mo.name = "Enabled"sv;
			mo.type = menu_object_type::clicker_bool;
			mo.clicker_bool.value = &active;

			menu.menu_elements.push_back(mo);
		}

	}


	const auto initialized = [] {

		on_audio_tick_ingame[MODULE_ID] = update;
		on_menu_init[MODULE_ID] = menu_init;

		return 1;
	}();


}

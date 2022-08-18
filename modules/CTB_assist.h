#pragma once

namespace CTB_assist {

	constexpr static u32 MODULE_ID{ 4 };

	u8 full_auto{ 0 }, auto_dash{ 0 }, overshot_stop{ 0 };

	void auto_bot(osu_GameMode_Player* gamemode) {

		auto* note = gamemode->hitobject_manager->get_top_note();
		auto* next_note = gamemode->hitobject_manager->get_top_note(u32(note));

		auto* ruleset = (osu_RulesetFruits*)gamemode->ruleset;

		if (note == 0 || ruleset == 0 || ruleset->catcher1 == 0)
			return;

		const float width_target = (ruleset->catcher_width_half + ruleset->catcher_margin) * 0.5f;

		float note_x = note->pos.x;

		if (next_note) {

			const float delta = std::min(q_fabs(next_note->pos.x - note_x), width_target);

			if (next_note->pos.x > note_x) note_x += delta;
			else note_x -= delta;

		}

		const float time_delta = (float)std::max(note->time[0] - *osu_data.time, 1);
		
		const float max_move = ruleset->base_movement_speed * time_delta;

		virtual_keyboard.active = 1;

		const auto target_delta = note_x - ruleset->catcher1->position.x;

		virtual_keyboard.CTB.dash = (max_move <= q_fabs(note_x - ruleset->catcher1->position.x));

		if (q_fabs(target_delta) >= 1.f) {
			if (target_delta > 0.f)
				virtual_keyboard.CTB.right = 1;
			else
				virtual_keyboard.CTB.left = 1;
		}

	}

	u32 sprint_note_id{};

	void _fastcall mod_player_input(osu_GameMode_Player* gamemode) {

		auto* note = (osu_Hitobject_HitCircleFruits*)gamemode->hitobject_manager->get_top_note();

		while(note->is_banana())
			note = (osu_Hitobject_HitCircleFruits*)gamemode->hitobject_manager->get_top_note((u32)note);

		// Sometimes slider ends just don't exist.

		auto* ruleset = (osu_RulesetFruits*)gamemode->ruleset;

		if (note == 0 || ruleset == 0 || ruleset->catcher1 == 0)
			return;

		const float width_target = (ruleset->catcher_width_half + ruleset->catcher_margin) * 0.5f;

		const float delta = note->pos.x - ruleset->catcher1->position.x;
		const float delta_abs = q_fabs(delta);

		virtual_keyboard.active = 1;

		if (auto_dash && sprint_note_id != u32(note)) {

			const float time_delta = (float)std::max(note->time[0] - *osu_data.time, 1);
			const float max_move = ruleset->base_movement_speed * time_delta;

			sprint_note_id = (max_move + width_target * 0.8f <= delta_abs)
				? u32(note) : 0;
		}

		if(sprint_note_id == u32(note))
			virtual_keyboard.CTB.dash = 1;

		if (overshot_stop == 0)
			return;

		if ((note->time[0] - *osu_data.time) <= 750) {

			if (delta_abs > width_target)
				return;

			if (delta > width_target * 0.8f)
				virtual_keyboard.CTB.left = 2;
			if (delta < -width_target * 0.8f)
				virtual_keyboard.CTB.right = 2;
		}

	}

	void __fastcall tick() {

		if (*osu_data.play_mode != 2)
			return;

		const auto gamemode = (osu_GameMode_Player*)osu_data.running_gamemode[0];

		if (gamemode->async_load_complete == 0 || gamemode->game->is_unsafe() || gamemode->hitobject_manager == 0)
			return;

		if(full_auto)
			return auto_bot(gamemode);
		
		if (overshot_stop || auto_dash)
			mod_player_input(gamemode);

	}

	void __fastcall menu_init() {

		auto& menu = AQM::module_menu[MODULE_ID];

		menu.sprite_list.reserve(64);

		menu.name = "CTB Assist"sv;

		menu.icon = FontAwesome::apple;
		menu.icon_offset.y += 0.75f;

		menu.colour = _col{ 13, 186, 65 , 255 };

		{
			menu_object mo{};

			mo.name = "Auto Bot"sv;
			mo.type = menu_object_type::clicker_bool;
			mo.clicker_bool.value = &full_auto;

			menu.menu_elements.push_back(mo);
		}
		{
			menu_object mo{};

			mo.name = "Overshot Stop"sv;
			mo.type = menu_object_type::clicker_bool;
			mo.clicker_bool.value = &overshot_stop;

			menu.menu_elements.push_back(mo);
		}
		{
			menu_object mo{};

			mo.name = "Auto Dash"sv;
			mo.type = menu_object_type::clicker_bool;
			mo.clicker_bool.value = &auto_dash;

			menu.menu_elements.push_back(mo);
		}


	}

	const auto initialized = [] {

		on_audio_tick_ingame[MODULE_ID] = tick;
		on_menu_init[MODULE_ID] = menu_init;

		return 1;
	}();

}

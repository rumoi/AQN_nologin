#pragma once

namespace enlighten {

	constexpr static u32 MODULE_ID{ 3 };

	u8 active{1};

	float AR{ 0.f }, OD{ 0.f }, CS{ 0.f }, HP{0.f};

	bool is_active() {
		return (AR != 0.f || OD != 0.f || CS != 0.f || HP != 0.f);
	}

	using _osufunc_HitObjectManager_UpdateVariables = void(__fastcall*)(osu_Hitobject_Manager*, bool, bool);

	_osufunc_HitObjectManager_UpdateVariables OSU_HitObjectManager_UpdateVariables;

	void __fastcall __HitObjectManager_UpdateVariables(osu_Hitobject_Manager* hm, bool redrawSliders, bool updateActiveTimingPoint){

		if (hm->beatmap) {
			if(AR != 0.f) hm->beatmap->difficulty_approach_rate = AR;
			if(OD != 0.f) hm->beatmap->difficulty_overall = OD;
			if(CS != 0.f) hm->beatmap->difficulty_circle_size = CS;
			if(HP != 0.f) hm->beatmap->difficulty_hp_drain_rate = HP;
		}

		return OSU_HitObjectManager_UpdateVariables(hm, redrawSliders, updateActiveTimingPoint);
	}

	bool loaded{};

	void __fastcall tick() {

		if (loaded || is_active() == 0 || *osu_data.mode != 2)
			return;

		const auto gamemode = (osu_GameMode_Player*)osu_data.running_gamemode[0];

		if (gamemode->async_load_complete == 0 || gamemode->game->is_unsafe() || gamemode->hitobject_manager == 0)
			return;

		size_t func = (size_t)gamemode->hitobject_manager->vtable;

		func = *(size_t*)(func + 0x30);
		func += 0x10;

		loaded = 1;

		OSU_HitObjectManager_UpdateVariables = (_osufunc_HitObjectManager_UpdateVariables)*(size_t*)func;

		*(size_t*)func = (size_t)__HitObjectManager_UpdateVariables;

		osu_data.force_restart |= 1;

	}

	void __fastcall menu_init() {

		auto& menu = AQM::module_menu[MODULE_ID];

		menu.sprite_list.reserve(64);

		menu.name = "Enlighten"sv;

		menu.icon = FontAwesome::eye_slash;
		menu.icon_offset.y += 0.25f;

		menu.colour = _col{ 186, 64, 13 , 255 };

		#define DO(x) {\
			menu_object mo{};\
			mo.name = #x##sv;\
			mo.type = menu_object_type::slider;\
			mo.slider.value = (u32)&x;\
			mo.slider.min_value = 0.f; mo.slider.max_value = 10.f;\
			menu.menu_elements.push_back(mo);\
		}

		DO(AR);
		DO(OD);
		DO(CS);
		DO(HP);

		#undef DO

	}

	const auto initialized = [] {

		on_audio_tick[MODULE_ID] = tick;
		on_menu_init[MODULE_ID] = menu_init;

		return 1;
	}();

}

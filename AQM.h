#pragma once

namespace AQM {

	u8 menu_is_active{}, toggle_held{}, menu_button_hidden{};

	u8 current_module{};


	struct _menu {

		std::string_view name;

		_col colour;

		u16 icon;
		vec2 icon_offset;

		std::vector<menu_object> menu_elements;

		std::vector<pSprite_proxy> sprite_list;

	};

	std::array<_menu, MAX_MODULE_COUNT> module_menu{};

	constexpr static _col MENU_COLOUR{ 18, 82, 209, 255};

	constexpr static float MENU_WIDTH = 475.f;
	constexpr static float MENU_OPTIONS_X_DELTA = 15.f;

	menu_object* current_held_slider{};

	void __fastcall on_clicker_bool_toggle(void*, pSprite* sprite) {

		const auto id{ sprite->AQM.ID };

		for (auto& elem : AQM::module_menu[sprite->AQM._module].menu_elements) {

			if (elem.type != menu_object_type::clicker_bool)
				continue;

			if (*elem.clicker_bool.back_plate != id)
				continue;

			u8* v = elem.clicker_bool.value;

			*v = !*v;

			auto* pip = sprite_cursor_layer[0]->get_sprite(*elem.clicker_bool.pip);

			if (!pip)
				break;

			constexpr static std::string_view new_clicker[2]{ "circle-empty"sv, "circle-full"sv };

			pip->texture = osu::load_texture(new_clicker[bool(*v)], SkinSource::Osu);

			break;
		}

	}

	void __fastcall on_slider_click(void*, pSprite* sprite) {

		const auto id{ sprite->AQM.ID };

		for (auto& elem : AQM::module_menu[sprite->AQM._module].menu_elements) {

			if (elem.type != menu_object_type::slider)
				continue;

			if (*elem.slider.back_plate != id)
				continue;

			current_held_slider = &elem;

			break;
		}

	}

	pSprite_proxy header_text{};
	pSprite_proxy header_icon{};

	pSprite_proxy menu_button{};

	std::vector<pSprite_proxy> AQM_base;

	void __fastcall toggle_menu(void*, pSprite* sprite) {

		auto* sm = sprite_cursor_layer[0];

		const auto set_vis = [v = menu_is_active](pSprite* s) {

			if (s == 0) return;

			s->bypass = v;

		};

		set_vis(sm->get_sprite(*header_text));
		set_vis(sm->get_sprite(*header_icon));

		for (auto& v : AQM_base)
			set_vis(sm->get_sprite(*v));

		for (auto& v : module_menu[current_module].sprite_list)
			set_vis(sm->get_sprite(*v));

		menu_is_active = !menu_is_active;

		if(sprite) sprite->position.x = menu_is_active ? MENU_WIDTH * 0.62f : 0.f;

	}

	void __fastcall show_module_menu(void*, const pSprite* sprite) {
		
		current_module = sprite->AQM._module;

		for (size_t i{}; i < module_menu.size(); ++i) {

			if (i == sprite->AQM._module)
				continue;

			for (auto& sp : module_menu[i].sprite_list) {

				auto* s = sprite_cursor_layer[0]->get_sprite(*sp);

				if (s == 0)
					continue;

				s->bypass = 1;
			}

		}

		auto& menu = module_menu[sprite->AQM._module];

		((pText*)sprite_cursor_layer[0]->get_sprite(*header_text))->set_text(menu.name);
		((pText*)sprite_cursor_layer[0]->get_sprite(*header_icon))->set_font_awesome(menu.icon);

		for (auto& sp : menu.sprite_list) {

			auto* s = sprite_cursor_layer[0]->get_sprite(*sp);

			if (s == 0)
				continue;

			s->bypass = 0;
		}

	}

	void load_menu_base(pTexture* pixel) {

		menu_is_active = 1;

		sprite_cursor_layer[0]->handle_input = 1;
		sprite_cursor_layer[0]->handle_overlay_input = 1;

		AQM_base.reserve(64);

		{
			pSprite* back_plate = osu::create_pSprite(pixel, Fields::CentreRight, Origins::oCentreLeft,
				Clocks::Game, { (MENU_WIDTH * 0.62f), 0.f }, 0.5f, 1, _col(8, 8, 8, 255)
			);

			back_plate->AQM.is_base = 1;
			back_plate->vector_scale = vec2{ MENU_WIDTH + 5.f, 1200.f };
			back_plate->alpha = 0.99f;

			{ // Eats the inputs for stuff below the menu, like the song list
				back_plate->handle_input = 1;
				auto* t = osu::create_transformation(TransformationType::Fade);

				t->start_float = 0.99f;
				t->end_float = 0.99f;
				t->time[0] = 0;
				t->time[1] = 1;

				back_plate->hover_effect = t;
			}

			AQM_base.emplace_back(back_plate->AQM.ID);

			sprite_cursor_layer[0]->add_sprite(back_plate);
		}

		{
			pSprite* name_back_plate = osu::create_pSprite(pixel, Fields::TopRight, Origins::oTopRight,
				Clocks::Game, { 0.f,0.f }, 0.55f, 1, MENU_COLOUR.dim(0.65f));

			name_back_plate->AQM.is_base = 1;
			name_back_plate->vector_scale = vec2{ MENU_WIDTH - 5, 50 };

			AQM_base.emplace_back(name_back_plate->AQM.ID);

			sprite_cursor_layer[0]->add_sprite(name_back_plate);
		}

		{
			pSprite* rim_highlight = osu::create_pSprite(pixel, Fields::CentreRight, Origins::oCentreLeft,
				Clocks::Game, { MENU_WIDTH * 0.62, 0 }, 0.7f, 1, MENU_COLOUR);

			rim_highlight->AQM.is_base = 1;
			rim_highlight->vector_scale = vec2{ 6.5f,1200.f };

			AQM_base.emplace_back(rim_highlight->AQM.ID);

			sprite_cursor_layer[0]->add_sprite(rim_highlight);
		}

		{
			pSprite* h_text = osu::create_pText("Menu", 14, { MENU_WIDTH * 0.62f - 25.f, 8.f }, {}, 0.6f, 1, { 255,255,255,255 }, 1);

			h_text->AQM.is_base = 1;
			h_text->field = Fields::TopRight;
			h_text->origin = Origins::oTopLeft;

			header_text = pSprite_proxy(h_text->AQM.ID);

			sprite_cursor_layer[0]->add_sprite(h_text);
		}

		{
			pSprite* h_icon = osu::create_pText("?", 18, { MENU_WIDTH * 0.62f - 6.f, 16.f }, {}, 0.6f, 1, { 235, 235, 235,255 }, 1);

			h_icon->AQM.is_base = 1;
			h_icon->field = Fields::TopRight;
			h_icon->origin = Origins::oCentreLeft;

			header_icon = pSprite_proxy(h_icon->AQM.ID);

			sprite_cursor_layer[0]->add_sprite(h_icon);
		}
		
		{
			pSprite* m_button = osu::create_pSprite(osu::load_texture("editor-draw-newcombo-expand"sv, SkinSource::Osu),
				Fields::CentreRight, Origins::oCentreRight,
				Clocks::Game, { MENU_WIDTH * 0.62f, -188.f }, 0.981f, 1, MENU_COLOUR);

			m_button->AQM.is_base = 1;
			m_button->handle_input = 1;

			m_button->vector_scale = vec2{ 1.5f,0.85f };

			m_button->on_click = osu::create_event_handler((size_t)toggle_menu);
			m_button->tooltip = osu::create_string("Toggle the menu here!"sv);

			menu_button = pSprite_proxy(m_button->AQM.ID);

			sprite_cursor_layer[0]->add_sprite(m_button);

		}

		{
			const pTexture* bp_texture{osu::load_texture("inputoverlay-key"sv, SkinSource::Osu)};

			for (size_t i = 0; i < MAX_MODULE_COUNT; ++i) {

				const auto& menu = module_menu[i];

				if (menu.name.size() == 0 || menu.icon == 0)
					continue;

				const float x_pos = (float)MENU_WIDTH * 0.62f - (30.f + 59.f * ((i & 3)));
				const float y_pos = 42.f + (16.f * (i > 3));

				{					
					pText* text = osu::create_pText("?"sv, 15,
						{ x_pos + menu.icon_offset.x, y_pos + menu.icon_offset.y },
						{}, 0.6f, 1, menu.colour, 1);

					text->AQM.is_base = 1;
					text->field = Fields::TopRight;
					text->origin = Origins::oCentre;
					text->alpha = 1.f;

					text->set_font_awesome(menu.icon);

					AQM_base.emplace_back(text->AQM.ID);
					sprite_cursor_layer[0]->add_sprite(text);

				}
				
				{
					pSprite* backplate = osu::create_pSprite(bp_texture, Fields::TopRight, Origins::oCentre, Clocks::Game, 
						{x_pos, y_pos}, 0.55f, 1, menu.colour.dim(0.55f)
					);

					backplate->AQM.is_base = 1;
					backplate->AQM._module = i;
					backplate->handle_input = 1;
					backplate->vector_scale = vec2{ 1.25f, 0.5f };
					backplate->alpha = 0.8f;
					backplate->tooltip = osu::create_string(menu.name);
					backplate->on_click = osu::create_event_handler((size_t)show_module_menu);

					AQM_base.emplace_back(backplate->AQM.ID);
					sprite_cursor_layer[0]->add_sprite(backplate);
				}

			}

		}


	}

	bool LOADED{};

	void init() {

		if (LOADED)
			return;

		LOADED = 1;

		for (auto f : on_menu_init)
			if (f) f();

		pTexture* white_pixel = *osu_data.white_texture;

		load_menu_base(white_pixel);

		for (size_t _m{}; _m < module_menu.size(); ++_m){

			auto& m = module_menu[_m];

			float c_y = -150.f;

			for (size_t i{}; i < m.menu_elements.size(); ++i) {

				auto& obj = m.menu_elements[i];

				if (i) {

					auto& p_obj = m.menu_elements[i-1];

					switch (obj.type) {
						case menu_object_type::clicker_bool:
							c_y += p_obj.type != menu_object_type::clicker_bool ? 30.f : 25.f;
							break;
						case menu_object_type::padding:
							c_y += obj.padding.value;
							break;
						default:
							c_y += 25.f;
							break;
					}

				}

				if (obj.type == menu_object_type::clicker_bool) {

					pSprite* back_plate = osu::create_pSprite(white_pixel, Fields::CentreRight, Origins::oCentreRight,
						Clocks::Game, { 0.f, c_y }, 0.55f, 1, MENU_COLOUR.dim(0.7f));

					back_plate->handle_input = 1;
					back_plate->AQM._module = _m;
					back_plate->vector_scale = vec2{ MENU_WIDTH - 4, 25.f };
					back_plate->tooltip = obj.tooltip.size() ? osu::create_string(obj.tooltip) : 0;

					back_plate->on_click = osu::create_event_handler((size_t)on_clicker_bool_toggle);

					back_plate->alpha = 0.01f;
					back_plate->hover_effect = []() {
					
						auto* t = osu::create_transformation(TransformationType::Fade);
					
						if (t) {
							t->start_float = 0.01f;
							t->end_float = 1.f;
							t->time[0] = 0;
							t->time[1] = 200;
						}
					
						return t;
					}();

					pSprite* option_pip = osu::create_pSprite(
						osu::load_texture(*obj.clicker_bool.value ? "circle-full"sv : "circle-empty"sv, SkinSource::Osu),
						Fields::CentreRight, Origins::oCentreRight,
						Clocks::Game, { 255.f + MENU_OPTIONS_X_DELTA, c_y }, 0.6f, 1, MENU_COLOUR);

					option_pip->AQM._module = _m;

					pText* value_text = osu::create_pText(
						obj.name, 12, vec2{ 248 + MENU_OPTIONS_X_DELTA, c_y }, {}, 0.6f, 1, { 255,255,255,255 }, 1
					);

					value_text->AQM._module = _m;
					value_text->field = Fields::CentreRight;
					value_text->origin = Origins::oCentreLeft;

					obj.clicker_bool.back_plate = pSprite_proxy(back_plate->AQM.ID);
					obj.clicker_bool.pip = pSprite_proxy(option_pip->AQM.ID);
					obj.value_text = pSprite_proxy(value_text->AQM.ID);

					m.sprite_list.emplace_back(back_plate->AQM.ID);
					m.sprite_list.emplace_back(option_pip->AQM.ID);
					m.sprite_list.emplace_back(value_text->AQM.ID);

					sprite_cursor_layer[0]->add_sprite(back_plate);
					sprite_cursor_layer[0]->add_sprite(option_pip);
					sprite_cursor_layer[0]->add_sprite(value_text);

				}
				else if (obj.type == menu_object_type::slider) {

					const vec2 back_pos{ 265.f + MENU_OPTIONS_X_DELTA, c_y };

					pSprite* back_plate = osu::create_pSprite(white_pixel, Fields::CentreRight, Origins::oTopLeft,
						Clocks::Game, back_pos, 0.55f, 1, MENU_COLOUR.dim(0.35f));

					back_plate->handle_input = 1;
					back_plate->AQM._module = _m;

					back_plate->alpha = 0.8f;
					back_plate->vector_scale = vec2{ 150.f, 20.f };
					back_plate->tooltip = obj.tooltip.size() ? osu::create_string(obj.tooltip) : 0;

					back_plate->on_click = osu::create_event_handler((size_t)on_slider_click);

					float current_value = obj.slider.is_double ? float(*(double*)obj.slider.value) : *(float*)obj.slider.value;

					const vec2 slider_pos = [&]() {

						const vec2 offset{
							(back_plate->vector_scale.x * ((current_value - obj.slider.min_value)
								/ std::max(1.f, obj.slider.max_value - obj.slider.min_value))) * 0.62f, 0
						};

						return back_pos - ((offset.x > 0) ? offset : vec2{});
					}();

					pSprite* slider = osu::create_pSprite(white_pixel, Fields::CentreRight, Origins::oTopCentre,
						Clocks::Game, slider_pos, 0.6f, 1, MENU_COLOUR);

					slider->AQM._module = _m;
					slider->vector_scale = vec2(13.f, 20.f);

					pText* value_text = osu::create_pText(
						obj.get_slider_string(), 12,
						back_pos - vec2(back_plate->vector_scale.x * 0.62f + 8.f)
						, {}, 0.6f, 1, {255,255,255,255}, 1
					);

					value_text->AQM._module = _m;
					value_text->field = Fields::CentreRight;

					obj.slider.back_plate = pSprite_proxy(back_plate->AQM.ID);
					obj.slider.slider = pSprite_proxy(slider->AQM.ID);
					obj.value_text = pSprite_proxy(value_text->AQM.ID);

					m.sprite_list.emplace_back(back_plate->AQM.ID);
					m.sprite_list.emplace_back(slider->AQM.ID);
					m.sprite_list.emplace_back(value_text->AQM.ID);

					sprite_cursor_layer[0]->add_sprite(back_plate);
					sprite_cursor_layer[0]->add_sprite(slider);
					sprite_cursor_layer[0]->add_sprite(value_text);

				}

			}

		}		
		
		constexpr static pSprite dummy{};

		show_module_menu(0, &dummy);// load the module menu of 0

	}

	void update_slider(const vec2 pos) {

		if (current_held_slider == 0)
			return;

		if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0) {
			current_held_slider = 0;
			return;
		}

		auto* m_obj = current_held_slider;

		auto* spr = sprite_cursor_layer[0]->get_sprite(*m_obj->slider.back_plate);

		if (spr == 0)
			return;

		const float t = [&]{
			const float delta = pos.x - spr->draw_rectangle.x;
			const float width = std::max(spr->draw_rectangle.w, 1.f);
			return std::clamp(delta / width, 0.f, 1.f);
		}();

		if (auto* _s{ sprite_cursor_layer[0]->get_sprite(*m_obj->slider.slider) }; _s)
			_s->position = spr->position - vec2(spr->vector_scale.x * t * 0.62f, 0.f);

		auto new_value = m_obj->slider.min_value + (m_obj->slider.max_value - m_obj->slider.min_value) * t;

		if (m_obj->slider.snap_to_int)
			new_value = std::round(new_value);

		bool value_changed{};

		if (m_obj->slider.is_double) {
			double* v = (double*)m_obj->slider.value;
			value_changed = *v != new_value;
			*v = new_value;
		}else {
			float* v = (float*)m_obj->slider.value;
			value_changed = *v != new_value;
			*v = new_value;
		}

		if (value_changed == 0)
			return;
		
		if (auto* vt{ (pText*)sprite_cursor_layer[0]->get_sprite(*m_obj->value_text) }; vt)
			vt->set_text(m_obj->get_slider_string());

	}

}
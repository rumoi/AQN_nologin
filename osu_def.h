#pragma once

#include "osu_enum.h"

#pragma pack(push)
#pragma pack(1)

template<typename T>
struct net_array {

	u32 vtable;
	u32 size;
	T data[1];

};

template<typename T>
struct net_list {

	u32 vtable;
	net_array<T>* _items;
	void* _sync_root;
	u32 _size;
	u32 _version;

	template<typename T>
	void for_each(T func) {

		if (size_t(this) == 0 || _items == 0)
			return;

		for (size_t i{}, size{std::min(_size, _items->size)}; i < size; ++i)
			func(_items->data[i]);

	}

};

struct net_drawing_rectangle {
	u32 vtable;
	int x, y, width, height;
};

struct net_string {

	u32 vtable;
	u32 size;
	u16 text[1];

};

struct net_event_handler {
	u32 vtable;

	void* _target;
	void* _method_base;
	void* _method_ptr;
	void* _method_ptr_aux;

	net_array<net_event_handler*>* _inv_list;

	int inv_count[4];

};


namespace osu {

	net_string* create_string(const std::string_view s);

}

struct osu_mouse_handler {
	u32 vtable;
	int was_active;
	vec2 position;

	int previous_native_mouse_position[4];

};

struct pTexture {};

struct pTransformation {

	u32 vtable;

	union {
		float start_float;
		void* start_rectangle;
	};

	vec2 start_vector;
	u32 start_colour;
	union {
		float end_float;
		void* end_rectangle;
	};
	vec2 end_vector;
	u32 end_colour;
	int time[2];
	int loop_delay;
	int max_loop_count;
	int current_loop_count;
	int type;
	int easing;
	u8 tag_numeric;
	u8 loop;
	u8 is_loop_static;

};

struct osu_rect {
	float x, y, w, h;

};

struct pSprite {

	u32 vtable;

	net_list<pTransformation*>* effect;
	net_list<pTransformation*>* effect_loop;

	u32 tag;
	void* click_effect;
	net_event_handler* on_click;

	Clocks clock;
	float depth;
	int tag_int;

	float rotation;
	float scale;

	float alpha;

	int blur_radius;
	int blue_type;

	u8 always_draw;
	u8 handle_input;

	u8 click_confirm;
	u8 is_visible;
	u8 hovering;
	u8 bypass;
	u8 is_diposable;

	u8 _pad0;

	_col initial_colour;

	vec2 initial_position;
	vec2 position;
	vec2 vector_scale;

	u8 clip_rect_scaled[0x14];
	u8 clip_rect[0x14];

	pTransformation* hover_effect;
	net_list<void>* hover_effects;
	net_list<void>* un_hover_effects;

	pTexture* texture;
	net_string* tooltip;

	void* on_update;

	net_event_handler* on_hover;
	net_event_handler* on_un_hover;
	net_event_handler* on_disposable;

	int draw_height;
	int draw_width;
	int draw_left;
	int draw_top;
	int height;
	int width;

	Fields field;

	float hover_priority;

	Origins origin;

	union {
		float view_offset_immune_point;
		struct {
			u16 ID;
			u8 _module, is_base;
		} AQM; // Can't use the padding bytes, causes instability
	};

	float draw_scale;

	u8 additive;
	u8 wrap_texture;
	u8 draw_exact;
	u8 dim_immune;
	u8 reverse;
	u8 track_rotation;
	u8 view_offset_immune;
	u8 keep_events_bound;
	u8 scale_to_window_ratio;

	u8 padding_0[3];

	void* draw_colour;
	vec2 draw_position;

	u32 exact_coordinates;

	vec2 last_measure;
	vec2 origin_position;
	osu_rect draw_rectangle;
	vec2 draw_origin;
	vec2 draw_scale_vector;
	vec2 draw_origin_scaled;

	osu_rect draw_rectangle_source;

};

struct pSprite_proxy {

	int AQM_ID;

	int operator*()const{ return AQM_ID; }

};

struct pText : pSprite {

	void* on_unload;
	int state;

	net_event_handler* on_refresh_texture;

	net_string* text;

	u32 border_width;
	u32 rendering_resolution;
	u32 text_alignment;
	u32 shadow;
	float text_size;
	u32 font_face;

	u8 text_aa;
	u8 text_bold;
	u8 text_italic;
	u8 text_render_specific;
	u8 text_changed;
	u8 text_underline;
	u8 text_aggressive_cleanup;

	u8 _pad0;

	vec2 text_bounds;
	float corner_bounds[4];
	vec2 padding;

	void* background_colour;
	void* border_colour;
	void* text_colour;

	void set_font_awesome(u16 f) {

		font_face = FontFace::FontAwesome;

		net_string* text = osu::create_string(" ");

		if (text == 0)
			return;

		text->text[0] = f;

		const u32 tp = (u32)this;

		__asm {
			MOV EDX, text
			MOV ESI, tp
			MOV ECX, ESI
			MOV EAX, [ECX]
			MOV EAX, [EAX + 0x30]
			CALL DWORD PTR[EAX + 0x14]
		}

	}

	void set_text(std::string_view s){

		const u32 text = (u32)osu::create_string(s);
		
		if (text == 0)
			return;
		
		const u32 tp = (u32)this;
		
		__asm {
			MOV EDX, text
			MOV ESI, tp
			MOV ECX, ESI
			MOV EAX, [ECX]
			MOV EAX, [EAX + 0x30]
			CALL DWORD PTR[EAX + 0x14]
		}

	}

};

struct pAnimation : pSprite {

	double frame_delay;
	double animation_start_time;

	void* texture_array;
	void* custom_sequence;
	void* animation_finished;
	int loop_type;
	int texture_count;
	int current_frame;
	int texture_frame;
	u8 draw_dimensions_manual_override;
	u8 run_animation;
	u8 first_frame;
	u8 finished;

};

struct {

	u32 page_size;
	size_t buckets[64];

	u32 bucket_id, bucket_in;

	void reserve(u32 size) {

		if (page_size == 0) {

			SYSTEM_INFO lpSystemInfo;

			GetSystemInfo(&lpSystemInfo);

			page_size = lpSystemInfo.dwPageSize;

			buckets[0] = (size_t)VirtualAlloc(nullptr, page_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			bucket_id = (bucket_in = 0);
		}

		if (bucket_in + size >= page_size) {

			buckets[++bucket_id] = (size_t)VirtualAlloc(nullptr, page_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

			bucket_in = 0;

		}

		bucket_in += size;
	}

	size_t allocate_chunk(const size_t size) {

		reserve(size);

		return (buckets[bucket_id] + bucket_in) - size;
	}

	size_t allocate_system_event_jump(const size_t func) {

		std::array<u8,16> jump{
			0x8b, 0x1c, 0x24, // MOV EBX, [ESP]
			0x83, 0xc4, 0x08, // ADD ESP, 0x8
			0x53, //PUSH EBX
			0xe8, 0,0,0,0,// call
			0xc3,// RET
			0x90,0x90,0x90
		};

		reserve(jump.size());

		const auto mem_start = (buckets[bucket_id] + bucket_in) - jump.size();

		*(int*)(jump.data() + 8) = func - (mem_start + 12);

		*(std::array<u8, 16>*)mem_start = jump;

		return mem_start;
	}

} erw_memory{};

namespace osu { // All functions here require to be called from inside an osu thread.

	std::atomic<int> UNIQUE_ID{ 1 };

	struct _clr_alloc {

		u32 d[2]{};

		_clr_alloc(size_t l) {
			d[0] = *(u32*)(l + 1);
			d[1] = l + 10;
			d[1] += *(int*)(l + 6);
		}
		_clr_alloc(){}
	};

	size_t GLOBAL_STRING_CONSTRUCT{};

	net_string* create_string(const std::string_view s) {
		// Avoid calling this during ruleset transitions.

		if (!GLOBAL_STRING_CONSTRUCT) {

			constexpr auto static string_init{
				TO_AOB("33 c9 e8 ? ? ? ? 8b f8 8d ? ? 33 d2 89 10 c6")
			};

			auto t = mem::find_ERWP_cached(0, string_init);

			if (t == 0)
				return 0;

			t += 0x3;
			GLOBAL_STRING_CONSTRUCT = RJMP_REBASE(t);
		}

		const size_t psize = s.size();

		net_string* p;

		__asm {

			MOV EDX, 0x20 // Character to set, a space in this case.
			XOR ECX, ECX
			PUSH psize

			CALL[GLOBAL_STRING_CONSTRUCT]

			MOV p, EAX
		}

		if (p)
			for (size_t i{}; i < psize; ++i)
				p->text[i] = s[i];

		return p;
	}

	size_t TEXTURE_LOAD_ADD{};

	pTexture* load_texture(std::string_view texture_name, SkinSource source) {

		if (texture_name.size() == 0)
			return 0;

		if (!TEXTURE_LOAD_ADD) {

			constexpr auto static load_texture_aob{
				TO_AOB("c7 45 ? 00 00 b8 41 c7 45 ? 00 00 c8 41 8d 45")
			};

			auto t = mem::find_ERWP_cached(0, load_texture_aob);

			if (t) {

				while (*(DWORD*)t != 0x01BA006A)t--;

				t += 8;

				TEXTURE_LOAD_ADD = RJMP_REBASE(t);

			}

		}

		pTexture* resp{};

		if (TEXTURE_LOAD_ADD == 0)
			return resp;

		const auto str_add = create_string(texture_name);

		if (str_add) {

			__asm {

				MOV EDX, source
				MOV ECX, str_add
				PUSH 0x0//use atlas

				CALL[TEXTURE_LOAD_ADD]

				MOV resp, EAX

			}
		}

		return resp;
	}

	_clr_alloc sprite_alloc{};
	size_t SPRITE_INITIALIZE{};

	pSprite* create_pSprite(const pTexture* texture, const u32 field, const u32 origin, const u32 clock, const vec2 pos, const float depth, const u32 always_draw, const _col colour) {

		if (!texture)
			return 0;

		if (sprite_alloc.d[0] == 0) {

			constexpr auto static sprite_init_aob{
				TO_AOB("b9 ? ? ? ? e8 ? ? ? ? 8b f0 8b 45 ? 89 45 ? 6a 0a 6a 01 6a 00 ff 75 ? ff 75 ? 68 ? ? ? ? 6a 00 8d")
			};

			size_t t = mem::find_ERWP_cached(0, sprite_init_aob);

			if (t == 0)
				return 0;

			sprite_alloc = _clr_alloc{ t };

			t += 0x26;

			while (*(u32*)t != 0xe8d38bce) ++t;

			t += 3;

			SPRITE_INITIALIZE = t + 5;
			SPRITE_INITIALIZE += *(int*)(t + 1);

		}

		const u32 COL = std::bit_cast<u32>(colour);

		pSprite* sprite;

		const u32 clr_ptr = (u32)&sprite_alloc.d[0];

		__asm {

			MOV EAX, clr_ptr
			MOV ECX, [EAX]
			CALL [EAX + 0x4]
			MOV sprite, EAX

			MOV ESI, EAX

			PUSH field
			PUSH origin
			PUSH clock
			PUSH pos.y
			PUSH pos.x
			PUSH depth
			PUSH always_draw
			PUSH COL
			PUSH 0
			MOV EBX, texture
			MOV EDX, EBX
			MOV ECX, sprite
			MOV ESI, ECX

			CALL [SPRITE_INITIALIZE]
		}

		if (sprite)
			sprite->AQM.ID = ++UNIQUE_ID;

		return sprite;
	}

	_clr_alloc clr_SystemEvent;

	net_event_handler* create_event_handler(const u32 func) {

		if (func == 0)
			return 0;

		if (clr_SystemEvent.d[0] == 0) {

			constexpr auto static system_event_aob{
				TO_AOB("39 09 e8 ? ? ? ? b9 ? ? ? ? e8 ? ? ? ? 8b f0 83 7d")
			};

			u32 t = mem::find_ERWP_cached(0, system_event_aob);

			if (t == 0)
				return 0;
			t += 7;

			clr_SystemEvent = _clr_alloc{ t };
		}

		const auto add = erw_memory.allocate_system_event_jump(func);

		if (add == 0)
			return 0;

		net_event_handler* event_handler = (net_event_handler*)&clr_SystemEvent.d[0];

		__asm {
			MOV EAX, event_handler
			MOV ECX, [EAX]
			CALL [EAX + 0x4]
			MOV event_handler, EAX
		}

		if (event_handler)
			event_handler->_method_ptr = (void*)add;

		return event_handler;
	}

	_clr_alloc pText_alloc{};

	size_t PTEXT_INITIALIZE{};

	pText* create_pText(std::string_view text, float font_size, vec2 position, vec2 box, float depth, u32 always_draw, _col col, u32 shadow) {

		if (pText_alloc.d[0] == 0) {
		
			constexpr auto static text_aob{
				TO_AOB("89 45 ? 68 00 00 60 41 8d")
			};
		
			u32 t = mem::find_ERWP_cached(0, text_aob);
		
			if (t == 0)
				return 0;
		
			u32 count{};
		
			while (*(u8*)(t) != 0xB9 || *(u8*)(t + 5) != 0xe8) {
				++count;
				--t;
				if (count > 66)
					return 0;
			}

			pText_alloc = _clr_alloc{ t };

		}

		if (!PTEXT_INITIALIZE) {

			constexpr auto static text_aob{
				TO_AOB("68 1b 2f 7d 3f 6a 00 8d 85")
			};

			u32 t = mem::find_ERWP_cached(0, text_aob);

			if (t == 0)
				return 0;

			u32 count{};

			while (*(u16*)t != 0x558B || *(u8*)(t + 3) != 0xe8) {
				++count;
				++t;
				if (count > 66)
					return 0;
			}

			t += 4;
			PTEXT_INITIALIZE = RJMP_REBASE(t);

		}

		net_string* text_ptr = osu::create_string(text);
		
		if (text_ptr == 0)
			return 0;

		u32 col_value = std::bit_cast<u32>(col);
		
		pText* sprite;

		u32 clr_ptr = (u32)&pText_alloc.d[0];

		__asm {

			MOV EAX, clr_ptr
			MOV ECX, [EAX]
			CALL [EAX + 0x4]
		
			MOV sprite, EAX
			MOV ESI, EAX
		
			PUSH font_size
			PUSH position.y
			PUSH position.x
			PUSH box.y
			PUSH box.x
			PUSH depth
			PUSH always_draw
			PUSH col_value
			PUSH shadow
		
			MOV EDX, text_ptr
		
			MOV ECX, ESI;
		
			CALL [PTEXT_INITIALIZE]
		}

		if (sprite)
			sprite->AQM.ID = ++UNIQUE_ID;

		return sprite;
	}

	_clr_alloc pTransformation_alloc{};

	pTransformation* create_transformation(TransformationType type) {

		if (pTransformation_alloc.d[0] == 0) {

			constexpr auto static aob{
				TO_AOB("75 ? b9 ? ? ? ? e8 ? ? ? ? 8b d0 8b 4e 04 c7 42 ? 08 00 00 00")
			};

			u32 t = mem::find_ERWP_cached(0, aob);

			if (t == 0)
				return 0;

			pTransformation_alloc = _clr_alloc{ t + 2 };

		}

		pTransformation* transformation = (pTransformation*)&pTransformation_alloc.d[0];

		__asm {
			MOV EAX, transformation
			MOV ECX, [EAX]
			CALL[EAX + 0x4]
			MOV transformation, EAX
		}

		if (transformation)
			transformation->type = type;

		return transformation;
	}

}
size_t GLOBAL_ADD_SPRITE{};

struct sprite_manager {

	u32 vtable;

	i64 last_frame_id;

	void* depth_compare;

	net_list<void>* forward_play_list;
	net_list/*stack*/<void>* forward_play_past;
	net_list/*queue*/<void>* forward_play_queue;

	pSprite* hover_sprite;

	net_list<pSprite*>* sprite_list;

	void* sprite_batches;

	float alpha;

	float fade0, fade1;

	int create_time;
	float scale;
	float gamefield_sprite_ratio;
	int exception_count;
	float fade_target;
	int pixels_drawn;
	int blend_mode;

	u8 bypass;
	u8 first_draw;
	u8 enable_profiling;
	u8 custom_view_rectangle;
	u8 forward_play_optimizations[2];
	u8 handle_input;
	u8 masking;
	u8 widescreen_auto_offset;
	u8 calculcate_invisible_updates;
	u8 is_widescreen;
	u8 is_disposed;
	u8 allow_rewind;
	u8 has_begun;
	u8 handle_overlay_input;
	u8 click_handled_at_least_once;

	vec2 view_offset;
	vec2 scroll_offset;
	vec2 base_offset;

	u8 view_rectangle[0x10];
	u8 view_rectangle_scaled[0x10];
	u8 clip_rectangle[0x10];

	bool add_sprite(pSprite* sprite_pointer) {

		if (!GLOBAL_ADD_SPRITE) {

			constexpr auto static sprite_manager_add{
				TO_AOB("8b 0d ? ? ? ? 8b d3 39 09 e8 ? ? ? ? 6a 00 6a 00")
			};

			GLOBAL_ADD_SPRITE = mem::find_ERWP_cached(0, sprite_manager_add);

			if (GLOBAL_ADD_SPRITE == 0)
				return 0;

			GLOBAL_ADD_SPRITE += 0xb;

			GLOBAL_ADD_SPRITE = RJMP_REBASE(GLOBAL_ADD_SPRITE);

		}

		if (!sprite_pointer || !GLOBAL_ADD_SPRITE)
			return 0;

		const auto p = (size_t)this;

		__asm {
			MOV ECX, p
			MOV ESI, sprite_pointer
			MOV EDX, ESI
			CMP [ECX], ECX
			CALL [GLOBAL_ADD_SPRITE]
		}

		return 1;

	}

	pSprite* get_sprite(const u32 id) volatile{

		const auto* items{ sprite_list->_items };

		const u32 size = items ? std::min(sprite_list->_size, id ? items->size : 0) : 0;

		for (size_t i{}; i < size; ++i) {

			pSprite* const v{ items->data[i] };

			if (v == 0 || v->AQM.ID != id)
				continue;

			return v;
		}

		return 0;
	}

	template<typename func>
	void for_each_sprite(func lamb) {

		const auto* items = sprite_list->_items;

		const u32 size = items ? std::min(sprite_list->_size, items->size) : 0;

		for (size_t i{}; i < size; ++i)
			lamb(items->data[i]);

	}
	
};


enum menu_object_type {
	clicker_bool = 0, slider, button,
	padding
};

struct menu_object {

	menu_object_type type;

	std::string_view name;
	std::string_view tooltip;

	int AQM_ID;

	pSprite_proxy value_text;

	union {

		struct {
			float min_value, max_value;
			u32 value;
			u32 is_double:1, snap_to_int:1;
			pSprite_proxy back_plate;
			pSprite_proxy slider;

		} slider;

		struct {
			u8* value;
			pSprite_proxy back_plate;
			pSprite_proxy pip;

		} clicker_bool;

		struct {
			float value;
		} padding;

	};

	std::string get_slider_string() const {

		// lazy gross allocations.

		double v = slider.is_double ? *(double*)slider.value : *(float*)slider.value;

		if (slider.snap_to_int)
			return std::string(name) + ": " + std::to_string(int(v));

		std::string str{ std::to_string(v) };

		if (auto i{ str.find('.')}; i != std::string::npos)
			str.resize(std::min(i + 3, str.size()));

		return std::string(name) + ": " + str;
	}

};

struct osu_HitobjectBase {

	u32 vtable;
	void* __identity;

	double spatial_length;
	int time[2];
	int type;
	int sound_type;
	int segment_count;
	int combo_colour_offset;
	int combo_colour_index;
	int stack_count;
	u8 last_in_combo;
	u8 _pad0[3];
	void* colour;
	vec2 pos;

};

struct osu_Hitobject : osu_HitobjectBase {

	double max_HP;

	void* hit_object_manager;

	net_list<pSprite*>* dim_collection;
	net_list<pSprite*>* sprite_collection;

	net_string* sample_file;
	net_list<pSprite*>* last_kiai_sprites;

	int hit_value;

	int combo_offset;
	int combo_colour_index;

	int sample_addr;
	int sample_volume;
	int sample_set;
	int sample_set_additions;
	int custom_sample_set;

	int tag_numeric;
	int score_value;

	u8 is_hit;
	u8 is_selected;
	u8 kiai;
	u8 dimmed;
	u8 sounded;
	u8 drawable;
	u8 is_scoreable;
	u8 _pad0;

	vec2 base_position;

};

struct osu_Hitobject_HitCircleOsu : osu_Hitobject {

};

struct osu_Hitobject_SliderOsu : osu_Hitobject {
	double curve_length;
	double velocity;

	pAnimation* slider_ball;
	pSprite* slider_ball_spec;
	pSprite* slider_ball_nd;

	net_list<void*/*HitCircleSliderEnd*/>* slider_end_circles;

	pAnimation* slider_follower;

	sprite_manager* sprite_manager_;
	pSprite* slider_body;
	void* render_target;

	void* slider_curve_points;
	void* slider_curve_smooth_lines;
	void* slider_curve_draw_lines_without_remainder;
	void* slider_curve_draw_lines_with_remainder;

	void* slider_smooth_to_draw_line_relationships;
	void* slider_draw_to_smooth_line_relationships;

	void* slider_curve_remainder_line;
	net_list<double*> cumulative_lengths;

	osu_Hitobject_HitCircleOsu* slider_start_circle;
	void* ticks_hit_status;
	void* slider_repeat_points;
	net_list<int> slider_score_timing_points;

	void* sound_type_list;
	void* sample_set_list;
	void* sample_set_addition_list;

	int curve_type;
	int slider_cruve_stored_start;

	int draw_left, draw_top, draw_width, draw_height;

	int curve_type_savable;
	float snaking_progress;
	int successful_place_length;
	int down_button;
	int slider_ticks_hit;
	int init_time;
	int last_tick_sounded;
	int slider_ticks_missed;
	u8 requires_redraw;
	u8 keep_velocity;
	u8 allow_new_placing;
	u8 placing_pending;
	u8 placing_valid;
	u8 successful_place;
	u8 unified_sound_addition;
	u8 body_selected;
	u8 is_sliding;
	u8 _pad2[3];

	vec2 end_position;
	vec2 last_draw_position;
	vec2 placing_point;
	vec2 position_2;

};

struct osu_BeatmapBase {

	u32 vtable;
	void* __identity;

	double difficulty_slider_multiplier;
	double difficulty_slider_tick_rate;
	net_string* artist;
	net_string* artist_unicode;
	net_string* tags;
	net_string* title;
	net_string* title_unucode;

	float difficulty_approach_rate;
	float difficulty_circle_size;
	float difficulty_hp_drain_rate;
	float difficulty_overall;

};

struct osu_Beatmap : osu_BeatmapBase {

	double eyup_stars;
	double bemani_stars;
	double mania_stars;
	u64 last_info_update;

	void* package;

	net_string* audio_expected_md5;
	net_string* audio_filename;
	net_string* background_image;
	net_string* beatmap_checksum;
	net_string* containing_folder;
	net_string* creator;
	net_string* display_title;
	net_string* display_title_full;
	net_string* display_title_no_artist;

	void* tom_stars;

	net_string* filename;
	net_string* online_display_name;

	void* online_personal_scores;
	net_list<void>* score;

	net_string* skin_preference;
	net_string* sort_title;
	net_string* source;
	net_string* version;

	net_list<void>* control_points;
	net_string* extraction_folder;

	void* score_retrieval_request;
	void* on_got_online_scores;
	void* background_visible;
	
	int audio_leadin;
	int beatmap_id;
	int beatmap_set_id;
	int total_records;
	int beatmap_topic_id;
	int beatmap_version;
	int last_edit_time;

	void* countdown;
	int countdown_offset;
	int default_sample_set;
	int drain_length;
	int real_drain_length;
	int object_count;
	int online_offset;
	float online_rating;
	int scores_type;
	int player_offset;

	int player_rank_fruits;
	int player_rank_osu;
	int player_rank_taiko;
	int player_rank_mania;

	int player_mode;

	int preview_time;
	int sample_volume;

	float stack_leniency;

	int submission_status;
	float timeline_zoom;

	int total_length;
	int version_offset;
	int mania_speed;

	int overlay_position;

	u16 count_normal;
	u16 count_slider;
	u16 count_spinner;

	u8 always_showplayfield;
	u8 audio_present;
	u8 is_temp;
	u8 sever_has_osz2;
	u8 beatmap_present;
	u8 custom_colours;
	u8 database_not_found;
	u8 computed_difficulties;
	u8 headers_loaded;
	u8 incorrect_checksum;
	u8 in_osz_container;
	u8 letterbox_in_breaks;
	u8 widescreen_storyboard;
	u8 new_file;
	u8 story_fire_in_front;
	u8 use_skin_sprites_in_SB;
	u8 special_style;
	u8 samples_match_playback_rate;
	u8 updates_available;
	u8 online_favourite;
	u8 checked_for_package;
	u8 epilepsy_warning;
	u8 disabled_samples;
	u8 disabled_skin;
	u8 disable_storyboard;
	u8 disable_video;
	u8 visual_settings_override;
	void* background_colour;
	u64 date_last_played;
	u64 date_modified;

	void* dim_level;

};

struct osu_Hitobject_Manager_Base {
	u32 vtable;
	void* __identity;
	double slider_scoring_point_distance;
	double spinner_rotation_ratio;
	float hit_object_radius;
	int pre_empt;
	int hit_window_50;
	int hit_window_100;
	int hit_window_300;
	float stack_offset;
};

struct osu_Hitobject_Manager : osu_Hitobject_Manager_Base {

	osu_Beatmap* beatmap;

	int active_mods;

	net_list<void>* bookmarks;
	net_list<void>* combo_colours;

	void* event_manager;
	void* hit_factory;

	net_list<osu_Hitobject*>* hit_objects;
	net_list<osu_Hitobject*>* hit_objects_minimal;
	net_list<osu_Hitobject*>* hit_objects_replaced;

	osu_Hitobject* last_hit_object;
	sprite_manager* sprite_manager;

	void* variables;
	void* mania_stage;

	net_list<void>* follow_points;

	void* forced_hit;

	osu_Hitobject* last_added_object;

	int longest_object;
	int longest_object_count;
	float sprite_ratio;
	float sprite_display_size;
	int pre_empt_slider_complete;
	int current_combo_bad;
	int current_combo_katu;
	int current_hit_object_index;
	int hit_objects_count;
	u8 bookmarks_dont_delete;

};


struct osu_Framework_GameHost {

	u32 vtable;

	void* window;
	void* is_active;

	net_event_handler* activated;
	net_event_handler* deactivated;
	net_event_handler* exiting;
	net_event_handler* idle;

};

struct osu_Framework_Game {

	u32 vtable;

	osu_Framework_GameHost* host;

	net_event_handler* activated;
	net_event_handler* deactivated;
	net_event_handler* disposed;
	net_event_handler* exiting;

	u8 exit_requested;
	u8 is_active;

	bool is_unsafe() const{
		return (size_t(this) == 0) || exit_requested || (is_active == 0) || (host == 0);
	}

};

struct osu_GameMode {
	u32 vtable;

	osu_Framework_Game* game;

	net_event_handler* disposed;

	sprite_manager* base_sprite_manager;

};

struct osu_Score {};

struct osu_Ruleset {

	u32 vtable;

	double HP_multiplier_combo_end;
	double HP_multiplier_normal;
	double score_multiplier;
	double combo_addition;
	double mouse_idle_time;

	sprite_manager* sprite_manager_;
	sprite_manager* sprite_manager_widescreen;

	void* combo_counter;
	osu_Score* current_score;
	osu_Hitobject_Manager* hit_object_manager;

	void* hp_bar;
	osu_GameMode* player;

	void* score_display;
	void* score_meter;

	sprite_manager* sprite_manager_add;
	sprite_manager* sprite_manager_flashlight;

	net_list<pSprite*>* combo_burst_sprites;
	pSprite* s_flashlight;
	pSprite* s_lights_out;

	void* fail_sound;
	int recoveries;
	int total_hits_possible;
	int combo_burst_number;
	int hax_check_count;
	u8 is_initialized;
	u8 new_flashlight_overlay;
	u8 _pad0[2];
	vec2 mouse_last_position;

};

struct osu_RulesetOsu : osu_Ruleset {

	pSprite* last_smoke_sprite;
	net_list<pSprite*>* last_smoke_trail_length;
	pTexture* t_smoke;

};

struct osu_GameMode_SongSelect : osu_GameMode {

	pSprite* button_mods;
	pSprite* button_options;
	pSprite* button_random;

	pText* details[6];

	void* dropdown_group;
	void* dropdown_sort;

	osu_Beatmap* initial_beatmap;

	pText* mods_list;
	pText* mods_lis;
	void* ranked_only_checkbox;

	pSprite* rank_forum;
	net_list<void>* rating_stars;

	pSprite* s_mode_logo;
	pSprite* s_mode_sprite;
	pSprite* s_osu[3];

	pSprite* scroll_bar_background;
	pSprite* scroll_bar_foreground;
	pSprite* search_background;

	pText* search_info;

	void* search_text_box;
	void* back_button;

	pText* search_text;

	sprite_manager* sprite_manager_high;
	sprite_manager* sprite_manager_low;
	sprite_manager* sprite_manager_background;
	sprite_manager* sprite_manager_dragged_items;

	void* scroll_list;

	pTexture* star2;
	pSprite* status_icon;
	pSprite* status_icon_approved;
	pSprite* status_icon_ranked;
	pSprite* status_icon_unknown;
	
	void* tabs;

	pSprite* white_overlay;

	void* beatmap_dragged_items;

	pSprite* beatmap_background;
	pSprite* check_button;

	net_string* new_beatmap_name;

	void* open_group;
	net_string* search_string;

	void* difficulty_calculation_thread_pool;

	net_list<pSprite*>* sprites_personal_best;

	pSprite* top_background;
	pSprite* button_mode;

	void* mode_menu;

	net_list<void>* scores;

	void* ranking_type;
	pSprite* loading_spinner;
	void* beatmap_tree_manager;
	void* audio_thread;

	osu_Beatmap* audio_thread_loading_map;

	int active_group_number;
	int next_search_update;
	int tab_displayed_mode;
	int next_beatmap_to_schedule_index;

	float displayed_rating;
	int audio_state;

	u8 handle_input;
	u8 starting;
	u8 multi_mode;
	u8 play_mode;
	u8 returning_from_play_mode;
	u8 audio_error;
	u8 was_dialog_error;
	u8 exiting;
	u8 cleaned_up;
	u8 popup_menu_active;
	u8 search_mode;
	u8 search_text_changed;
	u8 force_next_selection;

};

struct osu_GameMode_Player : osu_GameMode {

	u64 score_checksum_numerical; 
	double new_hax;
	u64 date_time_check_time_initial;

	net_list<pSprite*>* s_arrows;

	osu_Hitobject* active_hitobject;

	u32 visual_settings;
	void* scrubber;
	net_string* current_progress_string;
	void* event_manager;

	osu_Hitobject_Manager* hitobject_manager;

	pSprite* pause_cursor;
	pSprite* pause_cursor_text;

	net_list<pSprite*>* pause_sprites;

	pSprite* pause_continue;
	pSprite* pause_retry;
	pSprite* pause_back;
	void* progress_bar;

	osu_Ruleset* ruleset;

	pSprite* s_break_fail;
	pSprite* s_break_pass;

	void* break_stop_times;
	net_string* score_checksum;
	void* score_entry;
	void* skip;
	pText* spectator_list;

	sprite_manager* sprite_manager_;
	sprite_manager* sprite_manager_add;
	sprite_manager* sprite_manager_below_hitobjects_widescreen;
	sprite_manager* sprite_manager_below_scoreboard_widescreen;
	sprite_manager* sprite_manager_metadata;
	sprite_manager* sprite_manager_interface;
	sprite_manager* sprite_manager_interface_widescreen;
	sprite_manager* sprite_manager_pause_screen;
	sprite_manager* sprite_manager_highest;

	pTexture* t_star2;

	void* input_overlay;

	void*/*bindablebool*/ show_interface;

	void* loader;

	void* frame_analysis;

	net_list<pSprite*>* warnings;

	void* preview_audio_track;
	void* pause_screen_loop;

	void* on_click; // hitobject related
	osu_Hitobject* pending_score_change_object;

	net_list<pSprite*>* pause_arrows;
	pSprite* loading_spinner;

	osu_Beatmap* beatmap;

	float bloom_burst;
	int burst_sample_number;
	int current_update_frame;
	int combo_burst_combo_last;

	int last_pause;
	int miss_previous_frame;
	int progress_string_last_position;
	int resume_after_break_time;
	int score_previous_frame;
	int skip_boundary;
	int countdown_time;
	int spectator_list_update;

	int status;

	int last_hit_time;
	int first_hit_time;
	
	int mode_original;

	float preview_audio_track_volume;

	int last_player_offset_change;
	int last_custom_combo;
	int last_frame_time;

	float unk0[2];

	int pause_selected;
	int audio_start_time;

	int total_playtime;
	int unk1;

	int rank_type;

	int lead_in_time;
	int audio_check_count;
	int audio_check_time;
	int audio_check_time_comp;
	int date_time_check_count;
	int date_time_check_time_comp;
	int fl_skipped_this_note;
	int hax_check_count;
	int unk2;
	int last_lpc_check;
	// Don't use anything below here except async_load_complete
	u8 is_tutorial;
	u8 break_accounted_for;
	u8 combo_burst_pending;
	u8 failed_previous_frame;
	u8 force_replay_frame;
	u8 has_skipped_outro;
	u8 online_data_pending;
	u8 passed;
	u8 reported_endgame;
	u8 unpause_confirmed;
	u8 drain_time;
	u8 initial_allow_submission;
	u8 input_recived_at_least_once;
	u8 replay_mode_stable;
	u8 load_delay_added;
	u8 new_sync_beat_waiting;
	u8 failed_state;
	u8 unk3[5];
	u8 finished_initial_audio_setup;
	u8 async_load_complete;//0x183
	u8 is_autoplay_replay;
	u8 reloading_after_change;
	u8 fl_checked_this_play;
	u8 allow_submission_hax_check;
	u8 hax_found;
	int unk4[2];
	float mouse_velocity[2];
	float mouse_velocity_last_position[2];
	float pause_location[2];
	void* star_break_additive;
};

#pragma pack(pop)
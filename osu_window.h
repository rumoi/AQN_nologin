#pragma once

namespace osu_window {

	constexpr static int PLAYFIELD_WIDTH = 512;
	constexpr static int PLAYFIELD_HEIGHT = 384;
	constexpr static int FIELD_WIDTH = 640;
	constexpr static int FIELD_HEIGHT = 480;

	vec2 playfield_offset;

	float game_ratio;

	vec2 offset_vector;

	void resize() {

		const auto current_bounds = *osu_data.client_bounds[0];

		const float screen_ratio = float(current_bounds.height) / float(FIELD_HEIGHT);

		const float game_width = PLAYFIELD_WIDTH * screen_ratio;
		const float game_height = PLAYFIELD_HEIGHT * screen_ratio;

		game_ratio = game_height / PLAYFIELD_HEIGHT;

		float mode_offset = -16 * screen_ratio;// Inside the editor it should be different, w/e.

		playfield_offset = (
			offset_vector = vec2((float)(current_bounds.width - game_width) / 2, (float)(current_bounds.height - game_height) / 4 * 3 + mode_offset)
		);

		offset_vector.x += current_bounds.x;
		offset_vector.y += current_bounds.y;

	}

	const vec2 field_to_display(vec2 x) {
		return playfield_offset + x * game_ratio;
	}

	const vec2 field_to_display(float x, float y) {
		return field_to_display({x,y});
	}

};
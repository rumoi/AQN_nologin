#pragma once

template<size_t byte_count>
struct AOB_PATTERN {

	static_assert(byte_count <= 64);

	constexpr static size_t size{ byte_count };

	u64 wildcard;
	std::array<u8, size> op_code;

};

__forceinline constexpr u8 hex_to_dec(u8 hex){

	hex = (hex >= 'A' && hex <= 'F') ? hex + u8('a' - 'A') : hex;

	return (hex >= 'a' && hex <= 'f') ? (10 + (hex - u8('a'))) : (hex - u8('0'));
}

constexpr u8 u8_hex_parse(std::string_view x) {

	if (x.size() == 0)
		return 0;

	if (x.size() == 2)
		return (hex_to_dec(x[0]) << 4) + hex_to_dec(x[1]);

	return hex_to_dec(x[0]);
}

consteval f_u8 GET_AOB_SIZE(const char* s) {
	return 1 + std::ranges::count_if(std::string_view(s), [](const char v) {return v == ' '; });;
}

template<size_t byte_count>
consteval auto PARSE_AOB(const char* s) {

	AOB_PATTERN<byte_count> ret{};

	const char* start{ s };

	size_t index{};

	const auto parse = [&](const char* end) {

		ON_SCOPE_EXIT(
			++index;
			start = end + 1;
		);

		if (*start == '?')
			ret.wildcard |= (u64(1) << index);
		else
			ret.op_code[index] = u8_hex_parse(std::string_view(start, size_t(end - start)));

	};

	for (char c{}; c = *s; ++s)
		if (c == ' ') parse(s);

	parse(s);

	return ret;
}

#define TO_AOB(x) PARSE_AOB<GET_AOB_SIZE(x)>(x)

namespace mem {

	// Pretty useless caching for an internal project.

	struct _page {
		size_t start;
		std::vector<u8> data;
	};
	
	std::vector<_page> ERWP_cache; // ~12mb of ERWP total

	void load_cache() {

		ERWP_cache.reserve(64);

		constexpr u32 page_type = MEM_PRIVATE, page_protect = PAGE_EXECUTE_READWRITE;

		MEMORY_BASIC_INFORMATION info;

		for (size_t i{}; VirtualQuery((void*)i, &info, sizeof(info)); i += info.RegionSize) {

			if (!i || info.State != MEM_COMMIT || info.Protect != page_protect || info.Type != page_type)
				continue;

			auto& c{ ERWP_cache.emplace_back() };

			c.start = i;
			c.data.resize(info.RegionSize);

			// ~37 pages to copy
			memcpy((void*)c.data.data(), (void*)i, c.data.size());

		}

	}

	template<size_t byte_count>
	size_t find_ERWP_cached(const size_t start, AOB_PATTERN<byte_count> AOB) {

		if (ERWP_cache.size() == 0)
			load_cache();

		for (const auto& page : ERWP_cache) {

			// Expects the AOB to not cross the page boundary.

			if (page.start < start)
				continue;

			size_t match_count{}, i{}, size{ page.data.size() };

			for (; i < size; ++i) {
				
				if (((AOB.wildcard >> match_count) & 1) || AOB.op_code[match_count] == page.data[i]) {

					if (++match_count == AOB.size) {

						const auto offset = (i - match_count) + 1;

						return offset + page.start;
					}

				}
				else {
					i -= match_count;
					match_count = 0;
				}

			}


		}

		ERWP_cache.clear();

		return 0;
	}

}
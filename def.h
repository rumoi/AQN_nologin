#pragma once

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint_fast8_t f_u8;
typedef uint_fast16_t f_u16;
typedef uint_fast32_t f_u32;
typedef uint_fast64_t f_u64;

static_assert(sizeof(u8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(u64) == 8);

template <typename F> struct on_scope_exit {
private: F func;
public:
	on_scope_exit(const on_scope_exit&) = delete;
	on_scope_exit& operator=(const on_scope_exit&) = delete;

	constexpr on_scope_exit(F&& f) : func(std::forward<F>(f)) {}
	constexpr ~on_scope_exit() { func(); }
};

#define PCAT0(x, y) PCAT1(x, y)
#define PCAT1(x, y) PCAT2(!, x ## y)
#define PCAT2(x, r) r
#define ON_SCOPE_EXIT(...) on_scope_exit PCAT0(__scope, __LINE__) {[&]{__VA_ARGS__}}

#define RJMP_REBASE(x) (int(x + 4) + *(int*)x)

[[nodiscard]] consteval std::string_view operator"" sv(const char* _Str, size_t _Len) noexcept {
	return std::string_view(_Str, _Len);
}

struct vec2 {

	float x, y;

	vec2 operator-(const vec2 v) const{
		return { x - v.x, y - v.y };
	}
	vec2 operator+(const vec2 v) const {
		return { x + v.x, y + v.y };
	}

};
struct vec3 {
	union {
		struct { float x, y, z; };
		struct { float r, g, b; };
	};
};
struct _col {

	u8 r, g, b, a;

	constexpr _col dim(float f) const {

		// Not gamma correct, was like this in the original

		return {
			u8(float(r) * f),
			u8(float(g) * f),
			u8(float(b) * f),
			a };

	}

};


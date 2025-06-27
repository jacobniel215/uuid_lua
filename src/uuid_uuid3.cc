#ifdef CXX_MODULES_SUPPORT

#ifndef IMPORT_STD
module;

#include <array>
#include <cstddef>
#include <string_view>
#endif

export module uuid:uuid3;
import :object;
import md5;

#ifdef IMPORT_STD
import std;
#endif

export class UUIDv3 {
public:
	static auto GetUUID(std::string_view name, const UUID ns) -> UUID;
};

#else

#include <array>
#include <cstddef>
#include <string_view>
#include "uuid3.hh"
#include "md5.hh"
#endif

const std::size_t BYTE = 8;

namespace {
constexpr auto bit_to_byte(std::size_t bit) -> std::size_t {
	return bit / BYTE;
}
}

auto UUIDv3::GetUUID(std::string_view name, const UUID ns) -> UUID {
	Md5 md5;

	md5.add(ns.data());
	md5.add(name);

	std::array<unsigned char, 16> md5_hash = md5.hash();
	std::array<unsigned char, bit_to_byte(128)> uuid {};

	// md5_high
	for (int i = 0; i < bit_to_byte(48); i++) {
		uuid[i] = md5_hash[i];
	}

	// ver and md5_mid
	uuid[6] = md5_hash[6];

	// clear out higher nibble (4 bits) (blame big endian)
	uuid[6] &= 0x0F;
	// set ver
	uuid[6] |= ((3 << 4) & 0xF0);
	uuid[7] = md5_hash[7];

	// set var and md5_low
	for (int i = bit_to_byte(64); i < bit_to_byte(128); i++) {
		uuid[i] = md5_hash[i];
		// set var
		if (i == bit_to_byte(64)) {
			// set the first bit to 1
			uuid[i] |= (1 << 7);

			// set the second bit to 2
			uuid[i] &= ~(1 << 6);
		}
	}

	return UUID { uuid };
}

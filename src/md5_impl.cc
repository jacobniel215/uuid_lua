#ifdef CXX_MODULES_SUPPORT

#ifndef IMPORT_STD
module;

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <vector>
#endif

module md5;

#ifdef IMPORT_STD
import std;
#endif

#else
#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <vector>
#include "md5.hh"
#endif

namespace {
// md5 auxiliary functions
constexpr auto f(std::uint32_t x, std::uint32_t y, std::uint32_t z) -> std::uint32_t {
	// if x, then y, else z
	return (x & y) | (~x & z);
}

constexpr auto g(std::uint32_t x, std::uint32_t y, std::uint32_t z) -> std::uint32_t {
	return (x & z) | (y & ~z);
}

constexpr auto h(std::uint32_t x, std::uint32_t y, std::uint32_t z) -> std::uint32_t {
	// x xor y xor z
	return x ^ y ^ z;
}

constexpr auto i(std::uint32_t x, std::uint32_t y, std::uint32_t z) -> std::uint32_t {
	return y ^ (x | ~z);
}
}

void Md5::process_block(std::array<std::uint32_t, 16> block) {
	std::array<std::uint32_t, 16> m = block;
	std::uint32_t aa = wordBuffer[0];
	std::uint32_t bb = wordBuffer[1];
	std::uint32_t cc = wordBuffer[2];
	std::uint32_t dd = wordBuffer[3];

	std::uint32_t F = 0;
	unsigned int G = 0;

	for (std::size_t I = 0; I < 64; I++) {
		switch (I / 16) {
		case 0:
			F = f(bb, cc, dd);
			G = I;
			break;
		case 1:
			F = g(bb, cc, dd);
			G = ((I * 5) + 1) % 16;
			break;
		case 2:
			F = h(bb, cc, dd);
			G = ((I * 3) + 5) % 16;
			break;
		default:
			F = i(bb, cc, dd);
			G = (I * 7) % 16;
			break;
		}

		const std::uint32_t dd_copy = dd;
		dd = cc;
		cc = bb;
		// c++ 20 leftrotate function
		bb = bb + std::rotl(aa + F + constant_table[I] + m[G], shift_amounts[I]);
		aa = dd_copy;
	}
	wordBuffer[0] += aa;
	wordBuffer[1] += bb;
	wordBuffer[2] += cc;
	wordBuffer[3] += dd;
}

void Md5::reset() {
	// Initialize MD5 word buffers
	wordBuffer[0] = static_cast<std::uint32_t>(0x67452301);
	wordBuffer[1] = static_cast<std::uint32_t>(0xefcdab89);
	wordBuffer[2] = static_cast<std::uint32_t>(0x98badcfe);
	wordBuffer[3] = static_cast<std::uint32_t>(0x10325476);
}

Md5::Md5() {
	reset();
}

auto Md5::hash() -> std::array<unsigned char, 16> {
	std::vector<std::uint32_t> hashBuffer;
	std::uint32_t word = 0;
	std::size_t byte = 0;
	for (byte = 0; byte < buffer.size(); byte++) {
		switch (byte % 4) {
		case 0:
			word = static_cast<std::uint32_t>(buffer[byte]);
			continue;
		case 1:
			word |= static_cast<std::uint32_t>(buffer[byte]) << 8;
			continue;
		case 2:
			word |= static_cast<std::uint32_t>(buffer[byte]) << 16;
			continue;
		case 3:
			word |= static_cast<std::uint32_t>(buffer[byte]) << 24;
			hashBuffer.push_back(word);
			word = 0;
			continue;
		default:
			break;
		}
	}

	if (word != 0) {
		hashBuffer.push_back(word);
	}
	const std::size_t prePaddingSize = buffer.size();

	// set the first padded bit to 1 (the byte being 1 0 0 0 0 0 0 0)
	if ((byte % 4) == 0) {
		word = 0;
	} else {
		// add a byte so the switch puts the first padded byte in the right place
		word = hashBuffer[hashBuffer.size() - 1];
	}

	switch (byte % 4) {
	case 0:
		word = static_cast<std::uint32_t>(128);
		break;
	case 1:
		word |= static_cast<std::uint32_t>(128) << 8;
		break;
	case 2:
		word |= static_cast<std::uint32_t>(128) << 16;
		break;
	case 3:
		word |= static_cast<std::uint32_t>(128) << 24;
		break;
	default:
		break;
	}

	if ((byte % 4) == 0) {
		hashBuffer.push_back(word);
	} else {
		hashBuffer[hashBuffer.size() - 1] = word;
	}

	// message has to be padded to 448 according to the modulo of 512.
	while (((hashBuffer.size() * 32) % 512) != 448) {
		// just push zero bytes
		hashBuffer.push_back(0);
	}

	hashBuffer.push_back(static_cast<std::uint32_t>(prePaddingSize * 8));
	hashBuffer.push_back(static_cast<std::uint32_t>((prePaddingSize * 8) >> 32));

	for (std::size_t i = 0; i < hashBuffer.size();) {
		std::array<std::uint32_t, 16> block {};
		const std::size_t offset = (i + 15) > (hashBuffer.size() - 1) ? hashBuffer.size() - i : 16;

		std::copy(std::next(hashBuffer.begin(), i), std::next(hashBuffer.begin(), i + offset),
			std::begin(block));
		process_block(block);

		i += offset;
	}

	std::array<unsigned char, 16> digest = {};
	for (std::size_t i = 0; i < 4; i++) {
		digest[(i * 4) + 0] = static_cast<std::uint8_t>((wordBuffer[i] & 0x000000FF));
		digest[(i * 4) + 1] = static_cast<std::uint8_t>((wordBuffer[i] & 0x0000FF00) >> 8);
		digest[(i * 4) + 2] = static_cast<std::uint8_t>((wordBuffer[i] & 0x00FF0000) >> 16);
		digest[(i * 4) + 3] = static_cast<std::uint8_t>((wordBuffer[i] & 0xFF000000) >> 24);
	}

	reset();

	return digest;
}

void Md5::clear() {
	buffer.clear();
	reset();
}

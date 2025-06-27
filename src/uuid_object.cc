#ifdef CXX_MODULES_SUPPORT
module;
#ifndef HAVE_RANGES_VIEWS_CHUNK
#include <range/v3/view/chunk.hpp>
#define CHUNK ranges::views::chunk
#else
#define CHUNK std::views::chunk
#endif

#ifndef IMPORT_STD
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <initializer_list>
#include <iomanip>
#include <iterator>
#include <ranges>
#include <ios>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#endif
export module uuid:object;

#ifdef IMPORT_STD
import std;
#endif

#else

#ifndef HAVE_RANGES_VIEWS_CHUNK
#include <range/v3/view/chunk.hpp>
#define CHUNK ranges::views::chunk
#else
#define CHUNK std::views::chunk
#endif
#include "uuid.hh"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <initializer_list>
#include <iomanip>
#include <iterator>
#include <ranges>
#include <ios>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#endif

using UUIDArray = std::array<std::uint8_t, 16>;

#ifdef CXX_MODULES_SUPPORT
export class UUID {
public:
	UUID(const std::string_view uuid);
	UUID(const UUIDArray uuid);
	[[nodiscard]] auto to_string() const -> std::string;
	[[nodiscard]] auto data() const -> std::span<const std::uint8_t, 16>;

private:
	UUIDArray uuid {};
};
#endif

using byte_vector = std::vector<std::uint8_t>;

namespace {
auto get_uuid_string_length(const std::size_t size) -> std::size_t {
	if (size == 0)
		return 0;
	return size / 2;
}

auto convert_to_hex_byte(const char one, const char two) -> std::uint8_t {
	const std::span<const char, 2> hexByte { std::initializer_list { one, two } };
	return static_cast<std::uint8_t>(std::strtol(hexByte.data(), nullptr, 16));
}

auto convert_hex_string_to_bytes(std::string_view uuid) -> byte_vector {
	byte_vector bytes;
	auto iter
		= std::forward<std::string_view>(uuid) | CHUNK(2) | std::views::transform([](auto&& chunk) {
			  const char charone = chunk[0];
			  if (chunk.size() > 1) {
				  return convert_to_hex_byte(charone, chunk[1]);
			  } else {
				  return convert_to_hex_byte(charone, '\0');
			  }
		  });

	for (const std::uint8_t i : iter) {
		bytes.push_back(i);
	}

	return bytes;
}
}

UUID::UUID(std::string_view uuid) {
	std::string uuidString { uuid };
	std::erase(uuidString, '-');
	std::erase(uuidString, '{');
	std::erase(uuidString, '}');
	if (get_uuid_string_length(uuidString.length()) != 16) {
		throw std::invalid_argument(
			std::format("The UUID string is {} bytes long instead of 16 bytes long",
				get_uuid_string_length(uuidString.length())));
	}

	byte_vector bytes = convert_hex_string_to_bytes(uuidString);
	if (bytes.size() != 16) {
		throw std::invalid_argument("The UUID internal bytes vector is the wrong size");
	}
	std::ranges::copy(bytes, std::begin(this->uuid));
}

UUID::UUID(UUIDArray uuid) {
	std::ranges::copy(uuid, std::begin(this->uuid));
}

auto UUID::to_string() const -> std::string {
	std::stringstream ss;
	std::size_t index = 0;
	ss << std::hex << std::setfill('0');
	std::ranges::for_each(this->uuid, [&](auto i) {
		ss << std::setw(2) << static_cast<std::uint32_t>(i);
		// add dashes to uuid string
		if (index == 3 || index == 5 || index == 7 || index == 9) {
			ss << '-';
		}
		index++;
	});

	return ss.str();
}

auto UUID::data() const -> std::span<const std::uint8_t, 16> {
	return std::span { this->uuid };
}

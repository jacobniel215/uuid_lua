#ifndef UUID_HH
#define UUID_HH
#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>

using UUIDArray = std::array<uint8_t, 16>;

class UUID {
public:
	UUID(const std::string_view uuid);
	UUID(const UUIDArray uuid);
	[[nodiscard]] auto to_string() const -> std::string;
	[[nodiscard]] auto data() const -> std::span<const uint8_t, 16>;

private:
	UUIDArray uuid {};
};
#endif

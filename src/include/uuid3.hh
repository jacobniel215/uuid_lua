#ifndef UUID3_HH
#define UUID3_HH
#include "uuid.hh"
#include <string_view>
class UUIDv3 {
public:
	static auto GetUUID(std::string_view name, const UUID ns) -> UUID;
};
#endif

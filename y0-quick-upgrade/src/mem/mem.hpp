#pragma once

namespace mem
{
    uintptr_t        pattern_scan(std::string_view mod, std::string_view pattern);
    std::vector<int> pattern_to_bytes(std::string_view pattern);
    uintptr_t        follow_rel32(uintptr_t address, uintptr_t offset = 0x1);
}
#include "../shared.hpp"

#include <cstdlib>
#include <winternl.h>

std::vector<int> mem::pattern_to_bytes(std::string_view pattern)
{
    char* start          = const_cast<char*>(pattern.data());
    char* end            = start + pattern.length();
    std::vector<int> ret = {};

    for (char* cur = start; cur < end; ++cur)
    {
        if (*cur == '?')
        {
            ++cur;
            ret.push_back(-1);
        }
        else
        {
            ret.push_back(std::strtoul(cur, &cur, 16));
        }
    }

    return ret;
}

uintptr_t mem::follow_rel32(uintptr_t address, uintptr_t offset)
{
    return address + *reinterpret_cast<int32_t*>(address + offset) + 0x5;
}

uintptr_t mem::pattern_scan(std::string_view mod, std::string_view pattern)
{
    HMODULE mod_handle = GetModuleHandleA(!mod.empty() ? mod.data() : nullptr);
    if (!mod_handle)
        return {};

    IMAGE_DOS_HEADER* dos_header = reinterpret_cast<IMAGE_DOS_HEADER*>(mod_handle);
    IMAGE_NT_HEADERS* nt_header  = reinterpret_cast<IMAGE_NT_HEADERS*>(uintptr_t(dos_header) + dos_header->e_lfanew);
    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE || nt_header->Signature != IMAGE_NT_SIGNATURE)
        return {};

    byte* start                     = reinterpret_cast<byte*>(mod_handle);
    byte* end                       = start + nt_header->OptionalHeader.SizeOfImage;
    std::vector<int> pattern_bytes  = pattern_to_bytes(pattern);

    for (byte* cur = start; cur < end; ++cur)
    {
        bool found = true;

        for (size_t i = 0; i < pattern_bytes.size(); ++i)
        {
            if (cur[i] != static_cast<byte>(pattern_bytes[i]) &&
                pattern_bytes[i] != -1)
            {
                found = false;
                break;
            }
        }

        if (found)
            return reinterpret_cast<uintptr_t>(cur);
    }

    return {};
}

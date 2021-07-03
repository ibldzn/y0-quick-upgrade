#include "../shared.hpp"

#include "../../deps/minhook/include/MinHook.h"

static hooks::on_ability_unlock::fn o_on_ability_unlock = nullptr;

template <typename T>
bool make_hook(uintptr_t address, void* detour, T** original)
{
    if (MH_CreateHook(reinterpret_cast<LPVOID>(address), detour, reinterpret_cast<LPVOID*>(original)) != MH_OK)
        return false;

    if (MH_EnableHook(reinterpret_cast<LPVOID>(address)) != MH_OK)
        return false;

    return true;
}

#define MAKE_HOOK_SIG(mod, sig, relative, detour, original) \
[]() \
{ \
    uintptr_t hook_at = mem::pattern_scan(mod, sig); \
    if (relative) \
        hook_at = mem::follow_rel32(hook_at); \
    if (!make_hook(hook_at, detour, original)) \
        throw std::runtime_error("hooks::init - " ## #detour ## " failed!"); \
}()

void hooks::init()
{
    if (MH_Initialize() != MH_OK)
        throw std::runtime_error("hooks::init - Failed to initialize minhook!");

    MAKE_HOOK_SIG("", "40 53 48 83 EC 30 33 D2 48 89 6C 24 ?", false, hooks::on_ability_unlock::hook_func, &o_on_ability_unlock);
}

void hooks::destroy()
{
    MH_Uninitialize();
}

int64_t __fastcall hooks::on_ability_unlock::hook_func(int64_t a1)
{
    static auto sub_140445630 = reinterpret_cast<int64_t(__fastcall*)(int64_t a1, uint32_t a2)>(
        mem::follow_rel32(mem::pattern_scan("", "E8 ? ? ? ? 8B 6D 50"))
        );

    /// lmao. https://i.imgur.com/JL0S4Am.png
    uint32_t v2 = *reinterpret_cast<uint32_t*>(
        *reinterpret_cast<int64_t*>(a1 + 0xC0) +
        int64_t(0x30) *
        *reinterpret_cast<uint32_t*>(sub_140445630(a1, 0) + 0x20) +
        int64_t(0x24)
    );

    if (v2 != 3) /// We don't wanna do anything if we're not upgrading legend style
        return o_on_ability_unlock(a1);
    
    int32_t node = *reinterpret_cast<int32_t*>(a1 + 0x1B4);

    int64_t to_ret = 0;

    for (int32_t i = 0; i < 50; ++i) /// Loop through all nodes
    {
        *reinterpret_cast<int32_t*>(a1 + 0x1B4) = i;
        int64_t res = o_on_ability_unlock(a1);

        if (i == node) /// If it's the node we actually choose in the upgrade menu
            to_ret = res;
    }

    return to_ret;
}

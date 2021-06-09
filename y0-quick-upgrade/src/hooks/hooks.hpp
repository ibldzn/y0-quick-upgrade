#pragma once

namespace hooks
{
    void init();
    void destroy();

    namespace on_ability_unlock
    {
        using fn = int64_t(__fastcall*)(int64_t a1);
        int64_t __fastcall hook_func(int64_t a1);
    }
}
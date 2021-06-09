#pragma once

using byte = unsigned char;

#define WIN32_LEAN_AND_MEAN
#  include <Windows.h>
#undef WIN32_LEAN_AND_MEAN

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <string_view>

#include "hooks/hooks.hpp"
#include "mem/mem.hpp"
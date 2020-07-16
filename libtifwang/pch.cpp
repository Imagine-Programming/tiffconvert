// pch.cpp: source file corresponding to the pre-compiled header

#include "pch.h"

// When you are using pre-compiled headers, this source file is necessary for compilation to succeed.
_Check_return_ float _cdecl _byteswap_float(_In_ float _Number) {
    union { float f; int32_t i; } u;
    u.f = _Number;
    u.i = _byteswap_ulong(u.i);
    return u.f;
}

_Check_return_ double _cdecl _byteswap_double(_In_ double _Number) {
    union { double f; int64_t i; } u;
    u.f = _Number;
    u.i = _byteswap_uint64(u.i);
    return u.f;
}
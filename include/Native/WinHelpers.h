#ifdef _WIN32
#ifndef HEPH_WIN_HELPERS_H
#define HEPH_WIN_HELPERS_H

/** @file */

#include <string>
#include <combaseapi.h>
#include <comdef.h>

namespace Heph::Native
{
    /**
     * Gets error message from error code.
     *
     * @param hres Error code returned by the native method.
     * @return Error message.
     */
    constexpr inline std::string ComErrorStr(HRESULT hres)
    {
        if constexpr (sizeof(TCHAR) == sizeof(char))
        {
            return std::string((const char*)_com_error(hres).ErrorMessage());
        }
        else
        {
            const std::wstring ws((const wchar_t*)_com_error(hres).ErrorMessage());
            return std::string(ws.begin(), ws.end());
        }
    }
}

#endif
#endif
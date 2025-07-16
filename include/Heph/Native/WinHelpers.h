#ifdef _WIN32
#ifndef HEPH_WIN_HELPERS_H
#define HEPH_WIN_HELPERS_H

#include "Heph/Utils.h"
#include <string>
#include <combaseapi.h>
#include <comdef.h>

/** @file */

namespace Heph::Native
{
    /** @brief Implements helper methods for Windows. */
    class HEPH_API WinHelpers final
    {
    public:
        HEPH_DISABLE_INSTANCE(WinHelpers);

        /**
         * Gets error message from error code.
         *
         * @param hres Error code returned by the native method.
         * @return Error message.
         */
        static constexpr inline std::string ComErrorStr(HRESULT hres)
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
    };
}

#endif
#endif
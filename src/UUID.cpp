#include "UUID.h"
#include "HephEndian.h"
#include "Exceptions/ExternalException.h"
#include "Exceptions/InvalidArgumentException.h"
#include <format>
#include <sstream>
#include <iomanip>

#if defined(_WIN32)
#include "WinHelpers.h"
#include <combaseapi.h>
#endif

namespace Heph
{
    UUID::UUID()
    {
        this->data.fill(0);
    }

    UUID::UUID(const NativeUUID& nativeUUID)
    {
        (*this) = nativeUUID;
    }

    UUID::UUID(const std::string& uuidStr)
    {
        (*this) = uuidStr;
    }

    UUID::UUID(const std::wstring& uuidStr)
    {
        (*this) = uuidStr;
    }

    UUID& UUID::operator=(const NativeUUID& nativeUUID)
    {
#ifdef _WIN32

        NativeUUID temp;
        temp.Data1 = Heph::NativeToBigEndian(nativeUUID.Data1);
        temp.Data2 = Heph::NativeToBigEndian(nativeUUID.Data2);
        temp.Data3 = Heph::NativeToBigEndian(nativeUUID.Data3);

        (void)std::copy((uint8_t*)&temp.Data1, (uint8_t*)(&temp.Data1 + 1), this->data.data());
        (void)std::copy((uint8_t*)&temp.Data2, (uint8_t*)(&temp.Data2 + 1), this->data.data() + 4);
        (void)std::copy((uint8_t*)&temp.Data3, (uint8_t*)(&temp.Data3 + 1), this->data.data() + 6);
        (void)std::copy(nativeUUID.Data4, nativeUUID.Data4 + 8, this->data.data() + 8);

#endif

        return *this;
    }

    UUID& UUID::operator=(const std::string& uuidStr)
    {
        if (uuidStr.size() != 36)
        {
            HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Invalid UUID string.");
        }

        std::array<char, 3> byteStr = { 0, 0, 0 };
        std::istringstream byteIss(byteStr.data());
        std::istringstream iss(uuidStr);
        unsigned int byteVal;

        for (size_t i = 0; i < this->data.size(); ++i)
        {
            if (i == 4 || i == 6 || i == 8 || i == 10) iss.ignore(1); // discard dash 
            
            // iss cannot read byte hex, but can read int hex.
            // Hence extract one-byte hex as string (e.g., extract "A5" from "A52E4FDC") and
            // convert it.  
            iss >> byteStr[0] >> byteStr[1];

            // reset the byteIss.
            byteIss.str(byteStr.data());
            byteIss.seekg(0);
            
            byteIss >> std::hex >> byteVal;
            this->data[i] = static_cast<uint8_t>(byteVal);
        }

        return *this;
    }

    UUID& UUID::operator=(const std::wstring& uuidStr)
    {
        return (*this) = std::string(uuidStr.begin(), uuidStr.end());
    }

    UUID::operator NativeUUID() const
    {
#ifdef _WIN32

        NativeUUID nativeUUID;
        nativeUUID.Data1 = Heph::BigEndianToNative(*reinterpret_cast<const unsigned long*>(this->data.data()));
        nativeUUID.Data2 = Heph::BigEndianToNative(*reinterpret_cast<const unsigned short*>(this->data.data() + 4));
        nativeUUID.Data3 = Heph::BigEndianToNative(*reinterpret_cast<const unsigned short*>(this->data.data() + 6));
        (void)std::copy(this->data.data() + 8, this->data.data() + this->data.size(), nativeUUID.Data4);
        return nativeUUID;

#endif
    }

    UUID::operator std::string() const
    {
        return std::format("{:02X}{:02X}{:02X}{:02X}-{:02X}{:02X}-{:02X}{:02X}-{:02X}{:02X}-{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}",
            this->data[0], this->data[1], this->data[2], this->data[3],
            this->data[4], this->data[5],
            this->data[6], this->data[7],
            this->data[8],
            this->data[9],
            this->data[10],
            this->data[11],
            this->data[12],
            this->data[13],
            this->data[14],
            this->data[15]
        );
    }

    UUID::operator std::wstring() const
    {
        const std::string uuidStr = static_cast<std::string>(*this);
        return std::wstring(uuidStr.begin(), uuidStr.end());
    }

    bool UUID::operator==(const UUID& rhs) const
    {
        return this->data == rhs.data;
    }

    bool UUID::operator!=(const UUID& rhs) const
    {
        return this->data != rhs.data;
    }

    void UUID::Generate()
    {
        NativeUUID nativeUUID;

#if defined(_WIN32)

        HRESULT hres = CoCreateGuid(&nativeUUID);
        if (hres != S_OK)
        {
            _com_error(hres).ErrorMessage();
            HEPH_EXCEPTION_RAISE_AND_THROW(ExternalException, HEPH_FUNC, "Native create UUID method failed.", "COM", Native::ComErrorStr(hres));
        }
        (*this) = nativeUUID;

#endif
    }

    UUID UUID::Create()
    {
        UUID uuid;
        uuid.Generate();
        return uuid;
    }

    std::ostream& operator<<(std::ostream& os, const UUID& uuid)
    {
        os << static_cast<std::string>(uuid);
        return os;
    }

    std::istream& operator>>(std::istream& is, UUID& uuid)
    {
        std::string uuidStr;
        is >> uuidStr;
        uuid = uuidStr;
        return is;
    }
}
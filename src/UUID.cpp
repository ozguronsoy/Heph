#include "Heph/UUID.h"
#include "Heph/Exceptions/ExternalException.h"
#include "Heph/Exceptions/InvalidArgumentException.h"
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <algorithm>

#ifdef _WIN32
#include "Heph/Native/WinHelpers.h"
#include <combaseapi.h>
#elif defined(__ANDROID__)
#endif

namespace Heph
{
    UUID::UUID()
    {
        this->data.fill(0);
    }

    UUID::UUID(const Native& nativeUUID)
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

    UUID& UUID::operator=(const Native& nativeUUID)
    {
#ifdef _WIN32

        Native temp;
        temp.Data1 = std::byteswap(nativeUUID.Data1);
        temp.Data2 = std::byteswap(nativeUUID.Data2);
        temp.Data3 = std::byteswap(nativeUUID.Data3);

        (void)std::copy((uint8_t*)&temp.Data1, (uint8_t*)(&temp.Data1 + 1), this->data.data());
        (void)std::copy((uint8_t*)&temp.Data2, (uint8_t*)(&temp.Data2 + 1), this->data.data() + 4);
        (void)std::copy((uint8_t*)&temp.Data3, (uint8_t*)(&temp.Data3 + 1), this->data.data() + 6);
        (void)std::copy(nativeUUID.Data4, nativeUUID.Data4 + 8, this->data.data() + 8);

#elif defined(__ANDROID__)

        JNIEnv* env = Heph::Native::AndroidHelpers::GetEnv();
        (*this) = Heph::Native::AndroidHelpers::JStringToStdString(env, nativeUUID);

#elif defined(__linux__)

        (void)std::copy(nativeUUID.uuid, nativeUUID.uuid + 16, this->data.data());

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
            if (i == 4 || i == 6 || i == 8 || i == 10)
            {
                iss >> byteStr[0]; // discard dash
                if (byteStr[0] != '-')
                {
                    HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Invalid UUID string.");
                }
            }

            // iss cannot read byte hex, but can read int hex.
            // Hence extract one-byte hex as string (e.g., extract "A5" from "A52E4FDC") and
            // convert it.  
            iss >> byteStr[0] >> byteStr[1];

            if (!std::all_of(byteStr.data(), byteStr.data() + 2, ::isxdigit))
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Invalid UUID string.");
            }

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

    UUID& UUID::operator=(const UUID& rhs)
    {
        this->data = rhs.data;
        return *this;
    }

    UUID::operator Native() const
    {
        Native nativeUUID;

#ifdef _WIN32

        nativeUUID.Data1 = std::byteswap(*reinterpret_cast<const unsigned long*>(this->data.data()));
        nativeUUID.Data2 = std::byteswap(*reinterpret_cast<const unsigned short*>(this->data.data() + 4));
        nativeUUID.Data3 = std::byteswap(*reinterpret_cast<const unsigned short*>(this->data.data() + 6));
        (void)std::copy(this->data.data() + 8, this->data.data() + this->data.size(), nativeUUID.Data4);

#elif defined(__ANDROID__)

        JNIEnv* env = Heph::Native::AndroidHelpers::GetEnv();
        nativeUUID = Heph::Native::AndroidHelpers::StdStringToJString(env, static_cast<std::string>(*this));

#elif defined(__linux__)

        (void)std::copy(this->data.begin(), this->data.end(), nativeUUID.uuid);

#endif

        return nativeUUID;
    }

    UUID::operator std::string() const
    {
        return std::format("{:02X}{:02X}{:02X}{:02X}-{:02X}{:02X}-{:02X}{:02X}-{:02X}{:02X}-{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}",
            this->data[0], this->data[1], this->data[2], this->data[3],
            this->data[4], this->data[5],
            this->data[6], this->data[7],
            this->data[8], this->data[9],
            this->data[10], this->data[11], this->data[12], this->data[13], this->data[14], this->data[15]
        );
    }

    UUID::operator std::wstring() const
    {
        const std::string uuidStr = static_cast<std::string>(*this);
        return std::wstring(uuidStr.begin(), uuidStr.end());
    }

    void UUID::Generate()
    {
        Native nativeUUID;

#ifdef _WIN32

        HRESULT hres = CoCreateGuid(&nativeUUID);
        if (hres != S_OK)
        {
            _com_error(hres).ErrorMessage();
            HEPH_EXCEPTION_RAISE_AND_THROW(ExternalException, HEPH_FUNC, "Native create UUID method failed.", "COM", Heph::Native::WinHelpers::ComErrorStr(hres));
        }

#elif defined(__ANDROID__)

        JNIEnv* env = Heph::Native::AndroidHelpers::GetEnv();

        jclass uuidClass = env->FindClass("java/util/UUID");
        if (uuidClass == nullptr)
        {
            HEPH_EXCEPTION_RAISE_AND_THROW(ExternalException, HEPH_FUNC, "UUID jclass not found.", "JNI", "");
        }

        jmethodID randomUUIDMethod = env->GetStaticMethodID(uuidClass, "randomUUID", "()Ljava/util/UUID;");
        if (randomUUIDMethod == nullptr)
        {
            HEPH_EXCEPTION_RAISE_AND_THROW(ExternalException, HEPH_FUNC, "UUID::randomUUID jmethod not found.", "JNI", "");
        }

        jobject uuidObject = env->CallStaticObjectMethod(uuidClass, randomUUIDMethod);
        if (uuidObject == nullptr)
        {
            HEPH_EXCEPTION_RAISE_AND_THROW(ExternalException, HEPH_FUNC, "Failed to call UUID::randomUUID jmethod.", "JNI", "");
        }

        jmethodID toStringMethod = env->GetMethodID(uuidClass, "toString", "()Ljava/lang/String;");
        if (toStringMethod == nullptr)
        {
            HEPH_EXCEPTION_RAISE_AND_THROW(ExternalException, HEPH_FUNC, "Failed to call UUID::randomUUID jmethod.", "JNI", "");
        }

        nativeUUID = (jstring)env->CallObjectMethod(uuidObject, toStringMethod);

#elif defined(__linux__)

        uuid_generate(nativeUUID.uuid);

#endif

        (*this) = nativeUUID;
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
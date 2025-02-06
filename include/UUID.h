#ifndef HEPH_UUID_H
#define HEPH_UUID_H

#include "HephShared.h"
#include "HephEndian.h"
#include <cstdint>
#include <array>
#include <string>
#include <iostream>
#include <format>

#ifdef _WIN32
#include <comdef.h>
#elif defined(__ANDROID__)
#include "AndroidHelpers.h"
#elif defined(__linux__)
#include <uuid/uuid.h>
struct __heph_uuid_t_wrapper
{
    uuid_t uuid;
};
#else
#error unsupported platform.
#endif

/** @file */

namespace Heph
{
    /** @brief Class for generating and storing UUIDs. */
    class HEPH_API UUID final
    {
    public:
        /** @brief Native UUID type used by the target platform. */
#ifdef _WIN32
        using Native = GUID;
#elif defined(__ANDROID__)
        using Native = jstring;
#elif defined(__linux__)
        using Native = __heph_uuid_t_wrapper;
#endif

    private:
        /** UUID data in big-endian order. */
        std::array<uint8_t, 16> data;

    public:
        /** @copydoc default_constructor */
        UUID();

        /**
         * @copydoc constructor
         *
         * @param nativeUUID Native UUID to copy.
         */
        UUID(const Native& nativeUUID);

        /**
         * @copydoc constructor
         *
         * @param uuidStr UUID string.
         */
        explicit UUID(const std::string& uuidStr);

        /** @copydoc UUID(const std::string&) */
        explicit UUID(const std::wstring& uuidStr);

        UUID& operator=(const Native& nativeUUID);
        UUID& operator=(const std::string& uuidStr);
        UUID& operator=(const std::wstring& uuidStr);

        /** Converts Heph::UUID to native. */
        operator Native() const;
        /** Converts UUID to string. */
        explicit operator std::string() const;
        /** Converts UUID to wide string. */
        explicit operator std::wstring() const;

        /**
         * Compares two UUIDs.
         *
         * @param rhs UUID to compare.
         * @return true if both UUIDs have the same value, otherwise false.
         */
        bool operator==(const UUID& rhs) const = default;

        /** Generates new UUID via native methods. */
        void Generate();

        /** Creates a new UUID instance and calls UUID::Generate. */
        static UUID Create();

        /**
         * Inserts the UUID to the output stream.
         *
         * @param os Output stream.
         * @param uuid Universally unique identifier.
         * @return The output stream.
         */
        friend std::ostream& operator<<(std::ostream& os, const UUID& uuid);

        /**
         * Extracts UUID from the input stream.
         *
         * @param is Input stream.
         * @param uuid Universally unique identifier.
         * @return The input stream.
         */
        friend std::istream& operator>>(std::istream& is, UUID& uuid);

        friend std::formatter<UUID, char>;
        friend std::hash<UUID>;
    };
}

/** @brief Formatting support for Heph::UUID. */
template<>
struct HEPH_API std::formatter<Heph::UUID, char>
{
    constexpr std::format_parse_context::iterator parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    constexpr std::format_context::iterator format(const Heph::UUID& uuid, std::format_context& ctx) const
    {
        return std::format_to(ctx.out(),
            "{:02X}{:02X}{:02X}{:02X}-{:02X}{:02X}-{:02X}{:02X}-{:02X}{:02X}-{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}",
            uuid.data[0], uuid.data[1], uuid.data[2], uuid.data[3],
            uuid.data[4], uuid.data[5],
            uuid.data[6], uuid.data[7],
            uuid.data[8], uuid.data[9],
            uuid.data[10], uuid.data[11], uuid.data[12], uuid.data[13], uuid.data[14], uuid.data[15]
        );
    }
};

/** @brief Hash support for Heph::UUID. */
template<>
struct HEPH_API std::hash<Heph::UUID>
{
    size_t operator()(const Heph::UUID& uuid) const
    {
        const uint64_t* data = reinterpret_cast<const uint64_t*>(uuid.data.data());
        std::hash<uint64_t> hash;
        return hash(data[0]) ^ hash(data[1]);
    }
};

#endif
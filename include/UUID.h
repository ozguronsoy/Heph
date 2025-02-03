#ifndef HEPH_UUID_H
#define HEPH_UUID_H

#include "HephShared.h"
#include <cstdint>
#include <array>
#include <string>
#include <iostream>

#ifdef _WIN32
#include <comdef.h>
typedef GUID NativeUUID;
#else
#error unsupported platform, please provide UUID implementation.
#endif

/** @file */

namespace Heph
{
    /** @brief Class for creating and storing UUIDs.   */
    class HEPH_API UUID final
    {
    private:
        /** Actual UUID data. */
        std::array<uint8_t, 16> data;

    public:
        /** @copydoc default_constructor */
        UUID();

        /**
         * @copydoc constructor
         * 
         * @param nativeUUID Native UUID to copy.
         */
        UUID(const NativeUUID& nativeUUID);

        /**
         * @copydoc constructor
         * 
         * @param uuidStr UUID string.
         */
        explicit UUID(const std::string& uuidStr);

        /** @copydoc UUID(const std::string&) */
        explicit UUID(const std::wstring& uuidStr);

        UUID& operator=(const NativeUUID& nativeUUID);
        UUID& operator=(const std::string& uuidStr);
        UUID& operator=(const std::wstring& uuidStr);

        /** Converts Heph::UUID to native. */
        operator NativeUUID() const;
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
        bool operator==(const UUID& rhs) const;

        /**
         * Compares two UUIDs.
         *
         * @param rhs UUID to compare.
         * @return false if both UUIDs have the same value, otherwise true.
         */
        bool operator!=(const UUID& rhs) const;

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
    };
}

#endif
#ifndef HEPH_ENUM_H
#define HEPH_ENUM_H

#include "HephShared.h"
#include <type_traits>

/** @file */

namespace Heph
{
    /**
     * @brief Wrapper for C enums and scoped enums.
     *
     * @tparam T Type of the enum.
     */
    template<typename T>
        requires (std::is_enum_v<T> || std::is_scoped_enum_v<T>)
    struct Enum final
    {
        /** Value of the enum. */
        T value;

        /** @copydoc constructor
         *
         * @param v @copydetails value
         */
        constexpr Enum(T v = T())
            : value(v)
        {
        }

        /**
         * Assigns the enum to current instance.
         * 
         * @param rhs Enum to assign.
         * @return Reference to current instance.
         */
        constexpr Enum& operator=(T rhs)
        {
            this->value = rhs;
            return *this;
        }

        /** @copydoc operator=(T) */
        constexpr Enum& operator=(Enum rhs)
        {
            this->value = rhs.value;
            return *this;
        }

        /** Converts Heph::Enum to T. */
        constexpr operator T() const
        {
            return this->value;
        }

        /**
         * Compares two enums.
         *
         * @param rhs Enum to compare.
         * @return ``true`` if value of both enums are same, otherwise ``false``.
         */
        constexpr bool operator==(T rhs) const
        {
            return this->value == rhs;
        }

        /**
         * Checks whether the values are set.
         *
         * @param v Values to check.
         * @return ``true`` if all bits of the ``v`` are set, otherwise ``false``.
         */
        constexpr bool Test(T v) const
        {
            if constexpr (std::is_scoped_enum_v<T>) return (Enum::AsInt(this->value) & Enum::AsInt(v)) == Enum::AsInt(v);
            else return (this->value & v) == v;
        }

        /**
         * Or with ``v``.
         *
         * @param v Bits to set.
         */
        constexpr void Set(T v)
        {
            if constexpr (std::is_scoped_enum_v<T>)
            {
                auto result = Enum::AsInt(this->value) | Enum::AsInt(v);
                this->value = *reinterpret_cast<T*>(&result);
            }
            else
            {
                this->value = static_cast<T>(this->value | v);
            }
        }

    private:
        /**
         * Converts enum to it's integer representation.
         *
         * @param v Value of the enum.
         * @return Enum as int.
         */
        static constexpr auto AsInt(T v)
        {
            if constexpr (sizeof(T) == sizeof(uint64_t)) return *reinterpret_cast<uint64_t*>(&v);
            else if constexpr (sizeof(T) == sizeof(uint32_t)) return *reinterpret_cast<uint32_t*>(&v);
            else if constexpr (sizeof(T) == sizeof(uint16_t)) return *reinterpret_cast<uint16_t*>(&v);
            else return *reinterpret_cast<uint8_t*>(&v);
        }
    };
}

#endif
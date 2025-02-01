#ifndef HEPH_ENDIAN_H
#define HEPH_ENDIAN_H

#include "HephShared.h"
#include <bit>
#include <type_traits>

/** @file */

/** @brief Swaps the bytes of 16-bit data */
#define HEPH_SWAP_ENDIAN_16(x) ((((x) & 0xFF00) >> 8) | (((x) & 0x00FF) << 8))
/** @brief Swaps the bytes of 24-bit data */
#define HEPH_SWAP_ENDIAN_24(x) ((((x) & 0xFF0000) >> 16) | ((x) & 0x00FF00) | (((x) & 0x0000FF) << 16))
/** @brief Swaps the bytes of 32-bit data */
#define HEPH_SWAP_ENDIAN_32(x) ((((x) & 0xFF000000) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | (((x) & 0x000000FF) << 24))
/** @brief Swaps the bytes of 64-bit data */
#define HEPH_SWAP_ENDIAN_64(x)  (                                       \
                                (((x) & 0xFF00000000000000) >> 56)  |   \
                                (((x) & 0x00FF000000000000) >> 40)  |   \
                                (((x) & 0x0000FF0000000000) >> 24)  |   \
                                (((x) & 0x000000FF00000000) >> 8)   |   \
                                (((x) & 0x00000000FF000000) << 8)   |   \
                                (((x) & 0x0000000000FF0000) << 24)  |   \
                                (((x) & 0x000000000000FF00) << 40)  |   \
                                (((x) & 0x00000000000000FF) << 56)      \
                                )


namespace Heph
{
    /**
     * Swaps the bytes of the data and returns the result as a new instance.
     * 
     * @tparam T Type of the data.
     * @param data Data to be processed.
     * @return Byte swapped data. 
     */
    template<typename T>
    inline constexpr T SwapEndian(T data)
    {
        static_assert(sizeof(T) != 1, "sizeof T must be greater than 1.");
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, "T must be a primitive type.");

        if constexpr (sizeof(T) == 2) return HEPH_SWAP_ENDIAN_16(data);
        else if constexpr (sizeof(T) == 3) return HEPH_SWAP_ENDIAN_24(data);
        else if constexpr (sizeof(T) == 4) return HEPH_SWAP_ENDIAN_32(data);
        else if constexpr (sizeof(T) == 8) return HEPH_SWAP_ENDIAN_64(data);
        else static_assert (false, "invalid sizeof T.");
    }

    /**
     * Swaps the bytes of the data and returns the result as a new instance.
     * 
     * @tparam T Type of the data.
     * @param data Data to be processed.
     * @param endian Current endianness of the data.
     * @return Byte swapped data. 
     */
    template<typename T>
    inline constexpr T SwapEndian(T data, std::endian& endian)
    {
        endian = (endian == std::endian::little) ? (std::endian::big) : (std::endian::little);
        return Heph::SwapEndian<T>(data);
    }
}

#endif
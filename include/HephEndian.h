#ifndef HEPH_ENDIAN_H
#define HEPH_ENDIAN_H

#include "HephShared.h"
#include "HephConcepts.h"
#include <bit>
#include <stdint.h>
#include <stddef.h>
#include <utility>

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
    template<Primitive T>
        requires (sizeof(T) > 1)
    constexpr inline T SwapEndian(T data)
    {
        if constexpr (sizeof(T) == 2) return HEPH_SWAP_ENDIAN_16(data);
        else if constexpr (sizeof(T) == 3) return HEPH_SWAP_ENDIAN_24(data);
        else if constexpr (sizeof(T) == 4) return HEPH_SWAP_ENDIAN_32(data);
        else if constexpr (sizeof(T) == 8) return HEPH_SWAP_ENDIAN_64(data);
        else
        {
            const size_t halfSize = sizeof(T) / 2;
            uint8_t* pData = &data;

            for (size_t i = 0; i < halfSize; ++i)
            {
                std::swap(pData[i], pData[sizeof(T) - i - 1]);
            }

            return data;
        }
    }

    /**
     * Swaps the bytes of the data and returns the result as a new instance.
     *
     * @tparam T Type of the data.
     * @param data Data to be processed.
     * @param endian Current endianness of the data.
     * @return Byte swapped data.
     */
    template<Primitive T>
        requires (sizeof(T) > 1)
    constexpr inline T SwapEndian(T data, std::endian& endian)
    {
        endian = (endian == std::endian::little) ? (std::endian::big) : (std::endian::little);
        return Heph::SwapEndian<T>(data);
    }

    /**
     * Converts data from native endian to little endian.
     * @note This method does nothing if native endianness is little.
     *
     * @tparam T Type of the data.
     * @param data Data to be processed.
     * @return Data as little endian.
     */
    template<Primitive T>
        requires (sizeof(T) > 1)
    constexpr inline T NativeToLittleEndian(T data)
    {
        if constexpr (std::endian::native == std::endian::little)
            return data;
        else
            return Heph::SwapEndian(data);
    }

    /**
     * Converts data from native endian to big endian.
     * @note This method does nothing if native endianness is big.
     *
     * @tparam T Type of the data.
     * @param data Data to be processed.
     * @return Data as big endian.
     */
    template<Primitive T>
        requires (sizeof(T) > 1)
    constexpr inline T NativeToBigEndian(T data)
    {
        if constexpr (std::endian::native == std::endian::big)
            return data;
        else
            return Heph::SwapEndian(data);
    }

    /**
     * Converts data from little endian to native endian.
     * @note This method does nothing if native endianness is little.
     *
     * @tparam T Type of the data.
     * @param data Data to be processed.
     * @return Data as native endian.
     */
    template<Primitive T>
        requires (sizeof(T) > 1)
    constexpr inline T LittleEndianToNative(T data)
    {
        return Heph::NativeToLittleEndian(data);
    }

    /**
     * Converts data from big endian to native endian.
     * @note This method does nothing if native endianness is big.
     *
     * @tparam T Type of the data.
     * @param data Data to be processed.
     * @return Data as native endian.
     */
    template<Primitive T>
        requires (sizeof(T) > 1)
    constexpr inline T BigEndianToNative(T data)
    {
        return Heph::NativeToBigEndian(data);
    }
}

#endif
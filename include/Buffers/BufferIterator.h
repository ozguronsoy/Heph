#ifndef HEPH_BUFFER_ITERATOR_H
#define HEPH_BUFFER_ITERATOR_H

#include "HephShared.h"
#include "BufferFlags.h"
#include "Enum.h"
#include "Exceptions/InvalidArgumentException.h"
#include <type_traits>
#include <array>
#include <functional>

/** @file */

namespace Heph
{
    /** Specifies that the type is suitable to be stored in a Heph::Buffer. */
    template<typename T>
    concept BufferElement = std::is_default_constructible_v<T> && std::is_trivially_destructible_v<T>;

    /**
     * @brief Iterator for Heph::Buffer.
     *
     * @tparam TData Type of the elements stored in buffer.
     * @tparam NDimensions Number of dimensions.
     */
    template<BufferElement TData, size_t NDimensions>
        requires (NDimensions > 0)
    class HEPH_API BufferIterator final
    {
    public:
        /** @brief Difference type for std::iterator_traits. */
        using difference_type = std::ptrdiff_t;
        /** @brief Value type for std::iterator_traits. */
        using value_type = TData;
        /** @brief Pointer type for std::iterator_traits. */
        using pointer = std::conditional_t<std::is_const_v<value_type>, value_type* const, value_type*>;
        /** @brief Reference type for std::iterator_traits. */
        using reference = value_type&;
        /** @brief Iterator tag for std::iterator_traits. */
        using iterator_category = std::forward_iterator_tag;

        /** @brief ``size_t`` for 1D buffers, an array of ``size_t`` for multidimensional buffers. */
        using buffer_size_t = std::conditional_t<NDimensions == 1, size_t, std::array<size_t, NDimensions>>;
        /** @brief ``buffer_size_t`` instance with ``0`` for all dimensions. */
        static constexpr buffer_size_t BUFFER_SIZE_ZERO = {};

    private:
        pointer& ptr;
        const Enum<BufferFlags>& flags;
        const buffer_size_t& bufferSize;
        const buffer_size_t& strides;
        buffer_size_t indices;

    public:
        constexpr BufferIterator(pointer& ptr, const Enum<BufferFlags>& flags, const buffer_size_t& bufferSize, const buffer_size_t& strides)
            : ptr(ptr), flags(flags), bufferSize(bufferSize), strides(strides), indices(BUFFER_SIZE_ZERO)
        {
        }

        constexpr reference operator*()
        {
            return BufferIterator::Get<false>(this->ptr, this->flags, this->bufferSize, this->strides, this->indices);
        }

        constexpr pointer operator->()
        {
            return &**this;
        }

        constexpr BufferIterator operator+(size_t i0) const
        {
            BufferIterator result = *this;
            result += i0;
            return result;
        }

        template<size_t NDim = NDimensions>
        constexpr typename std::enable_if_t<(NDim > 1), BufferIterator> operator+(const buffer_size_t& rhs) const
        {
            BufferIterator result = *this;
            result += rhs;
            return result;
        }

        constexpr BufferIterator& operator+=(size_t i0)
        {
            if constexpr (NDimensions == 1) this->indices = this->flags.Test(BufferFlags::Circular) ? ((this->indices + i0) % this->bufferSize) : (this->indices + i0);
            else this->indices[0] = this->flags.Test(BufferFlags::Circular) ? ((this->indices[0] + i0) % this->bufferSize[0]) : (this->indices[0] + i0);

            return *this;
        }

        template<size_t NDim = NDimensions>
        constexpr typename std::enable_if_t<(NDim > 1), BufferIterator&> operator+=(const buffer_size_t& rhs)
        {
            for (size_t i = 0; i < NDimensions; ++i)
            {
                this->indices[i] =
                    this->flags.Test(BufferFlags::Circular)
                    ? ((this->indices[i] + rhs[i]) % this->bufferSize[i])
                    : (this->indices[i] + rhs[i]);
            }

            return *this;
        }

        constexpr BufferIterator operator-(size_t i0) const
        {
            BufferIterator result = *this;
            result -= i0;
            return result;
        }

        template<size_t NDim = NDimensions>
        constexpr typename std::enable_if_t<(NDim > 1), BufferIterator> operator-(const buffer_size_t& rhs) const
        {
            BufferIterator result = *this;
            result -= rhs;
            return result;
        }

        constexpr BufferIterator& operator-=(size_t i0)
        {
            if constexpr (NDimensions == 1) this->indices = this->flags.Test(BufferFlags::Circular) ? ((this->indices - i0) % this->bufferSize) : (this->indices - i0);
            else this->indices[0] = this->flags.Test(BufferFlags::Circular) ? ((this->indices[0] - i0) % this->bufferSize[0]) : (this->indices[0] - i0);

            return *this;
        }

        template<size_t NDim = NDimensions>
        constexpr typename std::enable_if_t<(NDim > 1), BufferIterator&> operator-=(const buffer_size_t& rhs)
        {
            for (size_t i = 0; i < NDimensions; ++i)
            {
                this->indices[i] =
                    this->flags.Test(BufferFlags::Circular)
                    ? ((this->indices[i] - rhs[i]) % this->bufferSize[i])
                    : (this->indices[i] - rhs[i]);
            }

            return *this;
        }

        constexpr BufferIterator& operator++()
        {
            if constexpr (NDimensions == 1) this->indices++;
            else this->IncrementIndex(NDimensions - 1);

            return *this;
        }

        constexpr BufferIterator& operator++(int)
        {
            BufferIterator temp = *this;
            this->operator++();
            return temp;
        }


        constexpr BufferIterator& operator--()
        {
            if constexpr (NDimensions == 1) this->indices--;
            else this->DecrementIndex(NDimensions - 1);

            return *this;
        }

        constexpr BufferIterator& operator--(int)
        {
            BufferIterator temp = *this;
            this->operator--();
            return temp;
        }

        constexpr bool operator==(const BufferIterator& rhs) const
        {
            return this->ptr == rhs.ptr && this->indices == rhs.indices;
        }

        constexpr reference Get()
        {
            return BufferIterator::Get<true>(this->ptr, this->flags, this->bufferSize, this->strides, this->indices);
        }

        template<bool CheckErrors, typename... Args>
            requires (sizeof...(Args) <= NDimensions && (std::is_convertible_v<Args, size_t> && ...))
        static constexpr reference Get(pointer& ptr, const Enum<BufferFlags>& flags, const buffer_size_t& bufferSize, const buffer_size_t& strides, Args... args)
        {
            const std::array<size_t, NDimensions> indices = { static_cast<size_t>(std::forward<Args>(args))... };
            if constexpr (NDimensions == 1) return BufferIterator::Get<CheckErrors>(ptr, flags, bufferSize, strides, indices[0]);
            else return BufferIterator::Get<CheckErrors>(ptr, flags, bufferSize, strides, indices);
        }

        template<bool CheckErrors>
        static constexpr reference Get(pointer& ptr, const Enum<BufferFlags>& flags, const buffer_size_t& bufferSize, const buffer_size_t& strides, const buffer_size_t& indices)
        {
            if constexpr (NDimensions == 1)
            {
                if constexpr (CheckErrors)
                {
                    if (indices >= bufferSize)
                    {
                        HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Index out of bounds.");
                    }
                }

                return flags.Test(BufferFlags::Circular) ? (ptr[indices % bufferSize]) : (ptr[indices]);
            }
            else
            {
                size_t n = 0;
                for (size_t i = 0; i < NDimensions; ++i)
                {
                    if constexpr (CheckErrors)
                    {
                        if (indices[i] >= bufferSize[i])
                        {
                            HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Index out of bounds.");
                        }
                    }

                    n +=
                        flags.Test(BufferFlags::Circular)
                        ? ((indices[i] % bufferSize[i]) * strides[i])
                        : (indices[i] * strides[i]);
                }
                return ptr[n];
            }
        }

    private:
        constexpr void IncrementIndex(size_t n)
        {
            this->indices[n]++;
            if (this->indices[n] == this->bufferSize[n] && n > 0)
            {
                this->indices[n] = 0;
                this->IncrementIndex(n - 1);
            }
        }

        constexpr void DecrementIndex(size_t n)
        {
            if (this->indices[n] == 0)
            {
                if (n > 0)
                {
                    this->indices[n] = this->bufferSize[n] - 1;
                    this->DecrementIndex(n - 1);
                }
            }
            else
            {
                this->indices[n]--;
            }
        }
    };
}

#endif
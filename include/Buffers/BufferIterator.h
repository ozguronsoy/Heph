#ifndef HEPH_BUFFER_ITERATOR_H
#define HEPH_BUFFER_ITERATOR_H

#include "HephShared.h"
#include "BufferFlags.h"
#include "HephConcepts.h"
#include "Enum.h"
#include "Exceptions/InvalidArgumentException.h"
#include <type_traits>
#include <array>
#include <functional>
#include <tuple>

/** @file */

namespace Heph
{
    /** @brief Specifies that the type can be stored in a buffer. */
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
        using iterator_category = std::bidirectional_iterator_tag;
        /** @brief ``size_t`` for 1D buffers, an array of ``size_t`` for multidimensional buffers. */
        using buffer_size_t = std::conditional_t<NDimensions == 1, size_t, std::array<size_t, NDimensions>>;

        /** @brief ``buffer_size_t`` instance with ``0`` for all dimensions. */
        static constexpr buffer_size_t BUFFER_SIZE_ZERO = {};

    private:
        pointer* ppData;
        Enum<BufferFlags> const* pFlags;
        buffer_size_t const* pBufferSize;
        buffer_size_t const* pStrides;
        buffer_size_t indices;

    public:
        BufferIterator(pointer& ptr, const Enum<BufferFlags>& flags, const buffer_size_t& bufferSize, const buffer_size_t& strides)
            : ppData(&ptr), pFlags(&flags), pBufferSize(&bufferSize), pStrides(&strides), indices(BUFFER_SIZE_ZERO)
        {
        }

        reference operator*()
        {
            return BufferIterator::Get<false>(*this->ppData, *this->pFlags, *this->pBufferSize, *this->pStrides, this->indices);
        }

        pointer operator->()
        {
            return &this->operator*();
        }

        BufferIterator operator+(size_t i) const
        {
            BufferIterator result = *this;
            result += i;
            return result;
        }

        template<size_t NDim = NDimensions>
            requires (NDim == NDimensions)
        typename std::enable_if_t<(NDim > 1), BufferIterator> operator+(const buffer_size_t& rhs) const
        {
            BufferIterator result = *this;
            result += rhs;
            return result;
        }

        BufferIterator& operator+=(size_t i)
        {
            if constexpr (NDimensions == 1) this->indices = (*this->pFlags).Test(BufferFlags::Circular) ? ((this->indices + i) % (*this->pBufferSize)) : (this->indices + i);
            else this->indices[NDimensions - 1] = (*this->pFlags).Test(BufferFlags::Circular) ? ((this->indices[NDimensions - 1] + i) % (*this->pBufferSize)[NDimensions - 1]) : (this->indices[NDimensions - 1] + i);

            return *this;
        }

        template<size_t NDim = NDimensions>
            requires (NDim == NDimensions)
        typename std::enable_if_t<(NDim > 1), BufferIterator&> operator+=(const buffer_size_t& rhs)
        {
            for (size_t i = 0; i < NDimensions; ++i)
            {
                this->indices[i] =
                    (*this->pFlags).Test(BufferFlags::Circular)
                    ? ((this->indices[i] + rhs[i]) % (*this->pBufferSize)[i])
                    : (this->indices[i] + rhs[i]);
            }

            return *this;
        }

        BufferIterator operator-(size_t i) const
        {
            BufferIterator result = *this;
            result -= i;
            return result;
        }

        template<size_t NDim = NDimensions>
            requires (NDim == NDimensions)
        typename std::enable_if_t<(NDim > 1), BufferIterator> operator-(const buffer_size_t& rhs) const
        {
            BufferIterator result = *this;
            result -= rhs;
            return result;
        }

        BufferIterator& operator-=(size_t i)
        {
            if constexpr (NDimensions == 1) this->indices = (*this->pFlags).Test(BufferFlags::Circular) ? ((this->indices - i) % (*this->pBufferSize)) : (this->indices - i);
            else this->indices[NDimensions - 1] = (*this->pFlags).Test(BufferFlags::Circular) ? ((this->indices[NDimensions - 1] - i) % (*this->pBufferSize)[NDimensions - 1]) : (this->indices[NDimensions - 1] - i);

            return *this;
        }

        template<size_t NDim = NDimensions>
            requires (NDim == NDimensions)
        typename std::enable_if_t<(NDim > 1), BufferIterator&> operator-=(const buffer_size_t& rhs)
        {
            for (size_t i = 0; i < NDimensions; ++i)
            {
                this->indices[i] =
                    (*this->pFlags).Test(BufferFlags::Circular)
                    ? ((this->indices[i] - rhs[i]) % (*this->pBufferSize)[i])
                    : (this->indices[i] - rhs[i]);
            }

            return *this;
        }

        BufferIterator& operator++()
        {
            if constexpr (NDimensions == 1) this->indices++;
            else this->IncrementIndex(NDimensions - 1);

            return *this;
        }

        BufferIterator& operator++(int)
        {
            BufferIterator temp = *this;
            this->operator++();
            return temp;
        }


        BufferIterator& operator--()
        {
            if constexpr (NDimensions == 1) this->indices--;
            else this->DecrementIndex(NDimensions - 1);

            return *this;
        }

        BufferIterator& operator--(int)
        {
            BufferIterator temp = *this;
            this->operator--();
            return temp;
        }

        bool operator==(const BufferIterator& rhs) const
        {
            return this->ppData == rhs.ppData && this->indices == rhs.indices;
        }

        const buffer_size_t& Indices() const
        {
            return this->indices;
        }

        template<bool CheckErrors, size_t NDim = NDimensions>
            requires (NDim == NDimensions)
        static typename std::enable_if_t<(NDim > 1), reference> Get(pointer& ptr, const Enum<BufferFlags>& flags, const buffer_size_t& bufferSize, const buffer_size_t& strides, const auto... indices)
        {
            static_assert(sizeof...(indices) > 0 && sizeof...(indices) <= NDimensions, "Invalid number of indices parameters.");
            static_assert((std::is_convertible_v<decltype(indices), size_t> && ...), "Invalid type for indices parameters, must be convertible to size_t.");

            const buffer_size_t indicesArray = { std::forward<size_t>(static_cast<size_t>(indices))... };
            return BufferIterator::Get<CheckErrors>(ptr, flags, bufferSize, strides, indicesArray);
        }

        template<bool CheckErrors>
        static reference Get(pointer& ptr, const Enum<BufferFlags>& flags, const buffer_size_t& bufferSize, const buffer_size_t& strides, const buffer_size_t& indices)
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
        void IncrementIndex(size_t n)
        {
            this->indices[n]++;
            if (this->indices[n] == (*this->pBufferSize)[n] && n > 0)
            {
                this->indices[n] = 0;
                this->IncrementIndex(n - 1);
            }
        }

        void DecrementIndex(size_t n)
        {
            if (this->indices[n] == 0)
            {
                if (n > 0)
                {
                    this->indices[n] = (*this->pBufferSize)[n] - 1;
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
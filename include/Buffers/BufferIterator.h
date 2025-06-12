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
#include <compare>

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
        /** @brief ``index_t`` for 1D buffers, an array of ``index_t`` for multidimensional buffers. */
        using buffer_index_t = std::conditional_t<NDimensions == 1, index_t, std::array<index_t, NDimensions>>;

        /** @brief ``buffer_size_t`` instance with ``0`` for all dimensions. */
        static constexpr buffer_size_t BUFFER_SIZE_ZERO = {};
        /** @brief ``buffer_index_t`` instance with ``0`` for all dimensions. */
        static constexpr buffer_index_t BUFFER_INDEX_ZERO = {};

    private:
        /** @brief Pointer to pointer to the first element of the buffer. */
        pointer* ppData;
        /** @brief Pointer to the buffer flags. */
        Enum<BufferFlags> const* pFlags;
        /** @brief Pointer to the buffer size. */
        buffer_size_t const* pSize;
        /** @brief Pointer to the buffer strides. */
        buffer_size_t const* pStrides;
        /** @brief Current position of the iterator. */
        buffer_index_t indices;

    public:
        /**
         * @copydoc constructor
         *
         * @param ptr Pointer to the first element of the buffer.
         * @param flags Buffer flags.
         * @param size Buffer size.
         * @param strides Buffer strides.
         */
        BufferIterator(pointer& ptr, const Enum<BufferFlags>& flags, const buffer_size_t& size, const buffer_size_t& strides)
            : ppData(&ptr), pFlags(&flags), pSize(&size), pStrides(&strides), indices(BUFFER_INDEX_ZERO)
        {
        }

        /** Gets the element referenced by the iterator. */
        reference operator*()
        {
            return BufferIterator::Get<false>(*this->ppData, *this->pFlags, *this->pSize, *this->pStrides, this->indices);
        }

        /** Provides pointer-like access to the element referenced by the iterator. */
        pointer operator->()
        {
            return &this->operator*();
        }

        /**
         * Returns a new advanced iterator.
         *
         * @param i The value to add to the last dimension.
         */
        BufferIterator operator+(index_t i) const
        {
            BufferIterator result = *this;
            result += i;
            return result;
        }

        /**
         * Returns a new advanced iterator.
         *
         * @param rhs The value to add.
         */
        template<size_t NDim = NDimensions>
            requires (NDim == NDimensions)
        typename std::enable_if_t<(NDim > 1), BufferIterator> operator+(const buffer_index_t& rhs) const
        {
            BufferIterator result = *this;
            result += rhs;
            return result;
        }

        /**
         * Moves the last dimension of the iterator forward.
         *
         * @param i The value to add to the last dimension.
         */
        BufferIterator& operator+=(index_t i)
        {
            this->IncrementIndex(NDimensions - 1, i);

            return *this;
        }

        /**
         * Moves the iterator forward.
         *
         * @param rhs The value to add to the current index.
         */
        template<size_t NDim = NDimensions>
            requires (NDim == NDimensions)
        typename std::enable_if_t<(NDim > 1), BufferIterator&> operator+=(const buffer_index_t& rhs)
        {
            for (size_t i = 0; i < NDimensions; ++i)
            {
                this->IncrementIndex(i, rhs[i]);
            }

            return *this;
        }

        /**
         * Returns a new moved back iterator.
         *
         * @param i The value to subtract from the last dimension.
         */
        BufferIterator operator-(index_t i) const
        {
            BufferIterator result = *this;
            result -= i;
            return result;
        }

        /**
         * Returns a new moved back iterator.
         *
         * @param rhs The value to subtract.
         */
        template<size_t NDim = NDimensions>
            requires (NDim == NDimensions)
        typename std::enable_if_t<(NDim > 1), BufferIterator> operator-(const buffer_index_t& rhs) const
        {
            BufferIterator result = *this;
            result -= rhs;
            return result;
        }

        /**
         * Moves the last dimension of the iterator backwards.
         *
         * @param i The value to subtract from the last dimension.
         */
        BufferIterator& operator-=(index_t i)
        {
            this->DecrementIndex(NDimensions - 1, i);
            return *this;
        }

        /**
         * Moves the iterator backwards.
         *
         * @param rhs The value to subtract.
         */
        template<size_t NDim = NDimensions>
            requires (NDim == NDimensions)
        typename std::enable_if_t<(NDim > 1), BufferIterator&> operator-=(const buffer_index_t& rhs)
        {
            for (size_t i = 0; i < NDimensions; ++i)
            {
                this->DecrementIndex(i, rhs[i]);
            }

            return *this;
        }

        /** Moves the iterator forward by one. */
        BufferIterator& operator++()
        {
            if constexpr (NDimensions == 1) this->indices++;
            else this->IncrementIndex(NDimensions - 1);

            return *this;
        }

        /** @copydoc operator++ */
        BufferIterator& operator++(int)
        {
            BufferIterator temp = *this;
            this->operator++();
            return temp;
        }

        /** Moves the iterator backwards by one. */
        BufferIterator& operator--()
        {
            if constexpr (NDimensions == 1) this->indices--;
            else this->DecrementIndex(NDimensions - 1);

            return *this;
        }

        /** @copydoc operator-- */
        BufferIterator& operator--(int)
        {
            BufferIterator temp = *this;
            this->operator--();
            return temp;
        }

        /** Compares the current iterator with another for ordering based on indices. */
        auto operator<=>(const BufferIterator& rhs) const
        {
            if (this->indices == rhs.indices)
                return std::weak_ordering::equivalent;

            if constexpr (NDimensions == 1)
            {
                return this->indices > rhs.indices ? std::weak_ordering::greater : std::weak_ordering::less;
            }
            else
            {
                for (size_t i = 0; i < NDimensions; ++i)
                {
                    if (this->indices[i] > rhs.indices[i])
                        return std::weak_ordering::greater;
                    else if (this->indices[i] < rhs.indices[i])
                        return std::weak_ordering::less;
                }
            }

            return std::weak_ordering::less;
        }

        /** Checks whether both iterators belong to same buffer and at the same position. */
        bool operator==(const BufferIterator& rhs) const
        {
            return this->ppData == rhs.ppData && this->indices == rhs.indices;
        }

        /** Gets the current indices. */
        const buffer_index_t& Indices() const
        {
            return this->indices;
        }

        /**
         * Moves the provided dimension of the iterator forward.
         *
         * @param dim Dimension to move.
         * @param n The value to add.
         */
        void IncrementIndex(size_t dim, index_t n = 1)
        {
            if constexpr (NDimensions == 1) this->indices += n;
            else
            {
                this->indices[dim] += n;

                if (dim > 0)
                {
                    n = this->indices[dim] / (*this->pSize)[dim];
                    this->indices[dim] %= (*this->pSize)[dim];
                    this->IncrementIndex(dim - 1, n);
                }
            }
        }

        /**
         * Moves the provided dimension of the iterator backwards.
         *
         * @param dim Dimension to move.
         * @param n The value to subtract.
         */
        void DecrementIndex(size_t dim, index_t n = 1)
        {
            if constexpr (NDimensions == 1) this->indices -= n;
            else
            {
                this->indices[dim] -= n;

                if (this->indices[dim] < 0)
                {
                    n = ((-this->indices[dim]) / (*this->pSize)[dim]) + 1;

                    // positive mod
                    const index_t s = (*this->pSize)[dim];
                    this->indices[dim] = (this->indices[dim] % s + s) % s;

                    this->DecrementIndex(dim - 1, n);
                }
            }
        }

        /**
         * Gets a reference to the element at the provided indices.
         *
         * @tparam CheckErrors Determines whether to validate indices.
         * @tparam NDim Number of dimensions for SFINAE, must be equal to ``NDimensions``.
         * @param ptr Pointer to the first element of the buffer.
         * @param flags Buffer flags.
         * @param size Buffer size.
         * @param strides Buffer strides.
         */
        template<bool CheckErrors, size_t NDim = NDimensions>
            requires (NDim == NDimensions)
        static typename std::enable_if_t<(NDim > 1), reference> Get(pointer& ptr, const Enum<BufferFlags>& flags, const buffer_size_t& size, const buffer_size_t& strides, const auto... indices)
        {
            static_assert(sizeof...(indices) > 0 && sizeof...(indices) <= NDimensions, "Invalid number of indices parameters.");
            static_assert((std::is_convertible_v<decltype(indices), index_t> && ...), "Invalid type for indices parameters, must be convertible to index_t.");

            const buffer_index_t indicesArray = { std::forward<index_t>(static_cast<index_t>(indices))... };
            return BufferIterator::Get<CheckErrors>(ptr, flags, size, strides, indicesArray);
        }

        /**
         * Gets a reference to the element at the provided indices.
         *
         * @tparam CheckErrors Determines whether to validate indices.
         * @param ptr Pointer to the first element of the buffer.
         * @param flags Buffer flags.
         * @param size Buffer size.
         * @param strides Buffer strides.
         */
        template<bool CheckErrors>
        static reference Get(pointer& ptr, const Enum<BufferFlags>& flags, const buffer_size_t& size, const buffer_size_t& strides, const buffer_index_t& indices)
        {
            const bool isCircular = flags.Test(BufferFlags::Circular);

            if constexpr (NDimensions == 1)
            {
                if (isCircular)
                {
                    return ptr[indices % size];
                }
                else
                {
                    if constexpr (CheckErrors)
                    {
                        if (indices < 0 || indices >= size)
                        {
                            HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Index out of bounds.");
                        }
                    }
                    return ptr[indices];
                }
            }
            else
            {
                index_t n = 0;
                if (isCircular)
                {
                    for (size_t i = 0; i < NDimensions; ++i)
                    {
                        n += (indices[i] % size[i]) * strides[i];
                    }
                }
                else
                {
                    for (size_t i = 0; i < NDimensions; ++i)
                    {
                        if constexpr (CheckErrors)
                        {
                            if (indices[i] < 0 || indices[i] >= size[i])
                            {
                                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Index out of bounds.");
                            }
                        }
                        n += indices[i] * strides[i];
                    }
                }
                return ptr[n];
            }
        }
    };
}

#endif
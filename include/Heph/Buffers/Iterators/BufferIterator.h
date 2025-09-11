#ifndef HEPH_BUFFER_ITERATOR_H
#define HEPH_BUFFER_ITERATOR_H

#include "Heph/Utils.h"
#include "Heph/Buffers/Iterators/BufferIteratorConcept.h"
#include "Heph/Exceptions/InvalidArgumentException.h"
#include <functional>
#include <tuple>
#include <compare>
#include <numeric>

/** @file */

namespace Heph
{
    /**
     * @brief Default iterator for Heph::Buffer.
     *
     * @tparam TData Type of the elements stored in buffer.
     * @tparam NDimensions Number of dimensions the buffer has.
     */
    template<BufferElement TData, size_t NDimensions>
        requires (NDimensions > 0)
    class HEPH_API BufferIterator final
    {
    public:
        /** @copybrief BufferIteratorTraits */
        using Traits = BufferIteratorTraits<TData, NDimensions>;

        /** @brief Difference type for std::iterator_traits. */
        using difference_type = typename Traits::difference_type;
        /** @brief Value type for std::iterator_traits. */
        using value_type = typename Traits::value_type;
        /** @brief Pointer type for std::iterator_traits. */
        using pointer = typename Traits::pointer;
        /** @brief Reference type for std::iterator_traits. */
        using reference = typename Traits::reference;
        /** @brief Iterator tag for std::iterator_traits. */
        using iterator_category = typename Traits::iterator_category;
        /** @brief ``size_t`` for 1D buffers, an array of ``size_t`` for multidimensional buffers. */
        using buffer_size_t = typename Traits::buffer_size_t;
        /** @brief ``index_t`` for 1D buffers, an array of ``index_t`` for multidimensional buffers. */
        using buffer_index_t = typename Traits::buffer_index_t;

        /** @brief ``buffer_size_t`` instance with ``0`` for all dimensions. */
        static constexpr buffer_size_t BUFFER_SIZE_ZERO = Traits::BUFFER_SIZE_ZERO;
        /** @brief ``buffer_index_t`` instance with ``0`` for all dimensions. */
        static constexpr buffer_index_t BUFFER_INDEX_ZERO = Traits::BUFFER_INDEX_ZERO;

    private:
        /** @brief Pointer to the first element of the buffer. */
        pointer pData;
        /** @brief Pointer to the buffer size. */
        const buffer_size_t* pSize;
        /** @brief Pointer to the buffer strides. */
        const buffer_size_t* pStrides;
        /** @brief Current position of the iterator. */
        buffer_index_t indices;

    public:
        /**
         * @copydoc constructor
         *
         * @param ptr Pointer to the first element of the buffer.
         * @param size Buffer size.
         * @param strides Buffer strides.
         * @param indices Buffer indices.
         */
        constexpr BufferIterator(pointer ptr, const buffer_size_t& size, const buffer_size_t& strides, const buffer_index_t& indices)
            : pData(ptr), pSize(&size), pStrides(&strides), indices(indices)
        {
        }

        /** Gets the element referenced by the iterator. */
        constexpr HEPH_FORCE_INLINE reference operator*()
        {
            return BufferIterator::Get<false>(this->pData, *this->pSize, *this->pStrides, this->indices);
        }

        /** Provides pointer-like access to the element referenced by the iterator. */
        constexpr HEPH_FORCE_INLINE pointer operator->()
        {
            return &this->operator*();
        }

        /**
         * Returns a new advanced iterator.
         *
         * @param i The value to add to the last dimension.
         */
        constexpr HEPH_FORCE_INLINE BufferIterator operator+(index_t i) const
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
        constexpr HEPH_FORCE_INLINE BufferIterator operator+(const buffer_index_t& rhs) const
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
        constexpr HEPH_FORCE_INLINE BufferIterator& operator+=(index_t i)
        {
            this->IncrementIndex(NDimensions - 1, i);
            return *this;
        }

        /**
         * Moves the iterator forward.
         *
         * @param rhs The value to add to the current index.
         */
        constexpr HEPH_FORCE_INLINE BufferIterator& operator+=(const buffer_index_t& rhs)
        {
            for (size_t i = 0; i < NDimensions; ++i)
                this->IncrementIndex(i, rhs[i]);
            return *this;
        }

        /**
         * Returns a new moved back iterator.
         *
         * @param i The value to subtract from the last dimension.
         */
        constexpr HEPH_FORCE_INLINE BufferIterator operator-(index_t i) const
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
        constexpr HEPH_FORCE_INLINE BufferIterator operator-(const buffer_index_t& rhs) const
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
        constexpr HEPH_FORCE_INLINE BufferIterator& operator-=(index_t i)
        {
            this->DecrementIndex(NDimensions - 1, i);
            return *this;
        }

        /**
         * Moves the iterator backwards.
         *
         * @param rhs The value to subtract.
         */
        constexpr HEPH_FORCE_INLINE BufferIterator& operator-=(const buffer_index_t& rhs)
        {
            for (size_t i = 0; i < NDimensions; ++i)
                this->DecrementIndex(i, rhs[i]);
            return *this;
        }

        /** Moves the iterator forward by one. */
        constexpr HEPH_FORCE_INLINE BufferIterator& operator++()
        {
            this->IncrementIndex(NDimensions - 1);
            return *this;
        }

        /** @copydoc operator++ */
        constexpr HEPH_FORCE_INLINE BufferIterator& operator++(int)
        {
            BufferIterator temp = *this;
            this->operator++();
            return temp;
        }

        /** Moves the iterator backwards by one. */
        constexpr HEPH_FORCE_INLINE BufferIterator& operator--()
        {
            this->DecrementIndex(NDimensions - 1);
            return *this;
        }

        /** @copydoc operator-- */
        constexpr HEPH_FORCE_INLINE BufferIterator& operator--(int)
        {
            BufferIterator temp = *this;
            this->operator--();
            return temp;
        }

        /** Checks whether both iterators belong to same buffer and at the same position. */
        constexpr HEPH_FORCE_INLINE bool operator==(const BufferIterator& rhs) const
        {
            return this->pData == rhs.pData && this->indices == rhs.indices;
        }

        /** Gets the current indices. */
        constexpr HEPH_FORCE_INLINE const buffer_index_t& Indices() const
        {
            return this->indices;
        }

        /**
         * Moves the provided dimension of the iterator forward.
         *
         * @param dim Dimension to move.
         * @param n The value to add.
         */
        constexpr void IncrementIndex(size_t dim, index_t n = 1)
        {
            this->indices[dim] += n;

            if (dim > 0)
            {
                n = this->indices[dim] / (*this->pSize)[dim];
                this->indices[dim] %= (*this->pSize)[dim];
                this->IncrementIndex(dim - 1, n);
            }
        }

        /**
         * Moves the provided dimension of the iterator backwards.
         *
         * @param dim Dimension to move.
         * @param n The value to subtract.
         */
        constexpr void DecrementIndex(size_t dim, index_t n = 1)
        {
            this->indices[dim] -= n;

            if (this->indices[dim] < 0)
            {
                n = ((-this->indices[dim]) / (*this->pSize)[dim]) + 1;

                // positive mod
                const index_t s = (*this->pSize)[dim];
                this->indices[dim] = (this->indices[dim] % s + s) % s;

                if (dim > 0)
                    this->DecrementIndex(dim - 1, n);
            }
        }

        /**
         * Gets a reference to the element at the provided indices.
         *
         * @tparam CheckErrors Determines whether to validate indices.
         * @param ptr Pointer to the first element of the buffer.
         * @param size Buffer size.
         * @param strides Buffer strides.
         */
        template<bool CheckErrors>
        static constexpr HEPH_FORCE_INLINE reference Get(pointer ptr, const buffer_size_t& size, const buffer_size_t& strides, const auto... indices)
        {
            static_assert(sizeof...(indices) > 0 && sizeof...(indices) <= NDimensions, "Invalid number of indices parameters.");
            static_assert((std::is_convertible_v<decltype(indices), index_t> && ...), "Invalid type for indices parameters, must be convertible to index_t.");

            const buffer_index_t indicesArray = { std::forward<index_t>(static_cast<index_t>(indices))... };
            return BufferIterator::Get<CheckErrors>(ptr, size, strides, indicesArray);
        }

        /**
         * Gets a reference to the element at the provided indices.
         *
         * @tparam CheckErrors Determines whether to validate indices.
         * @param ptr Pointer to the first element of the buffer.
         * @param size Buffer size.
         * @param strides Buffer strides.
         */
        template<bool CheckErrors>
        static constexpr HEPH_FORCE_INLINE reference Get(pointer ptr, const buffer_size_t& size, const buffer_size_t& strides, const buffer_index_t& indices)
        {
            if constexpr (CheckErrors)
            {
                for (size_t i = 0; i < NDimensions; ++i)
                {
                    if (indices[i] < 0 || indices[i] >= size[i])
                    {
                        HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Index out of bounds.");
                    }
                }
            }

            const index_t n = std::inner_product(indices.begin(), indices.end(), strides.begin(), 0);
            return ptr[n];
        }
    };

    /**
     * @brief Default iterator for Heph::Buffer.
     *
     * @tparam TData Type of the elements stored in buffer.
     */
    template<BufferElement TData>
    class HEPH_API BufferIterator<TData, 1> final
    {
    public:
        /** @copybrief BufferIteratorTraits */
        using Traits = BufferIteratorTraits<TData, 1>;

        /** @brief Difference type for std::iterator_traits. */
        using difference_type = typename Traits::difference_type;
        /** @brief Value type for std::iterator_traits. */
        using value_type = typename Traits::value_type;
        /** @brief Pointer type for std::iterator_traits. */
        using pointer = typename Traits::pointer;
        /** @brief Reference type for std::iterator_traits. */
        using reference = typename Traits::reference;
        /** @brief Iterator tag for std::iterator_traits. */
        using iterator_category = typename Traits::iterator_category;
        /** @brief ``size_t`` for 1D buffers, an array of ``size_t`` for multidimensional buffers. */
        using buffer_size_t = typename Traits::buffer_size_t;
        /** @brief ``index_t`` for 1D buffers, an array of ``index_t`` for multidimensional buffers. */
        using buffer_index_t = typename Traits::buffer_index_t;

        /** @brief ``buffer_size_t`` instance with ``0`` for all dimensions. */
        static constexpr buffer_size_t BUFFER_SIZE_ZERO = Traits::BUFFER_SIZE_ZERO;
        /** @brief ``buffer_index_t`` instance with ``0`` for all dimensions. */
        static constexpr buffer_index_t BUFFER_INDEX_ZERO = Traits::BUFFER_INDEX_ZERO;

    private:
        /** @brief Pointer to the current element. */
        pointer pData;

    public:
        /**
             * @copydoc constructor
             *
             * @param ptr Pointer to the first element of the buffer.
             * @param size Buffer size.
             * @param strides Buffer strides.
             * @param indices Buffer indices.
             */
        constexpr BufferIterator(pointer ptr, const buffer_size_t& size, const buffer_size_t& strides, const buffer_index_t& indices)
            : pData(ptr + indices)
        {
        }

        /** Gets the element referenced by the iterator. */
        constexpr HEPH_FORCE_INLINE reference operator*()
        {
            return *pData;
        }

        /** Provides pointer-like access to the element referenced by the iterator. */
        constexpr HEPH_FORCE_INLINE pointer operator->()
        {
            return &this->operator*();
        }

        /**
         * Returns a new advanced iterator.
         *
         * @param i The value to add to the last dimension.
         */
        constexpr HEPH_FORCE_INLINE BufferIterator operator+(index_t i) const
        {
            BufferIterator result = *this;
            result += i;
            return result;
        }

        /**
         * Moves the last dimension of the iterator forward.
         *
         * @param i The value to add to the last dimension.
         */
        constexpr HEPH_FORCE_INLINE BufferIterator& operator+=(index_t i)
        {
            this->pData += i;
            return *this;
        }

        /**
         * Returns a new moved back iterator.
         *
         * @param i The value to subtract from the last dimension.
         */
        constexpr HEPH_FORCE_INLINE BufferIterator operator-(index_t i) const
        {
            BufferIterator result = *this;
            result -= i;
            return result;
        }

        /**
         * Moves the last dimension of the iterator backwards.
         *
         * @param i The value to subtract from the last dimension.
         */
        constexpr HEPH_FORCE_INLINE BufferIterator& operator-=(index_t i)
        {
            this->pData -= i;
            return *this;
        }

        /** Moves the iterator forward by one. */
        constexpr HEPH_FORCE_INLINE BufferIterator& operator++()
        {
            this->pData++;
            return *this;
        }

        /** @copydoc operator++ */
        constexpr HEPH_FORCE_INLINE BufferIterator& operator++(int)
        {
            BufferIterator temp = *this;
            this->operator++();
            return temp;
        }

        /** Moves the iterator backwards by one. */
        constexpr HEPH_FORCE_INLINE BufferIterator& operator--()
        {
            this->pData--;
            return *this;
        }

        /** @copydoc operator-- */
        constexpr HEPH_FORCE_INLINE BufferIterator& operator--(int)
        {
            BufferIterator temp = *this;
            this->operator--();
            return temp;
        }

        /** Checks whether both iterators belong to same buffer and at the same position. */
        constexpr HEPH_FORCE_INLINE bool operator==(const BufferIterator& rhs) const
        {
            return this->pData == rhs.pData;
        }

        /** Gets the current indices. */
        constexpr HEPH_FORCE_INLINE const buffer_index_t& Indices() const
        {
            static const buffer_index_t bi = {};
            return bi;
        }

        /**
         * Moves the provided dimension of the iterator forward.
         *
         * @param dim Dimension to move.
         * @param n The value to add.
         */
        constexpr HEPH_FORCE_INLINE void IncrementIndex(size_t dim, index_t n = 1)
        {
            this->pData += n;
        }

        /**
         * Moves the provided dimension of the iterator backwards.
         *
         * @param dim Dimension to move.
         * @param n The value to subtract.
         */
        constexpr HEPH_FORCE_INLINE void DecrementIndex(size_t dim, index_t n = 1)
        {
            this->pData -= n;
        }

        /**
         * Gets a reference to the element at the provided indices.
         *
         * @tparam CheckErrors Determines whether to validate indices.
         * @param ptr Pointer to the first element of the buffer.
         * @param size Buffer size.
         * @param strides Buffer strides.
         */
        template<bool CheckErrors>
        static constexpr HEPH_FORCE_INLINE reference Get(pointer ptr, const buffer_size_t& size, const buffer_size_t& strides, const buffer_index_t& indices)
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
    };
}

#endif
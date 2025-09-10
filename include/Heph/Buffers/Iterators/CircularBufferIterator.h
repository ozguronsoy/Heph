#ifndef CIRCULAR_BUFFER_ITERATOR_H
#define CIRCULAR_BUFFER_ITERATOR_H

#include "Heph/Utils.h"
#include "Heph/Buffers/Iterators/BufferIteratorConcept.h"
#include "Heph/Exceptions/InvalidArgumentException.h"

/** @file */

namespace Heph
{
    /**
     * @brief Circular iterator for Heph::Buffer.
     *
     * @tparam TData Type of the elements stored in buffer.
     * @tparam NDimensions Number of dimensions the buffer has.
     */
    template<BufferElement TData, size_t NDimensions>
        requires (NDimensions > 0)
    class HEPH_API CircularBufferIterator final
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
        CircularBufferIterator(pointer ptr, const buffer_size_t& size, const buffer_size_t& strides, const buffer_index_t& indices)
            : pData(ptr), pSize(&size), pStrides(&strides), indices(indices)
        {
        }

        /** Gets the element referenced by the iterator. */
        reference operator*()
        {
            return CircularBufferIterator::Get<false>(this->pData, *this->pSize, *this->pStrides, this->indices);
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
        CircularBufferIterator operator+(index_t i) const
        {
            CircularBufferIterator result = *this;
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
        typename std::enable_if_t<(NDim > 1), CircularBufferIterator> operator+(const buffer_index_t& rhs) const
        {
            CircularBufferIterator result = *this;
            result += rhs;
            return result;
        }

        /**
         * Moves the last dimension of the iterator forward.
         *
         * @param i The value to add to the last dimension.
         */
        CircularBufferIterator& operator+=(index_t i)
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
        typename std::enable_if_t<(NDim > 1), CircularBufferIterator&> operator+=(const buffer_index_t& rhs)
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
        CircularBufferIterator operator-(index_t i) const
        {
            CircularBufferIterator result = *this;
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
        typename std::enable_if_t<(NDim > 1), CircularBufferIterator> operator-(const buffer_index_t& rhs) const
        {
            CircularBufferIterator result = *this;
            result -= rhs;
            return result;
        }

        /**
         * Moves the last dimension of the iterator backwards.
         *
         * @param i The value to subtract from the last dimension.
         */
        CircularBufferIterator& operator-=(index_t i)
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
        typename std::enable_if_t<(NDim > 1), CircularBufferIterator&> operator-=(const buffer_index_t& rhs)
        {
            for (size_t i = 0; i < NDimensions; ++i)
            {
                this->DecrementIndex(i, rhs[i]);
            }

            return *this;
        }

        /** Moves the iterator forward by one. */
        CircularBufferIterator& operator++()
        {
            if constexpr (NDimensions == 1) this->indices++;
            else this->IncrementIndex(NDimensions - 1);

            return *this;
        }

        /** @copydoc operator++ */
        CircularBufferIterator& operator++(int)
        {
            CircularBufferIterator temp = *this;
            this->operator++();
            return temp;
        }

        /** Moves the iterator backwards by one. */
        CircularBufferIterator& operator--()
        {
            if constexpr (NDimensions == 1) this->indices--;
            else this->DecrementIndex(NDimensions - 1);

            return *this;
        }

        /** @copydoc operator-- */
        CircularBufferIterator& operator--(int)
        {
            CircularBufferIterator temp = *this;
            this->operator--();
            return temp;
        }

        /** Checks whether both iterators belong to same buffer and at the same position. */
        bool operator==(const CircularBufferIterator& rhs) const
        {
            return this->pData == rhs.pData && this->indices == rhs.indices;
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

                    if (dim > 0)
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
         * @param size Buffer size.
         * @param strides Buffer strides.
         */
        template<bool CheckErrors, size_t NDim = NDimensions>
            requires (NDim == NDimensions)
        static typename std::enable_if_t<(NDim > 1), reference> Get(pointer const& ptr, const buffer_size_t& size, const buffer_size_t& strides, const auto... indices)
        {
            static_assert(sizeof...(indices) > 0 && sizeof...(indices) <= NDimensions, "Invalid number of indices parameters.");
            static_assert((std::is_convertible_v<decltype(indices), index_t> && ...), "Invalid type for indices parameters, must be convertible to index_t.");

            const buffer_index_t indicesArray = { std::forward<index_t>(static_cast<index_t>(indices))... };
            return CircularBufferIterator::Get<CheckErrors>(ptr, size, strides, indicesArray);
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
        static reference Get(pointer ptr, const buffer_size_t& size, const buffer_size_t& strides, const buffer_index_t& indices)
        {
            if constexpr (NDimensions == 1)
            {
                return ptr[indices % size];
            }
            else
            {
                index_t n = 0;
                for (size_t i = 0; i < NDimensions; ++i)
                    n += (indices[i] % size[i]) * strides[i];
                return ptr[n];
            }
        }
    };
}

#endif
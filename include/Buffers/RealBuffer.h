#ifndef HEPH_REAL_BUFFER_H
#define HEPH_REAL_BUFFER_H

#include "HephShared.h"
#include "ArithmeticBuffer.h"

/** @file */

namespace Heph
{
    /**
     * @brief Buffer for storing real numbers.
     *
     * @tparam TData Type of the elements stored in buffer. Must be default constructible and trivally destructible.
     * @tparam NDimensions Number of dimensions.
     */
    template <BufferElement TData, size_t NDimensions = 1>
        requires std::is_integral_v<TData> || std::is_floating_point_v<TData>
    class HEPH_API RealBuffer final : public ArithmeticBuffer<TData, NDimensions>
    {
    public:
        /** @brief Base class. */
        using Buffer = Buffer<TData, NDimensions>;

        /** @copybrief Buffer::iterator */
        using typename Buffer::iterator;
        /** @copybrief Buffer::const_iterator */
        using typename Buffer::const_iterator;
        /** @copybrief Buffer::reverse_iterator */
        using typename Buffer::reverse_iterator;
        /** @copybrief Buffer::const_reverse_iterator */
        using typename Buffer::const_reverse_iterator;
        /** @copybrief Buffer::buffer_size_t */
        using typename Buffer::buffer_size_t;
        /** @copybrief Buffer::buffer_index_t */
        using typename Buffer::buffer_index_t;
        /** @copybrief Buffer::InitializerList */
        using typename Buffer::InitializerList;

        /** @copybrief Buffer::BUFFER_SIZE_ZERO */
        using Buffer::BUFFER_SIZE_ZERO;
        /** @copybrief Buffer::BUFFER_INDEX_ZERO */
        using Buffer::BUFFER_INDEX_ZERO;

    protected:
        /** @copybrief ArithmeticBuffer::MIN_ELEMENT */
        using ArithmeticBuffer<TData, NDimensions>::MIN_ELEMENT;
        /** @copybrief ArithmeticBuffer::MAX_ELEMENT */
        using ArithmeticBuffer<TData, NDimensions>::MAX_ELEMENT;

    public:
        /** @copydoc Buffer::Buffer */
        explicit RealBuffer(Enum<BufferFlags> flags = BufferFlags::None, auto... size) : ArithmeticBuffer<TData, NDimensions>(flags, std::forward<decltype(size)>(size)...) {}
        /** @copydoc Buffer::Buffer */
        explicit RealBuffer(Enum<BufferFlags> flags, const buffer_size_t& size) : ArithmeticBuffer<TData, NDimensions>(flags, size) {}
        /** @copydoc Buffer::Buffer(const InitializerList&) */
        RealBuffer(const InitializerList& rhs) : ArithmeticBuffer<TData, NDimensions>(rhs) {}
        /** @copydoc Buffer::Buffer(const Buffer&) */
        RealBuffer(const RealBuffer& rhs) = default;
        /** @copydoc Buffer::Buffer(Buffer&&) */
        RealBuffer(RealBuffer&& rhs) noexcept = default;
        /** @copydoc Buffer::operator=(const Buffer&) */
        RealBuffer& operator=(const RealBuffer& rhs) = default;
        /** @copydoc Buffer::operator=(Buffer&&) */
        RealBuffer& operator=(RealBuffer&& rhs) noexcept = default;

        /** @copydoc destructor */
        ~RealBuffer() = default;

        /**
         * Creates a copy and adds constant value to its all elements.
         *
         * @tparam TRhs Type of the rhs.
         * @param rhs Constant to add.
         * @return New instance.
         */
        template<BufferElement TRhs>
            requires Addable<TData, TRhs>
        RealBuffer operator+(const TRhs& rhs) const
        {
            RealBuffer result = *this;
            result += rhs;
            return result;
        }

        /**
         * Creates a copy and subtracts constant value from all elements.
         *
         * @tparam TRhs Type of the rhs.
         * @param rhs Constant to subtract.
         * @return New instance.
         */
        template<BufferElement TRhs>
            requires Subtractable<TData, TRhs>
        RealBuffer operator-(const TRhs& rhs) const
        {
            RealBuffer result = *this;
            result -= rhs;
            return result;
        }

        /**
         * Creates a copy and multiplies all elements with a constant.
         *
         * @tparam TRhs Type of the rhs.
         * @param rhs Multiplication factor.
         * @return New instance.
         */
        template<BufferElement TRhs>
            requires Multipliable<TData, TRhs>
        RealBuffer operator*(const TRhs& rhs) const
        {
            RealBuffer result = *this;
            result *= rhs;
            return result;
        }

        /**
         * Creates a copy and divides all elements by a constant.
         *
         * @tparam TRhs Type of the rhs.
         * @param rhs Division factor.
         * @return New instance.
         */
        template<BufferElement TRhs>
            requires Divisible<TData, TRhs>
        RealBuffer operator/(const TRhs& rhs) const
        {
            RealBuffer result = *this;
            result /= rhs;
            return result;
        }

        /**
         * Creates a copy and performs element-wise addition.
         *
         * @tparam TRhs Type of the rhs elements.
         * @param rhs Right operand.
         * @return New instance.
         * @exception InvalidOperationException
         */
        template<BufferElement TRhsData>
            requires Addable<TData, TRhsData>
        RealBuffer operator+(const ArithmeticBuffer<TRhsData, NDimensions>& rhs) const
        {
            RealBuffer result = *this;
            result += rhs;
            return result;
        }

        /**
         * Creates a copy and performs element-wise subtraction.
         *
         * @tparam TRhs Type of the rhs elements.
         * @param rhs Right operand.
         * @return New instance.
         * @exception InvalidOperationException
         */
        template<BufferElement TRhsData>
            requires Subtractable<TData, TRhsData>
        RealBuffer operator-(const ArithmeticBuffer<TRhsData, NDimensions>& rhs) const
        {
            RealBuffer result = *this;
            result -= rhs;
            return result;
        }

        /**
         * Creates a copy and performs element-wise multiplication.
         *
         * @tparam TRhs Type of the rhs elements.
         * @param rhs Right operand.
         * @return New instance.
         * @exception InvalidOperationException
         */
        template<BufferElement TRhsData>
            requires Multipliable<TData, TRhsData>
        RealBuffer operator*(const ArithmeticBuffer<TRhsData, NDimensions>& rhs) const
        {
            RealBuffer result = *this;
            result *= rhs;
            return result;
        }

        /**
         * Creates a copy and performs element-wise division.
         *
         * @tparam TRhs Type of the rhs elements.
         * @param rhs Right operand.
         * @return New instance.
         * @exception InvalidOperationException
         */
        template<BufferElement TRhsData>
            requires Divisible<TData, TRhsData>
        RealBuffer operator/(const ArithmeticBuffer<TRhsData, NDimensions>& rhs) const
        {
            RealBuffer result = *this;
            result /= rhs;
            return result;
        }

        /**
         * Creates a copy and adds constant value to its all elements.
         *
         * @tparam TLhs Type of the lhs.
         * @param lhs Left operand.
         * @param rhs Right operand.
         * @return New instance.
         */
        template<BufferElement TLhs>
            requires Addable<TLhs, TData, TData>
        friend RealBuffer operator+(const TLhs& lhs, const RealBuffer& rhs)
        {
            return rhs + lhs;
        }

        /**
         * Creates a copy and subtracts constant value from all elements.
         *
         * @tparam TLhs Type of the lhs.
         * @param lhs Left operand.
         * @param rhs Right operand.
         * @return New instance.
         */
        template<BufferElement TLhs>
            requires Subtractable<TLhs, TData, TData>
        friend RealBuffer operator-(const TLhs& lhs, const RealBuffer& rhs)
        {
            RealBuffer result(BufferFlags::AllocUninitialized, rhs.size);

            iterator itResult = result.begin();
            for (const TData& rhsElement : rhs) *(itResult++) = lhs - rhsElement;

            result.flags = rhs.flags;
            return result;
        }

        /**
         * Creates a copy and multiplies all elements with a constant.
         *
         * @tparam TLhs Type of the lhs.
         * @param lhs Left operand.
         * @param rhs Right operand.
         * @return New instance.
         */
        template<BufferElement TLhs>
            requires Multipliable<TLhs, TData, TData>
        friend RealBuffer operator*(const TLhs& lhs, const RealBuffer& rhs)
        {
            return rhs * lhs;
        }

        /**
         * Creates a copy and divides all elements by a constant.
         *
         * @tparam TLhs Type of the lhs.
         * @param lhs Left operand.
         * @param rhs Right operand.
         * @return New instance.
         */
        template<BufferElement TLhs>
            requires Divisible<TLhs, TData, TData>
        friend RealBuffer operator/(const TLhs& lhs, const RealBuffer& rhs)
        {
            RealBuffer result(BufferFlags::AllocUninitialized, rhs.size);

            iterator itResult = result.begin();
            for (const TData& rhsElement : rhs) *(itResult++) = lhs / rhsElement;

            result.flags = rhs.flags;
            return result;
        }

        /** @copydoc ArithmeticBuffer::Invert */
        RealBuffer operator-() const
        {
            RealBuffer result = *this;
            result.Invert();
            return result;
        }

        /** @copydoc operator<<= */
        RealBuffer operator<<(size_t n) const
        {
            RealBuffer result = *this;
            result <<= n;
            return result;
        }

        /** @copydoc operator>>= */
        RealBuffer operator>>(size_t n) const
        {
            RealBuffer result = *this;
            result >>= n;
            return result;
        }

        /**
         * Shiftes the top-level entries to the left by provided amount.
         *
         * @note This method does not change the size of the buffer,
         * thus the entries that are shifted out of bounds will be lost.
         *
         * @param n Number of top-level entries to shift.
         * @exception InvalidOperationException
         */
        RealBuffer& operator<<=(size_t n)
        {
            Buffer::ShiftLeft(*this, n);
            return *this;
        }

        /**
         * Shiftes the top-level entries to the right by provided amount.
         *
         * @note This method does not change the size of the buffer,
         * thus the entries that are shifted out of bounds will be lost.
         *
         * @param n Number of top-level entries to shift.
         * @exception InvalidOperationException
         */
        RealBuffer& operator>>=(size_t n)
        {
            Buffer::ShiftRight(*this, n);
            return *this;
        }

        /**
         * Copies a section of the buffer.
         *
         * @note This method allocates memory for the destination buffer, hence no need to allocate in advance.
         *
         * @param index Index of the first top-level entry that will be copied.
         * @param size Number of top-level entries to copy.
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        RealBuffer SubBuffer(size_t index, size_t size)
        {
            RealBuffer result;
            Buffer::SubBuffer(*this, result, index, size);
            return result;
        }

        /**
         * Prepends the elements of the source buffer to the destination buffer.
         *
         * @param src The buffer whose elements will be prepended to the destination.
         * @exception InvalidOperationException
         * @exception InsufficientMemoryException
         */
        void Prepend(const RealBuffer& src)
        {
            Buffer::Prepend(*this, src);
        }

        /**
         * Appends the elements of the source buffer to the destination buffer.
         *
         * @note For multidimensional buffers, all dimensions except the first must have the same size in both buffers.
         *
         * @param src The buffer whose elements will be appended to the destination.
         * @exception InvalidOperationException
         * @exception InsufficientMemoryException
         */
        void Append(const RealBuffer& src)
        {
            Buffer::Append(*this, src);
        }

        /**
         * Inserts the elements of the source buffer to the destination buffer.
         *
         * @note For multidimensional buffers, all dimensions except the first must have the same size in both buffers.
         *
         * @param src The buffer whose elements will be inserted to the destination.
         * @exception InvalidArgumentException
         * @exception InvalidOperationException
         * @exception InsufficientMemoryException
         */
        void Insert(const RealBuffer& src, size_t index)
        {
            Buffer::Insert(*this, src, index);
        }

        /**
         * Removes a section of the buffer.
         *
         * @param index Index of the first top-level entry that will be removed.
         * @param size Number of top-level entries to remove.
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        void Cut(size_t index, size_t size)
        {
            Buffer::Cut(*this, index, size);
        }

        /**
         * Replaces a section of the buffer.
         *
         * @note This method does not allocate extra memory, just replaces the existing elements.
         *
         * @param src Buffer providing the replacement elements.
         * @param index Starting index in current buffer where replacement begins.
         * @param srcIndex Starting index in ``src`` from which top-level entries are copied.
         * @param size Number of top-level entries to replace.
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        void Replace(const RealBuffer& src, size_t index, size_t srcIndex, size_t size)
        {
            Buffer::Replace(*this, src, index, srcIndex, size);
        }

        /** @copydoc RealBuffer::Transpose */
        void Transpose(auto... perm)
        {
            static_assert(sizeof...(perm) == NDimensions, "Invalid number of perm parameters.");
            static_assert((std::is_convertible_v<decltype(perm), size_t> && ...), "Invalid type for perm parameters, must be convertible to size_t.");

            const buffer_size_t permArray = { std::forward<size_t>(static_cast<size_t>(perm))... };
            this->Transpose(permArray);
        }

        /**
         * Transposes a multidimensional buffer.
         *
         * @note This method allocates memory for the destination buffer, hence no need to allocate in advance.
         *
         * @param perm Permutation of the dimensions.
         * @exception InvalidOperationException
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        void Transpose(const buffer_size_t& perm)
        {
            Buffer::Transpose(*this, *this, perm);
        }

        /**
         * Changes the size of the buffer.<br>
         * If the new size is less than the old, elements at the end will be removed.
         *
         * @param newSize New size of the buffer.
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        void Resize(auto... newSize)
        {
            static_assert(sizeof...(newSize) > 0 && sizeof...(newSize) <= NDimensions, "Invalid number of newSize parametrs");
            static_assert((std::is_convertible_v<decltype(newSize), size_t> && ...), "Invalid type for newSize parameters, must be convertible to size_t.");

            const buffer_size_t ns = { std::forward<size_t>(static_cast<size_t>(newSize))... };
            this->Resize(ns);
        }

        /** @copydoc Resize */
        void Resize(const buffer_size_t& newSize)
        {
            Buffer::Resize(*this, newSize);
        }

        /**
         * Swaps the elements of the provided dimension.
         *
         * @param dim 0-based dimension that will be reversed.
         * @exception InvalidArgumentException
         */
        void Reverse(size_t dim = 0)
        {
            Buffer::Reverse(*this, dim);
        }
    };

    using DoubleBuffer = RealBuffer<double, 1>;
    extern template class RealBuffer<double, 1>;
}

#endif
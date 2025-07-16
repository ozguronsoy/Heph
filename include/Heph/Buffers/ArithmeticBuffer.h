#ifndef HEPH_ARITHMETIC_BUFFER_H
#define HEPH_ARITHMETIC_BUFFER_H

#include "Heph/Utils.h"
#include "Heph//Buffers/Buffer.h"
#include "Heph/Concepts.h"
#include <complex>
#include <cmath>

/** @file */

namespace Heph
{
    /**
     * @brief Base class for buffers that store arithmetic types. Provides arithmetic operations.
     *
     * @tparam TData Type of the elements stored in buffer.
     * @tparam NDimensions Number of dimensions.
     */
    template <BufferElement TData, size_t NDimensions = 1>
        requires Arithmetic<TData, TData>&& ArithmeticAssignable<TData, TData>
    class HEPH_API ArithmeticBuffer : public Buffer<TData, NDimensions>
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
        /** @brief Element with the minimum value. */
        static constexpr TData MIN_ELEMENT = std::numeric_limits<TData>::lowest();
        /** @brief Element with the maximum value. */
        static constexpr TData MAX_ELEMENT = std::numeric_limits<TData>::max();

    public:
        /** @copydoc Buffer::Buffer */
        explicit ArithmeticBuffer(Enum<BufferFlags> flags = BufferFlags::None, auto... size) : Buffer(flags, std::forward<decltype(size)>(size)...) {}
        /** @copydoc Buffer::Buffer */
        explicit ArithmeticBuffer(Enum<BufferFlags> flags, const buffer_size_t& size) : Buffer(flags, size) {}
        /** @copydoc Buffer::Buffer(const InitializerList&) */
        ArithmeticBuffer(const InitializerList& rhs) : Buffer(rhs) {}
        /** @copydoc Buffer::Buffer(const Buffer&) */
        ArithmeticBuffer(const ArithmeticBuffer& rhs) = default;
        /** @copydoc Buffer::Buffer(Buffer&&) */
        ArithmeticBuffer(ArithmeticBuffer&& rhs) noexcept = default;
        /** @copydoc Buffer::operator=(const Buffer&) */
        ArithmeticBuffer& operator=(const ArithmeticBuffer& rhs) = default;
        /** @copydoc Buffer::operator=(Buffer&&) */
        ArithmeticBuffer& operator=(ArithmeticBuffer&& rhs) noexcept = default;

        /** @copydoc destructor */
        virtual ~ArithmeticBuffer() = default;


        /** @copydoc ArithmeticBuffer::Invert */
        ArithmeticBuffer operator-() const
        {
            ArithmeticBuffer result = *this;
            result.Invert();
            return result;
        }

        /** @copydoc operator<<= */
        ArithmeticBuffer operator<<(size_t n) const
        {
            ArithmeticBuffer result = *this;
            result <<= n;
            return result;
        }

        /** @copydoc operator>>= */
        ArithmeticBuffer operator>>(size_t n) const
        {
            ArithmeticBuffer result = *this;
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
        ArithmeticBuffer& operator<<=(size_t n)
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
        ArithmeticBuffer& operator>>=(size_t n)
        {
            Buffer::ShiftRight(*this, n);
            return *this;
        }

        /**
 * Creates a copy and adds constant value to its all elements.
 *
 * @tparam TRhs Type of the rhs.
 * @param rhs Constant to add.
 * @return New instance.
 */
        template<BufferElement TRhs>
            requires Addable<TData, TRhs>
        ArithmeticBuffer operator+(const TRhs& rhs) const
        {
            ArithmeticBuffer result = *this;
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
        ArithmeticBuffer operator-(const TRhs& rhs) const
        {
            ArithmeticBuffer result = *this;
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
        ArithmeticBuffer operator*(const TRhs& rhs) const
        {
            ArithmeticBuffer result = *this;
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
        ArithmeticBuffer operator/(const TRhs& rhs) const
        {
            ArithmeticBuffer result = *this;
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
        ArithmeticBuffer operator+(const ArithmeticBuffer<TRhsData, NDimensions>& rhs) const
        {
            ArithmeticBuffer result = *this;
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
        ArithmeticBuffer operator-(const ArithmeticBuffer<TRhsData, NDimensions>& rhs) const
        {
            ArithmeticBuffer result = *this;
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
        ArithmeticBuffer operator*(const ArithmeticBuffer<TRhsData, NDimensions>& rhs) const
        {
            ArithmeticBuffer result = *this;
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
        ArithmeticBuffer operator/(const ArithmeticBuffer<TRhsData, NDimensions>& rhs) const
        {
            ArithmeticBuffer result = *this;
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
        friend ArithmeticBuffer operator+(const TLhs& lhs, const ArithmeticBuffer& rhs)
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
        friend ArithmeticBuffer operator-(const TLhs& lhs, const ArithmeticBuffer& rhs)
        {
            ArithmeticBuffer result(BufferFlags::AllocUninitialized, rhs.size);

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
        friend ArithmeticBuffer operator*(const TLhs& lhs, const ArithmeticBuffer& rhs)
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
        friend ArithmeticBuffer operator/(const TLhs& lhs, const ArithmeticBuffer& rhs)
        {
            ArithmeticBuffer result(BufferFlags::AllocUninitialized, rhs.size);

            iterator itResult = result.begin();
            for (const TData& rhsElement : rhs) *(itResult++) = lhs / rhsElement;

            result.flags = rhs.flags;
            return result;
        }

        /**
         * Adds constant value to all elements.
         *
         * @tparam TRhs Type of the rhs.
         * @param rhs Constant to add.
         * @return Reference to current instance.
         */
        template<BufferElement TRhs>
            requires AddAssignable<TData, TRhs>
        ArithmeticBuffer& operator+=(const TRhs& rhs)
        {
            for (TData& element : *this) element += rhs;
            return *this;
        }

        /**
         * Subtracts constant value from all elements.
         *
         * @tparam TRhs Type of the rhs.
         * @param rhs Constant to subtract.
         * @return Reference to current instance.
         */
        template<BufferElement TRhs>
            requires SubtractAssignable<TData, TRhs>
        ArithmeticBuffer& operator-=(const TRhs& rhs)
        {
            for (TData& element : *this) element -= rhs;
            return *this;
        }

        /**
         * Multiplies all elements with a constant.
         *
         * @tparam TRhs Type of the rhs.
         * @param rhs Multiplication factor.
         * @return Reference to current instance.
         */
        template<BufferElement TRhs>
            requires MultiplyAssignable<TData, TRhs>
        ArithmeticBuffer& operator*=(const TRhs& rhs)
        {
            for (TData& element : *this) element *= rhs;
            return *this;
        }

        /**
         * Divides all elements by a constant.
         *
         * @tparam TRhs Type of the rhs.
         * @param rhs Division factor.
         * @return Reference to current instance.
         */
        template<BufferElement TRhs>
            requires DivideAssignable<TData, TRhs>
        ArithmeticBuffer& operator/=(const TRhs& rhs)
        {
            for (TData& element : *this) element /= rhs;
            return *this;
        }

        /**
         * Performs element-wise addition.
         *
         * @tparam TRhs Type of the rhs elements.
         * @param rhs Right operand.
         * @return Reference to current instance.
         * @exception InvalidOperationException
         */
        template<BufferElement TRhsData>
            requires AddAssignable<TData, TRhsData>
        ArithmeticBuffer& operator+=(const ArithmeticBuffer<TRhsData, NDimensions>& rhs)
        {
            if (this->Size() != rhs.Size())
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidOperationException, HEPH_FUNC, "Size of both buffers must be the same.");
            }

            iterator itLhs = this->begin();
            const_iterator itRhs = rhs.cbegin();
            const_iterator itRhsEnd = rhs.cend();
            for (; itRhs != itRhsEnd; ++itLhs, ++itRhs) *itLhs += *itRhs;

            return *this;
        }

        /**
         * Performs element-wise subtraction.
         *
         * @tparam TRhs Type of the rhs elements.
         * @param rhs Right operand.
         * @return Reference to current instance.
         * @exception InvalidOperationException
         */
        template<BufferElement TRhsData>
            requires SubtractAssignable<TData, TRhsData>
        ArithmeticBuffer& operator-=(const ArithmeticBuffer<TRhsData, NDimensions>& rhs)
        {
            if (this->Size() != rhs.Size())
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidOperationException, HEPH_FUNC, "Size of both buffers must be the same.");
            }

            iterator itLhs = this->begin();
            const_iterator itRhs = rhs.cbegin();
            const_iterator itRhsEnd = rhs.cend();
            for (; itRhs != itRhsEnd; ++itLhs, ++itRhs) *itLhs -= *itRhs;

            return *this;
        }

        /**
         * Performs element-wise multiplication.
         *
         * @tparam TRhs Type of the rhs elements.
         * @param rhs Right operand.
         * @return Reference to current instance.
         * @exception InvalidOperationException
         */
        template<BufferElement TRhsData>
            requires MultiplyAssignable<TData, TRhsData>
        ArithmeticBuffer& operator*=(const ArithmeticBuffer<TRhsData, NDimensions>& rhs)
        {
            if (this->Size() != rhs.Size())
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidOperationException, HEPH_FUNC, "Size of both buffers must be the same.");
            }

            iterator itLhs = this->begin();
            const_iterator itRhs = rhs.cbegin();
            const_iterator itRhsEnd = rhs.cend();
            for (; itRhs != itRhsEnd; ++itLhs, ++itRhs) *itLhs *= *itRhs;

            return *this;
        }

        /**
         * Performs element-wise division.
         *
         * @tparam TRhs Type of the rhs elements.
         * @param rhs Right operand.
         * @return Reference to current instance.
         * @exception InvalidOperationException
         */
        template<BufferElement TRhsData>
            requires DivideAssignable<TData, TRhsData>
        ArithmeticBuffer& operator/=(const ArithmeticBuffer<TRhsData, NDimensions>& rhs)
        {
            if (this->Size() != rhs.Size())
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidOperationException, HEPH_FUNC, "Size of both buffers must be the same.");
            }

            iterator itLhs = this->begin();
            const_iterator itRhs = rhs.cbegin();
            const_iterator itRhsEnd = rhs.cend();
            for (; itRhs != itRhsEnd; ++itLhs, ++itRhs) *itLhs /= *itRhs;

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
        virtual ArithmeticBuffer SubBuffer(size_t index, size_t size)
        {
            ArithmeticBuffer result;
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
        virtual void Prepend(const ArithmeticBuffer& src)
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
        virtual void Append(const ArithmeticBuffer& src)
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
        virtual void Insert(const ArithmeticBuffer& src, size_t index)
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
        virtual void Cut(size_t index, size_t size)
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
        virtual void Replace(const ArithmeticBuffer& src, size_t index, size_t srcIndex, size_t size)
        {
            Buffer::Replace(*this, src, index, srcIndex, size);
        }

        /** @copydoc ArithmeticBuffer::Transpose */
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
        virtual void Transpose(const buffer_size_t& perm)
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
        virtual void Resize(const buffer_size_t& newSize)
        {
            Buffer::Resize(*this, newSize);
        }

        /**
         * Swaps the elements of the provided dimension.
         *
         * @param dim 0-based dimension that will be reversed.
         * @exception InvalidArgumentException
         */
        virtual void Reverse(size_t dim = 0)
        {
            Buffer::Reverse(*this, dim);
        }

        /**
         * Inverts the elements.
         *
         * @exception InvalidOperationException
        */
        void Invert()
        {
            if constexpr (std::is_integral_v<TData> && std::is_unsigned_v<TData>)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidOperationException, HEPH_FUNC, "TData must be signed.");
            }
            else
            {
                for (TData& element : *this) element = -element;
            }
        }

        /** Gets the element with the minimum value. */
        TData Min() const
        {
            if constexpr (!HasLessThan<TData>)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidOperationException, HEPH_FUNC, "No suitable '<' operator defined.");
            }
            else
            {
                TData result = ArithmeticBuffer::MAX_ELEMENT;
                for (const TData& element : *this)
                    if (element < result) result = element;
                return result;
            }
        }

        /** Gets the element with the maximum value. */
        TData Max() const
        {
            if constexpr (!HasGreaterThan<TData>)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidOperationException, HEPH_FUNC, "No suitable '>' operator defined.");
            }
            else
            {
                TData result = ArithmeticBuffer::MIN_ELEMENT;
                for (const TData& element : *this)
                    if (element > result) result = element;
                return result;
            }
        }

        /** Gets the element with the maximum absolute value. */
        TData AbsMax() const
        {
            if constexpr (!HasGreaterThan<TData>)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidOperationException, HEPH_FUNC, "No suitable '>' operator defined.");
            }
            else
            {
                TData result = ArithmeticBuffer::MIN_ELEMENT;
                TData absElement = 0;
                for (const TData& element : *this)
                {
                    absElement = std::abs(element);
                    if (absElement > result) result = absElement;
                }
                return result;
            }
        }

        /** Calculates the root mean square. */
        double Rms() const
        {
            if constexpr (!Multipliable<TData, TData, double>)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidOperationException, HEPH_FUNC, "Cannot calculate the rms, invalid type TData.");
            }
            else
            {
                const size_t elementCount = this->ElementCount();
                if (elementCount == 0) return 0;

                double sumSquared = 0;
                for (const TData& element : *this)
                    sumSquared += element * element;
                return std::sqrt(sumSquared / static_cast<double>(elementCount));
            }
        }
    };

    using DoubleBuffer = ArithmeticBuffer<double, 1>;
    extern template class ArithmeticBuffer<double, 1>;

    using ComplexBuffer = ArithmeticBuffer<std::complex<double>, 1>;
    extern template class ArithmeticBuffer<std::complex<double>, 1>;
}

#endif
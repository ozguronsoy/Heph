#ifndef HEPH_ARITHMETIC_BUFFER_H
#define HEPH_ARITHMETIC_BUFFER_H

#include "HephShared.h"
#include "Buffer.h"
#include "HephConcepts.h"
#include <cmath>

/** @file */

namespace Heph
{
    /**
     * @brief Base class for buffers that store arithmetic types. Provides arithmetic operations.
     *
     * @tparam TData Type of the elements stored in buffer. Must be default constructible and trivally destructible.
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
            TData result = ArithmeticBuffer::MAX_ELEMENT;
            for (const TData& element : *this)
                if (element < result) result = element;
            return result;
        }

        /** Gets the element with the maximum value. */
        TData Max() const
        {
            TData result = ArithmeticBuffer::MIN_ELEMENT;
            for (const TData& element : *this)
                if (element > result) result = element;
            return result;
        }

        /** Gets the element with the maximum absolute value. */
        TData AbsMax() const
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

        /** Calculates the root mean square. */
        double Rms() const
        {
            const size_t elementCount = this->ElementCount();
            if (elementCount == 0) return 0;

            double sumSquared = 0;
            for (const TData& element : *this)
                sumSquared += static_cast<double>(element) * static_cast<double>(element);
            return std::sqrt(sumSquared / static_cast<double>(elementCount));
        }
    };
}

#endif
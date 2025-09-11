#ifndef HEPH_BUFFER_H
#define HEPH_BUFFER_H

#include "Heph/Utils.h"
#include "Heph/Buffers/Iterators/BufferIterator.h"
#include "Heph/Enum.h"
#include "Heph/Exceptions/Exception.h"
#include "Heph/Exceptions/InvalidArgumentException.h"
#include "Heph/Exceptions/InvalidOperationException.h"
#include "Heph/Exceptions/InsufficientMemoryException.h"
#include <initializer_list>
#include <tuple>
#include <algorithm>
#include <ranges>
#include <numeric>

/** @file */

namespace Heph
{
    /** @brief Helper trait for declaring the initializer list type used in \ref buffers "Buffer". */
    template<BufferElement TData, size_t NDimensions>
    struct BufferInitializerListHelper
    {
        /** @brief Type of the initializer list. */
        using type = std::initializer_list<typename BufferInitializerListHelper<TData, (NDimensions - 1)>::type>;
    };

    /** @copybrief BufferInitializerListHelper */
    template<BufferElement TData>
    struct BufferInitializerListHelper<TData, 1>
    {
        /** @copybrief BufferInitializerListHelper::type */
        using type = std::initializer_list<TData>;
    };

    /** @brief Specifies how the transpose operation modifies the buffer. */
    enum TransposeMode
    {
        /** @brief Specifies that both memory and strides will be modified. */
        Normal,
        /** @brief Specifies that only strides will be modified, not the memory. */
        InPlace
    };

    /**
    * @brief Container for storing large sequential data.
    *
    * @tparam TData Type of the elements stored in buffer. Must be default constructible and trivally destructible.
    * @tparam NDimensions Number of dimensions.
    * @tparam TIterator Type of the iterator.
    */
    template <
        BufferElement TData,
        size_t NDimensions = 1,
        template<typename, size_t> typename TIterator = BufferIterator
    >
        requires (NDimensions > 0) && BufferIteratorConcept<TIterator<TData, NDimensions>, TData, NDimensions>
    class HEPH_API Buffer
    {
    public:
        /** @brief Type of the iterator used by the Buffer. */
        using iterator = TIterator<TData, NDimensions>;
        /** @brief Type of the constant iterator used by the Buffer. */
        using const_iterator = TIterator<const TData, NDimensions>;
        /** @brief Type of the reverse iterator used by the Buffer. */
        using reverse_iterator = std::reverse_iterator<iterator>;
        /** @brief Type of the constant reverse iterator used by the Buffer. */
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        /** @copybrief BufferIteratorTraits::buffer_size_t */
        using buffer_size_t = iterator::buffer_size_t;
        /** @copybrief BufferIteratorTraits::buffer_index_t */
        using buffer_index_t = iterator::buffer_index_t;

        /** @brief Type of the initializer list. */
        using InitializerList = typename BufferInitializerListHelper<TData, NDimensions>::type;

        /** Specifies the memory will not be initialized after allocation. */
        static constexpr bool ALLOC_UNINITIALIZED = false;
        /** Specifies the memory will be initialized after allocation. */
        static constexpr bool ALLOC_INITIALIZED = true;

    public:
        /** @copybrief BufferIteratorTraits::BUFFER_SIZE_ZERO */
        static constexpr buffer_size_t BUFFER_SIZE_ZERO = iterator::BUFFER_SIZE_ZERO;
        /** @copybrief BufferIteratorTraits::BUFFER_INDEX_ZERO */
        static constexpr buffer_index_t BUFFER_INDEX_ZERO = iterator::BUFFER_INDEX_ZERO;

    protected:
        /** @brief Pointer to the first element of the buffer, or nullptr if the buffer is empty. */
        TData* pData;
        /** @brief Number of elements in each dimension the buffer stores. */
        buffer_size_t size;
        /** @brief Number of elements to advance in one step for each dimension. */
        buffer_size_t strides;

    public:
        /**
         * @copydoc constructor
         *
         * @param size @copybrief size
         * @exception InsufficientMemoryException
         */
        explicit Buffer(auto... size)
            : pData(nullptr), size(BUFFER_SIZE_ZERO), strides(BUFFER_SIZE_ZERO)
        {
            static_assert(sizeof...(size) <= NDimensions, "Invalid number of size parameters parameters.");
            static_assert((std::is_convertible_v<decltype(size), size_t> && ...), "Invalid type for size parameters, must be convertible to size_t.");

            if constexpr (sizeof...(size) > 0)
            {
                if constexpr (NDimensions == 1) this->size = std::get<0>(std::forward_as_tuple(static_cast<size_t>(size)...));
                else this->size = { static_cast<size_t>(size)... };
            }

            const size_t elementCount = this->ElementCount();
            if (elementCount > 0)
            {
                this->pData = Buffer::Allocate(elementCount, ALLOC_INITIALIZED);
            }

            this->CalcStrides();
        }

        /**
         * @copydoc constructor
         *
         * @param size @copybrief size
         * @exception InsufficientMemoryException
         */
        explicit Buffer(const buffer_size_t& size)
            : pData(nullptr), size(size), strides(BUFFER_SIZE_ZERO)
        {
            const size_t elementCount = this->ElementCount();
            if (elementCount > 0)
            {
                this->pData = Buffer::Allocate(elementCount, ALLOC_INITIALIZED);
            }
            this->CalcStrides();
        }

        /**
         * @copydoc constructor
         *
         * @param rhs Initializer list.
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        Buffer(const InitializerList& rhs)
            : Buffer()
        {
            *this = rhs;
        }

        /** @copydoc copy_constructor */
        Buffer(const Buffer& rhs)
            : Buffer()
        {
            *this = rhs;
        }

        /** @copydoc move_constructor */
        Buffer(Buffer&& rhs) noexcept
            : Buffer()
        {
            *this = std::move(rhs);
        }

        /** @copydoc destructor */
        virtual ~Buffer()
        {
            this->Release();
        }

        /**
         * Copies the elements of ``rhs``.
         *
         * @param rhs Initializer list.
         * @return Reference to current instance.
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        Buffer& operator=(const InitializerList& rhs)
        {
            if (rhs.size() == 0) return *this;

            if constexpr (NDimensions == 1)
            {
                Buffer::Reallocate(this->pData, 0, rhs.size(), ALLOC_UNINITIALIZED);
                this->size = rhs.size();
                this->CalcStrides();

                (void)std::copy(rhs.begin(), rhs.end(), this->begin());
            }
            else
            {
                this->size[0] = rhs.size();

                {
                    Buffer<TData, (NDimensions - 1)> temp(*rhs.begin());

                    if (temp.ElementCount() == 0) return *this;

                    Buffer::Reallocate(this->pData, 0, temp.ElementCount() * rhs.size(), ALLOC_UNINITIALIZED);
                    if constexpr (NDimensions == 2)
                        this->size[1] = temp.Size();
                    else
                        (void)std::copy(temp.Size().begin(), temp.Size().end(), this->size.begin() + 1);
                }
                this->CalcStrides();


                iterator it = this->begin();
                for (const auto& sublist : rhs)
                {
                    Buffer<TData, (NDimensions - 1)> temp(sublist);

                    if constexpr (NDimensions == 2)
                    {
                        if (this->size[1] != temp.Size())
                        {
                            HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Size of all sub lists must be the same.");
                        }
                    }
                    else
                    {
                        if (!std::equal(temp.Size().begin(), temp.Size().end(), this->size.begin() + 1))
                        {
                            HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Size of all sub lists must be the same.");
                        }
                    }

                    (void)std::copy(temp.begin(), temp.end(), it);

                    it.IncrementIndex(0, 1);
                }
            }

            return *this;
        }

        /**
         * Copies the contents of ``rhs``.
         *
         * @param rhs Instance to copy.
         * @return Reference to current instance.
         * @exception InsufficientMemoryException
         */
        Buffer& operator=(const Buffer& rhs)
        {
            if (&rhs != this)
            {
                const size_t newElementCount = rhs.ElementCount();
                if (newElementCount > 0)
                {
                    Buffer::Reallocate(this->pData, this->ElementCount(), newElementCount, ALLOC_UNINITIALIZED);
                    this->size = rhs.size;
                    this->strides = rhs.strides;

                    (void)std::copy(rhs.begin(), rhs.end(), this->begin());
                }
            }

            return *this;
        }

        /**
         * Moves the contents of ``rhs``.
         *
         * @param rhs Instance to move.
         * @return Reference to current instance.
         */
        Buffer& operator=(Buffer&& rhs) noexcept
        {
            if (&rhs != this)
            {
                this->Release();

                this->pData = rhs.pData;
                this->size = rhs.size;
                this->strides = rhs.strides;

                rhs.pData = nullptr;
                rhs.size = BUFFER_SIZE_ZERO;
                rhs.strides = BUFFER_SIZE_ZERO;
            }

            return *this;
        }

        /**
         * Gets the element at the provided index.
         *
         * @param indices Indices of the element.
         * @return Reference to the element.
         */
        TData& operator[](auto... indices)
        {
            static_assert(sizeof...(indices) > 0 && sizeof...(indices) <= NDimensions, "Invalid number of indices parameters.");
            static_assert((std::is_convertible_v<decltype(indices), index_t> && ...), "Invalid type for indices parameters, must be convertible to index_t.");

            return iterator::template Get<false>(this->pData, this->size, this->strides, std::forward<index_t>(static_cast<index_t>(indices))...);
        }

        /** @copydoc operator[] */
        TData& operator[](const buffer_index_t& indices)
        {
            return iterator::template Get<false>(this->pData, this->size, this->strides, indices);
        }

        /** @copydoc operator[] */
        const TData& operator[](auto... indices) const
        {
            static_assert(sizeof...(indices) > 0 && sizeof...(indices) <= NDimensions, "Invalid number of indices parameters.");
            static_assert((std::is_convertible_v<decltype(indices), index_t> && ...), "Invalid type for indices parameters, must be convertible to index_t.");

            return const_iterator::template Get<false>(this->pData, this->size, this->strides, std::forward<index_t>(static_cast<index_t>(indices))...);
        }

        /** @copydoc operator[] */
        const TData& operator[](const buffer_index_t& indices) const
        {
            return const_iterator::template Get<false>(this->pData, this->size, this->strides, indices);
        }

        /**
         * Compares the elements of two buffers.
         *
         * @param rhs Buffer to compare.
         * @return ``true`` if elements are equal, otherwise ``false``.
         */
        bool operator==(const Buffer& rhs) const
        {
            return (this->IsEmpty() && rhs.IsEmpty()) || (this->size == rhs.size && std::equal(this->begin(), this->end(), rhs.begin()));
        }

        /**
         * Checks whether the buffer is empty.
         *
         * @return true if the buffer is empty, otherwise false.
         */
        bool IsEmpty() const
        {
            return (this->ElementCount() == 0) || (this->pData == nullptr);
        }

        /** Gets the number of elements in each dimension the buffer stores. */
        const buffer_size_t& Size() const
        {
            return this->size;
        }

        /**
         * Gets the number of elements of the dimension.
         *
         * @param dim 0-based dimension.
         * @exception InvalidArgumentException
         */
        size_t Size(size_t dim) const
        {
            if (dim >= NDimensions)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Invalid dimension.");
            }

            if constexpr (NDimensions == 1) return this->size;
            else return this->size[dim];
        }

        /** Gets the total number of elements. */
        size_t ElementCount() const
        {
            return Buffer::ElementCount(this->size);
        }

        /**
         * @copydoc operator[]
         *
         * @exception InvalidArgumentException
         */
        TData& At(auto... indices)
        {
            static_assert(sizeof...(indices) > 0 && sizeof...(indices) <= NDimensions, "Invalid number of indices parameters.");
            static_assert((std::is_convertible_v<decltype(indices), index_t> && ...), "Invalid type for indices parameters, must be convertible to index_t.");

            return iterator::template Get<true>(this->pData, this->size, this->strides, std::forward<index_t>(static_cast<index_t>(indices))...);
        }

        /** @copydoc At */
        TData& At(const buffer_index_t& indices)
        {
            return iterator::template Get<true>(this->pData, this->size, this->strides, indices);
        }

        /** @copydoc At */
        const TData& At(auto... indices) const
        {
            static_assert(sizeof...(indices) > 0 && sizeof...(indices) <= NDimensions, "Invalid number of indices parameters.");
            static_assert((std::is_convertible_v<decltype(indices), index_t> && ...), "Invalid type for indices parameters, must be convertible to index_t.");

            return const_iterator::template Get<true>(this->pData, this->size, this->strides, std::forward<index_t>(static_cast<index_t>(indices))...);
        }

        /** @copydoc At */
        const TData& At(const buffer_index_t& indices) const
        {
            return const_iterator::template Get<true>(this->pData, this->size, this->strides, indices);
        }

        /** Sets all elements to default value. */
        void Reset()
        {
            if (!this->IsEmpty())
                std::fill(this->begin(), this->end(), TData());
        }

        /** Releases the resources. */
        virtual void Release()
        {
            this->size = BUFFER_SIZE_ZERO;
            this->strides = BUFFER_SIZE_ZERO;

            if (this->pData != nullptr)
            {
                free(this->pData);
                this->pData = nullptr;
            }
        }

        /** Returns an iterator to the beginning. */
        iterator begin()
        {
            return iterator(this->pData, this->size, this->strides, BUFFER_INDEX_ZERO);
        }

        /** @copydoc begin */
        const_iterator begin() const
        {
            return const_iterator(this->pData, this->size, this->strides, BUFFER_INDEX_ZERO);
        }

        /** @copydoc begin */
        const_iterator cbegin() const
        {
            return this->begin();
        }

        /** Returns a reverse iterator to the beginning. */
        reverse_iterator rbegin()
        {
            return std::make_reverse_iterator(this->end());
        }

        /** @copydoc rbegin */
        const_reverse_iterator rbegin() const
        {
            return std::make_reverse_iterator(this->end());
        }

        /** @copydoc rbegin */
        const_reverse_iterator crbegin() const
        {
            return std::make_reverse_iterator(this->cend());
        }

        /** Returns an iterator to the end. */
        iterator end()
        {
            const buffer_index_t indices = { static_cast<index_t>(this->Size(0)) };
            return iterator(this->pData, this->size, this->strides, indices);
        }

        /** @copydoc end */
        const_iterator end() const
        {
            const buffer_index_t indices = { static_cast<index_t>(this->Size(0)) };
            return const_iterator(this->pData, this->size, this->strides, indices);
        }

        /** @copydoc end */
        const_iterator cend() const
        {
            return this->end();
        }

        /** Returns a reverse iterator to the end. */
        reverse_iterator rend()
        {
            return std::make_reverse_iterator(this->begin());
        }

        /** @copydoc rend */
        const_reverse_iterator rend() const
        {
            return std::make_reverse_iterator(this->begin());
        }

        /** @copydoc rend */
        const_reverse_iterator crend() const
        {
            return std::make_reverse_iterator(this->cbegin());
        }

    protected:
        /** Calculates strides. */
        void CalcStrides()
        {
            if constexpr (NDimensions == 1) this->strides = 1;
            else
            {
                for (size_t i = 0; i < NDimensions; ++i)
                {
                    this->strides[i] = std::accumulate(this->size.begin() + i + 1, this->size.end(), 1uz, std::multiplies<size_t>());
                }
            }
        }

        /**
         * @copydoc ElementCount
         *
         * @param size @copybrief size
         */
        static inline size_t ElementCount(const buffer_size_t& size)
        {
            if constexpr (NDimensions == 1) return size;
            else return std::accumulate(size.begin(), size.end(), 1uz, std::multiplies<size_t>());
        }

        /**
         * Allocates memory.
         *
         * @param elementCount Number of elements to allocate.
         * @param init Indicates whether to initialize the memory after successfull allocation.
         * @return Pointer to the allocated memory.
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        static TData* Allocate(size_t elementCount, bool init)
        {
            if (elementCount == 0)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Element count cannot be 0.");
            }

            TData* pData = reinterpret_cast<TData*>(malloc(elementCount * sizeof(TData)));
            if (pData == nullptr)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InsufficientMemoryException, HEPH_FUNC, std::format("Failed to allocate {} bytes.", elementCount * sizeof(TData)));
            }

            if (init)
            {
                std::fill(
                    pData,
                    (pData + elementCount),
                    TData()
                );
            }

            return pData;
        }

        /**
         * Reallocates memory.
         *
         * @param pData Pointer to the data that will be reallocated.
         * @param oldElementCount Old number of elements.
         * @param newElementCount New number of elements.
         * @param init Indicates whether to initialize the memory after successfull allocation.
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        static void Reallocate(TData*& pData, size_t oldElementCount, size_t newElementCount, bool init)
        {
            if (newElementCount == 0)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "New element count cannot be 0.");
            }

            TData* pTemp = reinterpret_cast<TData*>(realloc(pData, newElementCount * sizeof(TData)));
            if (pTemp == nullptr)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InsufficientMemoryException, HEPH_FUNC, std::format("Failed to reallocate {} bytes.", newElementCount * sizeof(TData)));
            }

            if (init && newElementCount > oldElementCount)
            {
                std::fill(
                    (pTemp + oldElementCount),
                    (pTemp + newElementCount),
                    TData()
                );
            }

            pData = pTemp;
        }

        /**
         * Shiftes the top-level entries to the left by provided amount.
         *
         * @note This method does not change the size of the buffer,
         * thus the entries that are shifted out of bounds will be lost.
         *
         * @param buffer Buffer that will be shifted.
         * @param n Number of top-level entries to shift.
         * @exception InvalidOperationException
         */
        static void ShiftLeft(Buffer& buffer, size_t n)
        {
            if (buffer.IsEmpty())
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidOperationException, HEPH_FUNC, "Buffer cannot be empty.");
            }

            if (n == 0) return;
            if (n >= buffer.Size(0))
            {
                buffer.Reset();
                return;
            }

            iterator itBegin = buffer.begin();
            itBegin.IncrementIndex(0, n);

            (void)std::move(itBegin, buffer.end(), buffer.begin());

            // set the invalid entries at the end to default value
            itBegin = buffer.end();
            itBegin.DecrementIndex(0, n);

            std::fill(itBegin, buffer.end(), TData());
        }

        /**
         * Shiftes the top-level entries to the right by provided amount.
         *
         * @note This method does not change the size of the buffer,
         * thus the entries that are shifted out of bounds will be lost.
         *
         * @param buffer Buffer that will be shifted.
         * @param n Number of top-level entries to shift.
         * @exception InvalidOperationException
         */
        static void ShiftRight(Buffer& buffer, size_t n)
        {
            if (buffer.IsEmpty())
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidOperationException, HEPH_FUNC, "Buffer cannot be empty.");
            }

            if (n == 0) return;
            if (n >= buffer.Size(0))
            {
                buffer.Reset();
                return;
            }

            iterator itSrcEnd = buffer.end();
            itSrcEnd.DecrementIndex(0, n);

            (void)std::move_backward(buffer.begin(), itSrcEnd, buffer.end());

            iterator itDestBegin = buffer.begin();
            itDestBegin.IncrementIndex(0, n);

            std::fill(buffer.begin(), itDestBegin, TData());
        }

        /**
         * Copies a section of the buffer.
         *
         * @note This method allocates memory for the destination buffer, hence no need to allocate in advance.
         *
         * @param src The buffer whose elements will be copied to the destination.
         * @param dest The buffer which elements will be coppied to.
         * @param index Index of the first top-level entry that will be copied.
         * @param size Number of top-level entries to copy.
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        static void SubBuffer(const Buffer& src, Buffer& dest, index_t index, size_t size)
        {
            if (index < 0 || index >= src.Size(0))
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Index out of bounds.");
            }

            if (size == 0) return;

            buffer_size_t oldSize = src.size; // do not initialize the section that will be copied.
            buffer_size_t newSize = src.size;
            if constexpr (NDimensions == 1)
            {
                oldSize -= index;
                newSize = size;
            }
            else
            {
                oldSize[0] -= index;
                newSize[0] = size;
            }

            Buffer::Reallocate(dest.pData, Buffer::ElementCount(oldSize), Buffer::ElementCount(newSize), ALLOC_INITIALIZED);
            dest.size = newSize;
            dest.CalcStrides();

            if (index + size > src.Size(0))
            {
                size = src.Size(0) - index;
            }

            const_iterator itSrcStart = src.cbegin();
            itSrcStart.IncrementIndex(0, index);

            const_iterator itSrcEnd = src.cbegin();
            itSrcEnd.IncrementIndex(0, index + size);

            (void)std::copy(itSrcStart, itSrcEnd, dest.begin());
        }

        /**
         * Prepends the elements of the source buffer to the destination buffer.
         *
         * @param dest The buffer to which elements will be prepended.
         * @param src The buffer whose elements will be prepended to the destination.
         * @exception InvalidOperationException
         * @exception InsufficientMemoryException
         */
        static void Prepend(Buffer& dest, const Buffer& src)
        {
            if (src.IsEmpty()) return;
            if (dest.IsEmpty())
            {
                dest = src;
                return;
            }

            if constexpr (NDimensions > 1)
            {
                if (!std::equal(dest.size.cbegin() + 1, dest.size.cend(), src.size.cbegin() + 1))
                {
                    HEPH_EXCEPTION_RAISE_AND_THROW(InvalidOperationException, HEPH_FUNC, "All dimensions except the first must have the same size in both buffers.");
                }
            }

            Buffer::Reallocate(dest.pData, dest.ElementCount(), dest.ElementCount() + src.ElementCount(), ALLOC_UNINITIALIZED);

            if (&dest == &src)
            {
                (void)std::copy(dest.begin(), dest.end(), dest.end());

                if constexpr (NDimensions == 1) dest.size += src.size;
                else dest.size[0] += src.size[0];

                return;
            }

            if constexpr (NDimensions == 1) dest.size += src.size;
            else dest.size[0] += src.size[0];

            Buffer::ShiftRight(dest, src.Size(0));
            (void)std::copy(src.begin(), src.end(), dest.begin());
        }

        /**
         * Appends the elements of the source buffer to the destination buffer.
         *
         * @note For multidimensional buffers, all dimensions except the first must have the same size in both buffers.
         *
         * @param dest The buffer to which elements will be appended.
         * @param src The buffer whose elements will be appended to the destination.
         * @exception InvalidOperationException
         * @exception InsufficientMemoryException
         */
        static void Append(Buffer& dest, const Buffer& src)
        {
            if (src.IsEmpty()) return;
            if (dest.IsEmpty())
            {
                dest = src;
                return;
            }

            if constexpr (NDimensions > 1)
            {
                if (!std::equal(dest.size.cbegin() + 1, dest.size.cend(), src.size.cbegin() + 1))
                {
                    HEPH_EXCEPTION_RAISE_AND_THROW(InvalidOperationException, HEPH_FUNC, "All dimensions except the first must have the same size in both buffers.");
                }
            }

            Buffer::Reallocate(dest.pData, dest.ElementCount(), dest.ElementCount() + src.ElementCount(), ALLOC_UNINITIALIZED);
            (void)std::copy(src.begin(), src.end(), dest.end()); // dest size is not updated yet

            if constexpr (NDimensions == 1) dest.size += src.size;
            else dest.size[0] += src.size[0];
        }

        /**
         * Inserts the elements of the source buffer to the destination buffer.
         *
         * @note For multidimensional buffers, all dimensions except the first must have the same size in both buffers.
         *
         * @param dest The buffer to which elements will be inserted.
         * @param src The buffer whose elements will be inserted to the destination.
         * @exception InvalidArgumentException
         * @exception InvalidOperationException
         * @exception InsufficientMemoryException
         */
        static void Insert(Buffer& dest, const Buffer& src, size_t index)
        {
            if (index > dest.Size(0))
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Index out of bounds.");
            }

            if (index == 0)
            {
                Buffer::Prepend(dest, src);
                return;
            }
            else if (index == dest.Size(0))
            {
                Buffer::Append(dest, src);
                return;
            }

            if constexpr (NDimensions > 1)
            {
                if (!std::equal(dest.size.cbegin() + 1, dest.size.cend(), src.size.cbegin() + 1))
                {
                    HEPH_EXCEPTION_RAISE_AND_THROW(InvalidOperationException, HEPH_FUNC, "All dimensions except the first must have the same size in both buffers.");
                }
            }

            Buffer::Reallocate(dest.pData, dest.ElementCount(), dest.ElementCount() + src.ElementCount(), ALLOC_UNINITIALIZED);

            iterator itInsertBegin = dest.begin();
            itInsertBegin.IncrementIndex(0, index);

            iterator itShiftDestBegin = itInsertBegin;
            itShiftDestBegin.IncrementIndex(0, src.Size(0));

            iterator itShiftDestEnd = dest.end();
            itShiftDestEnd.IncrementIndex(0, src.Size(0));

            (void)std::move_backward(itInsertBegin, dest.end(), itShiftDestEnd); // shift right side of the dest buffer

            if (&dest == &src)
            {
                (void)std::copy(dest.begin(), itInsertBegin, itInsertBegin); // left side

                itInsertBegin.IncrementIndex(0, index);
                (void)std::copy(itShiftDestBegin, itShiftDestEnd, itInsertBegin); // right side
            }
            else
            {
                (void)std::copy(src.begin(), src.end(), itInsertBegin);
            }

            if constexpr (NDimensions == 1) dest.size += src.size;
            else dest.size[0] += src.size[0];
        }

        /**
         * Removes a section of the buffer.
         *
         * @param buffer The buffer to be cut.
         * @param index Index of the first top-level entry that will be removed.
         * @param size Number of top-level entries to remove.
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        static void Cut(Buffer& buffer, index_t index, size_t size)
        {
            if (index < 0 || index >= buffer.Size(0))
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Index out of bounds.");
            }

            if (size == 0) return;

            if (index == 0 && size >= buffer.Size(0))
            {
                buffer.Release();
                return;
            }

            if (index + size > buffer.Size(0))
            {
                size = buffer.Size(0) - index;
            }

            buffer_size_t newSize = buffer.size;
            if constexpr (NDimensions == 1)
                newSize -= size;
            else
                newSize[0] -= size;

            Buffer temp(newSize);

            const_iterator itBuffer = buffer.cbegin();
            itBuffer.IncrementIndex(0, index);

            if (index > 0)
                (void)std::move(buffer.cbegin(), itBuffer, temp.begin());

            if (index + size < buffer.Size(0))
            {
                iterator itTemp = temp.begin();
                itTemp.IncrementIndex(0, index);
                itBuffer.IncrementIndex(0, size);

                (void)std::move(itBuffer, buffer.cend(), itTemp);
            }

            std::swap(buffer.pData, temp.pData);

            buffer.size = newSize;
            buffer.CalcStrides();
        }

        /**
         * Replaces a section of the buffer.
         *
         * @note This method does not allocate extra memory, just replaces the existing elements.
         *
         * @param b1 Buffer whose elements will be replaced.
         * @param b2 Buffer providing the replacement elements.
         * @param b1Index Starting index in b1 where replacement begins.
         * @param b2Index Starting index in b2 from which top-level entries are copied.
         * @param size Number of top-level entries to replace.
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        static void Replace(Buffer& b1, const Buffer& b2, index_t b1Index, index_t b2Index, size_t size)
        {
            if ((b1Index < 0 || b1Index >= b1.Size(0)) || (b2Index < 0 || b2Index >= b2.Size(0)))
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Index out of bounds.");
            }

            if ((b1Index + size) > b1.Size(0) || (b2Index + size) > b2.Size(0))
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Invalid size.");
            }

            if (&b1 == &b2)
            {
                Buffer temp = b2;
                Buffer::Replace(temp, b2, b1Index, b2Index, size);
                std::swap(b1.pData, temp.pData);
                return;
            }

            iterator itB1Begin = b1.begin();
            itB1Begin.IncrementIndex(0, b1Index);

            const_iterator itB2Begin = b2.begin();
            itB2Begin.IncrementIndex(0, b2Index);

            const_iterator itB2End = itB2Begin;
            itB2End.IncrementIndex(0, size);

            (void)std::copy(itB2Begin, itB2End, itB1Begin);
        }

        /**
         * Transposes a multidimensional buffer.
         *
         * @note This method allocates memory for the destination buffer, hence no need to allocate in advance.
         *
         * @param src The source buffer.
         * @param dest The destination buffer.
         * @param perm Permutation of the dimensions.
         * @param mode Specifies how the transpose operation modifies the buffer.
         * @exception InvalidOperationException
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        static void Transpose(const Buffer& src, Buffer& dest, const buffer_size_t& perm, Enum<TransposeMode> mode)
        {
            if constexpr (NDimensions == 1 || !std::equality_comparable<TData>)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidOperationException, HEPH_FUNC, "Cannot transpose a 1D buffer.");
            }
            else
            {
                const bool sameInstance = &src == &dest;
                const bool inPlace = mode.Test(TransposeMode::InPlace);

                if (sameInstance && !inPlace)
                {
                    Buffer temp;
                    Buffer::Transpose(src, temp, perm, mode);
                    dest = std::move(temp);
                    return;
                }

                // validate perm
                for (size_t i = 0; i < NDimensions; ++i)
                {
                    if (perm[i] >= NDimensions)
                    {
                        HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Invalid dimension.");
                    }

                    for (size_t j = (i + 1); j < NDimensions; ++j)
                    {
                        if (perm[i] == perm[j])
                        {
                            HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Duplicate dimension not allowed in perm.");
                        }
                    }
                }

                if (!sameInstance)
                {
                    Buffer::Reallocate(dest.pData, dest.ElementCount(), src.ElementCount(), ALLOC_UNINITIALIZED);
                    dest.size = src.size;
                    dest.strides = src.strides;
                }

                if (inPlace)
                {
                    if (sameInstance)
                    {
                        buffer_size_t newSize;
                        buffer_size_t newStrides;

                        for (size_t d = 0; d < NDimensions; ++d)
                        {
                            newSize[d] = src.size[perm[d]];
                            newStrides[d] = src.strides[perm[d]];
                        }

                        dest.size = newSize;
                        dest.strides = newStrides;
                    }
                    else
                    {
                        (void)std::move(src.begin(), src.end(), dest.begin());
                        for (size_t d = 0; d < NDimensions; ++d)
                        {
                            dest.size[d] = src.size[perm[d]];
                            dest.strides[d] = src.strides[perm[d]];
                        }
                    }
                }
                else
                {
                    for (size_t d = 0; d < NDimensions; ++d)
                    {
                        dest.size[d] = src.size[perm[d]];
                    }
                    dest.CalcStrides();

                    // optimize this later
                    const const_iterator itEnd = src.cend();
                    for (const_iterator it = src.cbegin(); it != itEnd; ++it)
                    {
                        buffer_index_t outputIndices = BUFFER_INDEX_ZERO;
                        for (size_t d = 0; d < NDimensions; ++d)
                        {
                            outputIndices[d] = it.Indices()[perm[d]];
                        }
                        dest[outputIndices] = *it;
                    }
                }
            }
        }

        /**
         * Changes the size of the buffer.<br>
         * If the new size is less than the old, elements at the end will be removed.
         *
         * @param buffer The buffer to be resized.
         * @param newSize New size of the buffer.
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        static void Resize(Buffer& buffer, const buffer_size_t& newSize)
        {
            const size_t newElementCount = Buffer::ElementCount(newSize);
            if (newElementCount == 0)
            {
                buffer.Release();
            }
            else
            {
                if constexpr (NDimensions == 1)
                {
                    Buffer::Reallocate(buffer.pData, buffer.ElementCount(), newElementCount, ALLOC_INITIALIZED);
                }
                else
                {
                    Buffer temp(newSize);

                    iterator it = temp.begin();
                    iterator itEnd = temp.end();
                    for (; it != itEnd; ++it)
                    {
                        const buffer_index_t indices = it.Indices();
                        if (std::ranges::equal(indices, buffer.size, std::less()))
                            *it = buffer[indices];
                        else
                            *it = TData();
                    }

                    std::swap(buffer.pData, temp.pData);
                }

                buffer.size = newSize;
                buffer.CalcStrides();
            }
        }

        /**
         * Swaps the elements of the provided dimension.
         *
         * @param buffer The buffer to be reversed.
         * @param dim 0-based dimension that will be reversed.
         * @exception InvalidArgumentException
         */
        static void Reverse(Buffer& buffer, size_t dim)
        {
            if (dim >= NDimensions)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Invalid dimension.");
            }

            if constexpr (NDimensions == 1)
            {
                const size_t halfSize = buffer.size / 2;
                for (size_t i = 0; i < halfSize; ++i)
                {
                    std::swap(buffer[i], buffer[buffer.size - i - 1]);
                }
            }
            else
            {
                const size_t dimSize = buffer.size[dim];
                buffer.size[dim] /= 2;

                const iterator itEnd = buffer.end();
                for (iterator it = buffer.begin(); it != itEnd; ++it)
                {
                    buffer_index_t secondElementIndices = it.Indices();
                    secondElementIndices[dim] = dimSize - secondElementIndices[dim] - 1;

                    std::swap(*it, buffer[secondElementIndices]);
                }

                buffer.size[dim] = dimSize;
            }
        }
    };
}

#endif
#ifndef HEPH_BUFFER_H
#define HEPH_BUFFER_H

#include "HephShared.h"
#include "BufferFlags.h"
#include "BufferIterator.h"
#include "Enum.h"
#include "Exceptions/InsufficientMemoryException.h"
#include "Exceptions/InvalidArgumentException.h"
#include "Exceptions/InvalidOperationException.h"
#include <algorithm>
#include <numeric>


/** @file */

namespace Heph
{
    /**
     * @brief Container for storing large sequential data.
     *
     * @tparam TSelf Type of the derived buffer (CRTP).
     * @tparam TData Type of the stored data. Must be default constructible and trivally destructible.
     * @tparam NDimensions Number of dimensions.
     */
    template<typename TSelf, BufferElement TData, size_t NDimensions = 1>
        requires (NDimensions > 0)
    class HEPH_API Buffer
    {
    public:
        /** @brief Type of the iterator used by the Buffer. */
        using iterator = BufferIterator<TData, NDimensions>;
        /** @brief Type of the constant iterator used by the Buffer. */
        using const_iterator = BufferIterator<const TData, NDimensions>;
        /** @copybrief iterator::buffer_size_t */
        using buffer_size_t = iterator::buffer_size_t;
        /** @copybrief iterator::BUFFER_SIZE_ZERO */
        static constexpr buffer_size_t BUFFER_SIZE_ZERO = iterator::BUFFER_SIZE_ZERO;

    protected:
        /** Pointer to the first element of the buffer, or nullptr if the buffer is empty. */
        TData* pData;

        /** Number of elements in each dimension the buffer stores. */
        buffer_size_t bufferSize;

        /** Number of elements to advance in one step for each dimension. */
        buffer_size_t strides;

        /** Specifies the configuration flags for the buffer. */
        Enum<BufferFlags> flags;

    public:
        /**
         * @copydoc constructor
         *
         * @param flags @copydetails flags
         * @param args @copydetails bufferSize
         * @exception InsufficientMemoryException
         */
        template<typename... Args>
        constexpr explicit Buffer(Enum<BufferFlags> flags = BufferFlags::None, Args... args)
            : pData(nullptr), flags(flags)
        {
            static_assert(std::derived_from<TSelf, Buffer<TSelf, TData, NDimensions>>, "TSelf must derive from Buffer<TSelf, TData, NDimensions>.");

            const std::array<size_t, NDimensions> bufferSize = { static_cast<size_t>(std::forward<Args>(args))... };
            if constexpr (NDimensions == 1) this->bufferSize = bufferSize[0];
            else this->bufferSize = bufferSize;

            const size_t elementCount = this->ElementCount();
            if (elementCount > 0)
            {
                this->pData = Buffer::Allocate(elementCount * sizeof(TData), flags);
            }

            this->CalcStrides();
        }

        /**
         * @copydoc constructor
         *
         * @param flags @copydetails flags
         * @param bufferSize @copydetails bufferSize
         * @exception InsufficientMemoryException
         */
        explicit Buffer(Enum<BufferFlags> flags, const buffer_size_t& bufferSize)
            : pData(nullptr), bufferSize(bufferSize), flags(flags)
        {
            const size_t elementCount = this->ElementCount();
            if (elementCount > 0)
            {
                this->pData = Buffer::Allocate(elementCount * sizeof(TData), flags);
            }
            this->CalcStrides();
        }

        /** @copydoc copy_constructor */
        Buffer(const Buffer& rhs)
            : pData(nullptr)
        {
            *this = rhs;
        }

        /** @copydoc move_constructor */
        Buffer(Buffer&& rhs) noexcept
            : pData(nullptr)
        {
            *this = std::move(rhs);
        }

        /** @copydoc destructor */
        virtual ~Buffer()
        {
            this->Release();
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
                this->Release();

                const size_t elementCount = rhs.ElementCount();
                if (elementCount > 0)
                {
                    this->pData = Buffer::Allocate(elementCount, BufferFlags::AllocUninitialized);
                    (void)std::copy(rhs.begin(), rhs.end(), this->begin());
                }

                this->bufferSize = rhs.bufferSize;
                this->strides = rhs.strides;
                this->flags = rhs.flags;
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
                this->pData = rhs.pData;
                this->bufferSize = std::move(rhs.bufferSize);
                this->strides = std::move(rhs.strides);
                this->flags = rhs.flags;

                rhs.pData = nullptr;
                rhs.bufferSize = BUFFER_SIZE_ZERO;
                rhs.strides = BUFFER_SIZE_ZERO;
                rhs.flags = BufferFlags::None;
            }

            return *this;
        }

        /**
         * Gets the element at the provided index.
         *
         * @param args Indices of the element.
         * @return Reference to the element.
         */
        template<typename... Args>
            requires (sizeof...(Args) == NDimensions && (std::is_convertible_v<Args, size_t> && ...))
        constexpr TData& operator[](Args... args)
        {
            return iterator::template Get<false>(this->pData, this->flags, this->bufferSize, this->strides, std::forward<Args>(args)...);
        }

        /**
         * Gets the element at the provided index.
         *
         * @param args Indices of the element.
         * @return Reference to the element.
         */
        template<typename... Args>
            requires (sizeof...(Args) == NDimensions && (std::is_convertible_v<Args, size_t> && ...))
        constexpr const TData& operator[](Args... args) const
        {
            return const_iterator::template Get<false>(this->pData, this->flags, this->bufferSize, this->strides, std::forward<Args>(args)...);
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
            return (this->size == 0) || (this->pData == nullptr);
        }

        /** Gets the buffer flags. */
        Enum<BufferFlags> Flags() const
        {
            return this->flags;
        }

        /** Sets the buffer flags. */
        void SetFlags(BufferFlags flags)
        {
            this->flags = flags;
        }

        /** Gets the number of elements in each dimension the buffer stores. */
        const buffer_size_t& Size() const
        {
            return this->bufferSize;
        }

        /**
         * Gets the number of elements for ``dim``.
         *
         * @param dim 0-based dimension.
         * @exception InvalidArgumentException
         */
        constexpr size_t Size(size_t dim) const
        {
            if (dim >= NDimensions)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Invalid dimension.");
            }

            if constexpr (NDimensions == 1) return this->bufferSize;
            else return this->bufferSize[dim];
        }

        /** Gets the total number of elements. */
        size_t ElementCount() const
        {
            return Buffer::ElementCount(this->bufferSize);
        }

        /**
         * @copydoc operator[]
         *
         * @exception InvalidArgumentException
         */
        template<typename... Args>
            requires (sizeof...(Args) == NDimensions && (std::is_convertible_v<Args, size_t> && ...))
        TData& At(Args... args)
        {
            return iterator::template Get<true>(this->pData, this->flags, this->bufferSize, this->strides, std::forward<Args>(args)...);
        }

        /** @copydoc At */
        template<typename... Args>
            requires (sizeof...(Args) == NDimensions && (std::is_convertible_v<Args, size_t> && ...))
        const TData& At(Args... args) const
        {
            return const_iterator::template Get<true>(this->pData, this->flags, this->bufferSize, this->strides, std::forward<Args>(args)...);
        }

        /**
         * Transposes a multidimensional buffer.
         *
         * @param args Permutation of the dimensions.
         * @exception InvalidOperationException
         * @exception InvalidArgumentException
         */
        template<typename... Args>
            requires (sizeof...(Args) == NDimensions && (std::is_convertible_v<Args, size_t> && ...))
        void Transpose(Args... args)
        {
            const std::array<size_t, NDimensions> perm = { static_cast<size_t>(std::forward<Args>(args))... };
            this->Transpose(perm);
        }

        /**
         * Transposes a multidimensional buffer.
         *
         * @param perm Permutation of the dimensions.
         * @exception InvalidOperationException
         * @exception InvalidArgumentException
         */
        constexpr void Transpose(const std::array<size_t, NDimensions>& perm)
        {
            if constexpr (NDimensions == 1 || !std::equality_comparable<TData>)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidOperationException, HEPH_FUNC, "Cannot transpose a 1D buffer.");
            }
            else
            {
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

                // TODO
            }
        }

        /**
         * Creates a transposed copy of the multidimensional buffer.
         *
         * @param args Permutation of the dimensions.
         * @exception InvalidOperationException
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        template<typename... Args>
            requires (sizeof...(Args) == NDimensions && (std::is_convertible_v<Args, size_t> && ...))
        TSelf Transposed(Args... args) const
        {
            TSelf result(*reinterpret_cast<const TSelf*>(this));
            result.Transpose(std::forward<Args>(args)...);
            return result;
        }

        /**
         * Creates a transposed copy of the multidimensional buffer.
         *
         * @param perm Permutation of the dimensions.
         * @exception InvalidOperationException
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        TSelf Transposed(const std::array<size_t, NDimensions>& perm) const
        {
            TSelf result(*reinterpret_cast<const TSelf*>(this));
            result.Transpose(perm);
            return result;
        }

        /** Releases the resources. */
        virtual void Release()
        {
            if (this->pData != nullptr)
            {
                free(this->pData);
                this->pData = nullptr;
            }

            this->bufferSize = BUFFER_SIZE_ZERO;
            this->strides = BUFFER_SIZE_ZERO;
            this->flags = BufferFlags::None;
        }

        /** Returns an iterator to the beginning. */
        iterator begin()
        {
            return iterator(this->pData, this->flags, this->bufferSize, this->strides);
        }

        /** @copydoc begin */
        const_iterator begin() const
        {
            return const_iterator(this->pData, this->flags, this->bufferSize, this->strides);
        }

        /** @copydoc begin */
        const_iterator cbegin() const
        {
            return this->begin();
        }

        /** Returns an iterator to the end. */
        iterator end()
        {
            return this->begin() + this->Size(0);
        }

        /** @copydoc end */
        const_iterator end() const
        {
            return this->begin() + this->Size(0);
        }

        /** @copydoc end */
        const_iterator cend() const
        {
            return this->end();
        }

    protected:
        /** Calculates strides. */
        constexpr void CalcStrides()
        {
            if constexpr (NDimensions == 1) this->strides = 1;
            else
            {
                for (size_t i = 0; i < NDimensions; ++i)
                {
                    this->strides[i] = std::accumulate(this->bufferSize.begin() + i + 1, this->bufferSize.end(), (size_t)1, std::multiplies<size_t>());
                }
            }
        }

        /**
         * @copydoc ElementCount
         *
         * @param bufferSize @copydetails bufferSize
         */
        static constexpr inline size_t ElementCount(const buffer_size_t& bufferSize)
        {
            if constexpr (NDimensions == 1) return bufferSize;
            else return std::accumulate(bufferSize.begin(), bufferSize.end(), (size_t)1, std::multiplies<size_t>());
        }

        /**
         * Allocates memory.
         *
         * @param size_byte Number of bytes to allocate.
         * @param flags @copydetails flags
         * @return Pointer to the allocated memory.
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        static TData* Allocate(size_t size_bytes, Enum<BufferFlags> flags)
        {
            if (size_bytes == 0)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Size cannot be 0.");
            }

            TData* pData = reinterpret_cast<TData*>(malloc(size_bytes));
            if (pData == nullptr)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InsufficientMemoryException, HEPH_FUNC, std::format("Failed to allocate {} bytes.", size_bytes));
            }

            if (!flags.Test(BufferFlags::AllocUninitialized))
            {
                std::fill(
                    reinterpret_cast<uint8_t*>(pData),
                    reinterpret_cast<uint8_t*>(pData) + size_bytes,
                    TData()
                );
            }

            return pData;
        }

        /**
         * Reallocates memory.
         *
         * @param pData Pointer to the data that will be reallocated.
         * @param oldSize_bytes Old size in bytes.
         * @param newSize_bytes New size in bytes.
         * @param flags @copydetails flags
         * @exception InvalidArgumentException
         * @exception InsufficientMemoryException
         */
        static void Reallocate(TData*& pData, size_t oldSize_bytes, size_t newSize_bytes, Enum<BufferFlags> flags)
        {
            if (newSize_bytes == 0)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Size cannot be 0.");
            }

            TData* pTemp = reinterpret_cast<TData*>(realloc(pData, newSize_bytes));
            if (pTemp == nullptr)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InsufficientMemoryException, HEPH_FUNC, std::format("Failed to reallocate {} bytes.", newSize_bytes));
            }

            if (newSize_bytes > oldSize_bytes && !flags.Test(BufferFlags::AllocUninitialized))
            {
                std::fill(
                    reinterpret_cast<uint8_t*>(pTemp) + oldSize_bytes,
                    reinterpret_cast<uint8_t*>(pTemp) + newSize_bytes,
                    TData()
                );
            }

            pData = pTemp;
        }
    };
}

#endif
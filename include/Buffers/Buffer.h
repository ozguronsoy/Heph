#ifndef HEPH_BUFFER_H
#define HEPH_BUFFER_H

#include "HephShared.h"
#include "Enum.h"
#include "HephConcepts.h"
#include "Exceptions/InsufficientMemoryException.h"
#include "Exceptions/InvalidArgumentException.h"
#include <stdlib.h>
#include <type_traits>
#include <algorithm>

/** @file */

namespace Heph
{
    /** @brief Defines flags for buffer configuration.  */
    enum BufferFlags
    {
        /** @brief No flags are set. */
        None = 0,
        /** @brief Specifies that the elements will NOT be initialized after allocating memory. */
        AllocUninitialized = 1,
        /** @brief Specifies that the buffer is circular.  */
        Circular = 2
    };

    /**
     * @brief Container for storing big chunk of sequential data.
     *
     * @tparam TSelf Type of the derived buffer (CRTP).
     * @tparam TData Type of the stored data. Must be default constructible and trivally destructible.
     */
    template<typename TSelf, typename TData>
        requires (std::is_default_constructible_v<TData>&& std::is_trivially_destructible_v<TData>)
    class HEPH_API Buffer
    {
    protected:
        /** Pointer to the first element of the buffer, or nullptr if the buffer is empty. */
        TData* pData;

        /** Number of elements the buffer stores. */
        size_t size;

        /** Specifies the configuration flags for the buffer. */
        Enum<BufferFlags> flags;

    public:
        /**
         * @copydoc constructor
         *
         * @param size @copydetails size
         * @param flags @copydetails flags
         * @exception InsufficientMemoryException
         */
        explicit Buffer(size_t size = 0, Enum<BufferFlags> flags = BufferFlags::None)
            : pData(nullptr), size(size), flags(flags)
        {
            static_assert(std::derived_from<TSelf, Buffer<TSelf, TData>>, "TSelf must derive from Buffer<TSelf, TData>.");

            if (size > 0)
            {
                this->pData = Buffer::Allocate(size * sizeof(TData), flags);
            }
        }

        /**
         * @copydoc copy_constructor
         *
         * @exception InsufficientMemoryException
         */
        Buffer(const Buffer& rhs)
        {
            if (rhs)
            {
                this->pData = Buffer::Allocate(rhs.size * sizeof(TData), BufferFlags::None);
                (void)std::copy(rhs.begin(), rhs.end(), this->begin());

                this->size = rhs.size;
                this->flags = rhs.flags;
            }
        }

        /** @copydoc move_constructor */
        Buffer(Buffer&& rhs) noexcept
            : pData(rhs.pData), size(rhs.size), flags(rhs.flags)
        {
            rhs.pData = nullptr;
            rhs.size = 0;
            rhs.flags = BufferFlags::None;
        }

        /** @copydoc destructor */
        virtual ~Buffer()
        {
            this->Release();
        }

        /**
         * Copies rhs to current instance.
         *
         * @param rhs Buffer to copy.
         * @return Reference to current instance.
         * @exception InsufficientMemoryException
         */
        Buffer& operator=(const Buffer& rhs)
        {
            if (&rhs != this)
            {
                this->Release();

                if (rhs)
                {
                    this->pData = Buffer::Allocate(rhs.size * sizeof(TData), BufferFlags::None);
                    (void)std::copy(rhs.begin(), rhs.end(), this->begin());
                }

                this->size = rhs.size;
                this->flags = rhs.flags;
            }

            return *this;
        }

        /**
         * Moves rhs to current instance.
         *
         * @param rhs Buffer to move.
         * @return Reference to current instance.
         * @exception InsufficientMemoryException
         */
        Buffer& operator=(Buffer&& rhs) noexcept
        {
            if (&rhs != this)
            {
                this->Release();

                this->pData = rhs.pData;
                this->size = rhs.size;
                this->flags = rhs.flags;

                rhs.pData = nullptr;
                rhs.size = 0;
                rhs.flags = BufferFlags::None;
            }

            return *this;
        }

        /**
         * Checks whether the buffer is empty.
         *
         * @return true if the buffer is not empty, otherwise false.
         */
        operator bool() const
        {
            return !this->IsEmpty();
        }

        /**
         * Gets the element at the provided index.
         *
         * @param index Index of the element.
         * @return Reference to the element.
         */
        TData& operator[](size_t index)
        {
            return this->flags.Test(BufferFlags::Circular)
                ? this->pData[index % this->size]
                : this->pData[index];
        }

        /** @copydoc operator[](size_t) */
        const TData& operator[](size_t index) const
        {
            return this->flags.Test(BufferFlags::Circular)
                ? this->pData[index % this->size]
                : this->pData[index];
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

        /** Gets the number of elements stored in the buffer. */
        size_t Size() const
        {
            return this->size;
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

        /**
         * Checks whether the buffer is empty.
         *
         * @return true if the buffer is empty, otherwise false.
         */
        bool IsEmpty() const
        {
            return (this->size == 0) || (this->pData == nullptr);
        }

        /** 
         * @copydoc operator[] 
         * 
         * @exception InvalidArgumentException
         */
        TData& At(size_t index)
        {
            if (this->flags.Test(BufferFlags::Circular))
            {
                return this->pData[index % this->size];
            }

            if (index >= this->size)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "index out of bounds.");
            }

            return this->pData[index];
        }

        /** @copydoc At(size_t) */
        const TData& At(size_t index) const
        {
            if (this->flags.Test(BufferFlags::Circular))
            {
                return this->pData[index % this->size];
            }

            if (index >= this->size)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "index out of bounds.");
            }

            return this->pData[index];
        }

        /** Sets all elements to their default values. */
        virtual void Reset()
        {
            if (*this)
            {
                std::fill(this->begin(), this->end(), TData());
            }
        }

        /**
         * Changes the size of the buffer.
         * If new size is less than the old size, elements will be removed from the end.
         *
         */
        virtual void Resize(size_t newSize)
        {
            if (this->size != newSize)
            {
                if (newSize == 0)
                {
                    this->Release();
                }
                else
                {
                    Buffer::Reallocate(
                        this->pData,
                        this->size * sizeof(TData),
                        newSize * sizeof(TData),
                        this->flags
                    );
                    this->size = newSize;
                }
            }
        }

        /** Swaps the buffer elements. */
        virtual void Reverse()
        {
            const size_t halfSize = this->size / 2;
            for (size_t i = 0; i < halfSize; ++i)
            {
                std::swap((*this)[i], (*this)[this->size - i - 1]);
            }
        }

        /** Releases the resources. */
        virtual void Release()
        {
            if (this->pData != nullptr)
            {
                free(this->pData);
                this->pData = nullptr;
            }

            this->size = 0;
            this->flags = BufferFlags::None;
        }

        /** Gets the pointer to the first element if buffer is not empty, otherwise nullptr. */
        TData* begin() const
        {
            return this->pData;
        }

        /** Gets the pointer to the end of the buffer if buffer is not empty, otherwise nullptr. */
        TData* end() const
        {
            return this->pData != nullptr
                ? (this->pData + this->size)
                : nullptr;
        }

    protected:
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
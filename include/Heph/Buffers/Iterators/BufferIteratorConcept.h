#ifndef BUFFER_ITERATOR_CONCEPT_H
#define BUFFER_ITERATOR_CONCEPT_H

#include "Heph/Concepts.h"
#include <array>
#include <type_traits>

/** @file */

namespace Heph
{
    /** @brief Specifies that the type can be stored in a buffer. */
    template<typename T>
    concept BufferElement = std::is_default_constructible_v<T> && std::is_trivially_destructible_v<T>;

    /** @brief Specifies that the type ``T`` satisfies all requirements to be used as a buffer iterator. */
    template<typename T, typename TData, size_t NDimensions>
    concept BufferIteratorConcept =
        (NDimensions > 0) &&
        BufferElement<TData> &&

        Addable<T, index_t, T>&&
        AddAssignable<T, index_t>&&
        Subtractable<T, index_t, T>&&
        SubtractAssignable<T, index_t>&&

        Addable<T, typename T::buffer_index_t, T>&&
        AddAssignable<T, typename T::buffer_index_t>&&
        Subtractable<T, typename T::buffer_index_t, T>&&
        SubtractAssignable<T, typename T::buffer_index_t>&&

        requires(T it)
    {
        { *it } -> std::same_as<typename T::reference>;

        { ++it } -> std::same_as<T&>;
        { it++ } -> std::same_as<T&>;
        { --it } -> std::same_as<T&>;
        { it-- } -> std::same_as<T&>;

        { it.Indices() } -> std::convertible_to<typename T::buffer_index_t>;

        { it <=> it };
    }&&

        requires(T it, size_t dim, index_t n)
    {
        { it.IncrementIndex(dim, n) };
        { it.DecrementIndex(dim, n) };
    }&&

        requires (T::pointer& ptr, const T::buffer_size_t& size, const T::buffer_size_t& strides, const T::buffer_index_t& indices)
    {
        { T::template Get<true>(ptr, size, strides, indices) } -> std::same_as<typename T::reference>;
        { T::template Get<false>(ptr, size, strides, indices) } -> std::same_as<typename T::reference>;
    };

    /** @brief Provides common type definitions and constants for buffer iterators.  */
    template<BufferElement TData, size_t NDimensions>
    struct BufferIteratorTraits final
    {
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
    };
}

#endif
#ifndef HEPH_CONCEPTS_H
#define HEPH_CONCEPTS_H

#include "HephShared.h"
#include <concepts>

/** @file */

namespace Heph
{
    /** @brief Specifies that a type is primitive (basic). */
    template<typename T>
    concept Primitive = std::is_void_v<T> || std::integral<T> || std::floating_point<T>;

    /**
     * @brief Specifies that a valid addition operator overload exists, where the left operand is of type ``TLhs``,
     * the right operand is of type ``TRhs``, and the return value is convertible to ``TResult``.
     */
    template<typename TLhs, typename TRhs = TLhs, typename TResult = TLhs>
    concept Addable = requires(TLhs lhs, TRhs rhs)
    {
        { lhs + rhs } -> std::convertible_to<TResult>;
    };

    /**
     * @brief Specifies that a valid addition assignment operator overload exists,
     * where the left operand is of type ``TLhs`` and the right operand is of type ``TRhs``.
     */
    template<typename TLhs, typename TRhs = TLhs>
    concept AddAssignable = requires(TLhs lhs, TRhs rhs)
    {
        { lhs += rhs } -> std::same_as<TLhs&>;
    };

    /**
     * @brief Specifies that a valid subtraction operator overload exists, where the left operand is of type ``TLhs``,
     * the right operand is of type ``TRhs``, and the return value is convertible to ``TResult``.
     */
    template<typename TLhs, typename TRhs = TLhs, typename TResult = TLhs>
    concept Subtractable = requires(TLhs lhs, TRhs rhs)
    {
        { lhs - rhs } -> std::convertible_to<TResult>;
    };

    /**
     * @brief Specifies that a valid subtraction assignment operator overload exists,
     * where the left operand is of type ``TLhs`` and the right operand is of type ``TRhs``.
     */
    template<typename TLhs, typename TRhs = TLhs>
    concept SubtractAssignable = requires(TLhs lhs, TRhs rhs)
    {
        { lhs -= rhs } -> std::same_as<TLhs&>;
    };

    /**
     * @brief Specifies that a valid multiplication operator overload exists, where the left operand is of type ``TLhs``,
     * the right operand is of type ``TRhs``, and the return value is convertible to ``TResult``.
     */
    template<typename TLhs, typename TRhs = TLhs, typename TResult = TLhs>
    concept Multipliable = requires(TLhs lhs, TRhs rhs)
    {
        { lhs* rhs } -> std::convertible_to<TResult>;
    };

    /**
     * @brief Specifies that a valid multiplication assignment operator overload exists,
     * where the left operand is of type ``TLhs`` and the right operand is of type ``TRhs``.
     */
    template<typename TLhs, typename TRhs = TLhs>
    concept MultiplyAssignable = requires(TLhs lhs, TRhs rhs)
    {
        { lhs *= rhs } -> std::same_as<TLhs&>;
    };

    /**
     * @brief Specifies that a valid division operator overload exists, where the left operand is of type ``TLhs``,
     * the right operand is of type ``TRhs``, and the return value is convertible to ``TResult``.
     */
    template<typename TLhs, typename TRhs = TLhs, typename TResult = TLhs>
    concept Divisible = requires(TLhs lhs, TRhs rhs)
    {
        { lhs / rhs } -> std::convertible_to<TResult>;
    };

    /**
     * @brief Specifies that a valid division assignment operator overload exists,
     * where the left operand is of type ``TLhs`` and the right operand is of type ``TRhs``.
     */
    template<typename TLhs, typename TRhs = TLhs>
    concept DivideAssignable = requires(TLhs lhs, TRhs rhs)
    {
        { lhs /= rhs } -> std::same_as<TLhs&>;
    };

    /**
     * @brief Specifies that valid arithmetic operator overloads (``+``, ``-``, ``*``, and ``/``) exist,
     * where the left operand is of type ``TLhs``, the right operand is of type ``TRhs``,
     * and the return value is convertible to ``TResult``.
     */
    template<typename TLhs, typename TRhs = TLhs, typename TResult = TLhs>
    concept Arithmetic = Addable<TLhs, TRhs, TResult>&& Subtractable<TLhs, TRhs, TResult>&& Multipliable<TLhs, TRhs, TResult>&& Divisible<TLhs, TRhs, TResult>;

    /**
     * @brief Specifies that valid arithmetic assignment operator overloads (``+=``, ``-=``, ``*=``, and ``/=``) exist,
     * where the left operand is of type ``TLhs`` and the right operand is of type ``TRhs``
     */
    template<typename TLhs, typename TRhs = TLhs>
    concept ArithmeticAssignable = AddAssignable<TLhs, TRhs>&& SubtractAssignable<TLhs, TRhs>&& MultiplyAssignable<TLhs, TRhs>&& DivideAssignable<TLhs, TRhs>;

    /** 
     * @brief Specifies that a valid less than operator overload exists,
     * where the left operand is of type ``TLhs`` and the right operand is of type ``TRhs``.
     */
    template<typename TLhs, typename TRhs = TLhs>
    concept HasLessThan = requires(TLhs lhs, TRhs rhs)
    {
        { lhs < rhs } -> std::convertible_to<bool>;
    };

    /** 
     * @brief Specifies that a valid greater than operator overload exists,
     * where the left operand is of type ``TLhs`` and the right operand is of type ``TRhs``.
     */
    template<typename TLhs, typename TRhs = TLhs>
    concept HasGreaterThan = requires(TLhs lhs, TRhs rhs)
    {
        { lhs > rhs } -> std::convertible_to<bool>;
    };
}

#endif
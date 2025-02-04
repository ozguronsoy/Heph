#ifndef HEPH_CONCEPTS_H
#define HEPH_CONCEPTS_H

#include "HephShared.h"
#include <concepts>

/** @file */

namespace Heph
{
    /** @brief Indicates that T is a primitive (basic) type. */
    template<typename T>
    concept Primitive = std::is_void_v<T> || std::integral<T> || std::floating_point<T>;

    /** 
     * @brief Indicates that there is a valid addition operator overload such that takes an object of type ``TLhs`` as the left operand, 
     * takes an object of type ``TRhs`` as the right operand, and type of the return value is convertible to ``TResult``. 
     */
    template<typename TLhs, typename TRhs = TLhs, typename TResult = TLhs>
    concept Addable = requires(TLhs lhs, TRhs rhs)
    {
        { lhs + rhs } -> std::convertible_to<TResult>;
    };

    /** 
     * @brief Indicates that there is a valid subtraction operator overload such that takes an object of type ``TLhs`` as the left operand, 
     * takes an object of type ``TRhs`` as the right operand, and type of the return value is convertible to ``TResult``. 
     */
    template<typename TLhs, typename TRhs = TLhs, typename TResult = TLhs>
    concept Subtractable = requires(TLhs lhs, TRhs rhs)
    {
        { lhs - rhs } -> std::convertible_to<TResult>;
    };

    /** 
     * @brief Indicates that there is a valid multiplication operator overload such that takes an object of type ``TLhs`` as the left operand, 
     * takes an object of type ``TRhs`` as the right operand, and type of the return value is convertible to ``TResult``. 
     */
    template<typename TLhs, typename TRhs = TLhs, typename TResult = TLhs>
    concept Multipliable = requires(TLhs lhs, TRhs rhs)
    {
        { lhs * rhs } -> std::convertible_to<TResult>;
    };

    /** 
     * @brief Indicates that there is a valid division operator overload such that takes an object of type ``TLhs`` as the left operand, 
     * takes an object of type ``TRhs`` as the right operand, and type of the return value is convertible to ``TResult``. 
     */
    template<typename TLhs, typename TRhs = TLhs, typename TResult = TLhs>
    concept Divisible = requires(TLhs lhs, TRhs rhs)
    {
        { lhs / rhs } -> std::convertible_to<TResult>;
    };

    /** 
     * @brief Indicates that there are valid arithmetic operator overloads such that takes an object of type ``TLhs`` as the left operand, 
     * takes an object of type ``TRhs`` as the right operand, and type of the return value is convertible to ``TResult``. 
     */
    template<typename TLhs, typename TRhs = TLhs, typename TResult = TLhs>
    concept Arithmetic = Addable<TLhs, TRhs, TResult> && Subtractable<TLhs, TRhs, TResult> && Multipliable<TLhs, TRhs, TResult> && Divisible<TLhs, TRhs, TResult>;
}

#endif
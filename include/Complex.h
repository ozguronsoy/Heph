#ifndef HEPH_COMPLEX_H
#define HEPH_COMPLEX_H

#include "HephShared.h"
#include <complex>

/** @file */

namespace Heph
{
    /**
     * @brief Provides ``+=``, ``-=``, ``*=``, and ``/=`` overloads to ``std::complex``.
     * 
     * @tparam T Type of the real and imaginary parts. 
     */
    template<typename T>
    class Complex : public std::complex<T>
    {
    public:
        using std::complex<T>::complex;

        Complex operator-() const
        {
            return Complex(-this->real(), -this->imag());
        }

        Complex& operator+=(const Complex& other)
        {
            std::complex<T>::operator+=(other);
            return *this;
        }

        Complex& operator-=(const Complex& other)
        {
            std::complex<T>::operator-=(other);
            return *this;
        }

        Complex& operator*=(const Complex& other)
        {
            std::complex<T>::operator*=(other);
            return *this;
        }

        Complex& operator/=(const Complex& other)
        {
            std::complex<T>::operator/=(other);
            return *this;
        }

        Complex& operator+=(const T& val)
        {
            std::complex<T>::operator+=(val);
            return *this;
        }

        Complex& operator-=(const T& val)
        {
            std::complex<T>::operator-=(val);
            return *this;
        }

        Complex& operator*=(const T& val)
        {
            std::complex<T>::operator*=(val);
            return *this;
        }

        Complex& operator/=(const T& val)
        {
            std::complex<T>::operator/=(val);
            return *this;
        }

        // Member binary operators
        Complex operator+(const Complex& other) const
        {
            Complex result = *this;
            result += other;
            return result;
        }

        Complex operator-(const Complex& other) const
        {
            Complex result = *this;
            result -= other;
            return result;
        }

        Complex operator*(const Complex& other) const
        {
            Complex result = *this;
            result *= other;
            return result;
        }

        Complex operator/(const Complex& other) const
        {
            Complex result = *this;
            result /= other;
            return result;
        }

        Complex operator+(const T& val) const
        {
            Complex result = *this;
            result += val;
            return result;
        }

        Complex operator-(const T& val) const
        {
            Complex result = *this;
            result -= val;
            return result;
        }

        Complex operator*(const T& val) const
        {
            Complex result = *this;
            result *= val;
            return result;
        }

        Complex operator/(const T& val) const
        {
            Complex result = *this;
            result /= val;
            return result;
        }

        // Friend functions for scalar on the left-hand side
        friend Complex operator+(const T& lhs, const Complex& rhs)
        {
            Complex result = rhs;
            result += lhs;
            return result;
        }

        friend Complex operator-(const T& lhs, const Complex& rhs)
        {
            Complex result(lhs);
            result -= rhs;
            return result;
        }

        friend Complex operator*(const T& lhs, const Complex& rhs)
        {
            Complex result = rhs;
            result *= lhs;
            return result;
        }

        friend Complex operator/(const T& lhs, const Complex& rhs)
        {
            Complex result(lhs);
            result /= rhs;
            return result;
        }
    };
}

#endif
#ifndef HEPH_COMPLEX_H
#define HEPH_COMPLEX_H

#include "HephShared.h"
#include "Concepts.h"
#include "Exceptions/InvalidArgumentException.h"
#include <complex>
#include <cmath>
#include <iostream>

/** @file */

namespace Heph
{
    /**
     * @brief Struct for representing complex numbers.
     *
     * @tparam TNumber Type of the real and imaginary numbers.
     */
    template<Primitive TNumber>
        requires (!std::is_void_v<TNumber>)
    struct HEPH_API Complex
    {
        /** @brief real part of the complex number. */
        TNumber real;
        /** @brief imaginary part of the complex number. */
        TNumber imag;

        /**
         * @copydoc constructor
         *
         * @param real @copydoc real
         * @param imag @copydoc imag
         */
        constexpr Complex(TNumber real = 0.0, TNumber imag = 0.0)
            : real(real), imag(imag)
        {
        }

        /** @copydoc copy_constructor */
        constexpr Complex(const Complex& rhs)
            : Complex(rhs.real, rhs.imag)
        {
        }

        /** @copydoc copy_constructor */
        explicit constexpr Complex(const std::complex<TNumber>& rhs)
            : Complex(rhs.real(), rhs.imag())
        {
        }

        constexpr Complex& operator=(TNumber rhs)
        {
            this->real = rhs;
            this->imag = 0;
            return *this;
        }

        constexpr Complex& operator=(const Complex& rhs)
        {
            this->real = rhs.real;
            this->imag = rhs.imag;
            return *this;
        }

        constexpr Complex& operator=(const std::complex<TNumber>& rhs)
        {
            this->real = rhs.real();
            this->imag = rhs.imag();
            return *this;
        }

        /**
         * Converts Heph::Complex to std::complex.
         *
         * @returns Complex number as std::complex.
         */
        constexpr operator std::complex<TNumber>() const
        {
            return std::complex<TNumber>(this->real, this->imag);
        }

        /**
         * Gets the real or imaginary part of the complex number via index.
         *
         * @param i Index; 0 for real, 1 for imaginary.
         * @returns Reference to the real or imaginary part of the complex number.
         * @exception InvalidArgumentException
         */
        constexpr TNumber& operator[](size_t i)
        {
            switch (i)
            {
            case 0:
                return this->real;
            case 1:
                return this->imag;
            default:
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "i must be 0 (real) or 1 (imaginary).");
            }
        }

        /** @copydoc operator[](size_t) */
        constexpr const TNumber& operator[](size_t i) const
        {
            switch (i)
            {
            case 0:
                return this->real;
            case 1:
                return this->imag;
            default:
                HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "i must be 0 (real) or 1 (imaginary).");
            }
        }

        /**
         * Inverts the complex number.
         *
         * @return Inverted complex number.
         */
        constexpr Complex operator-() const
        {
            return Complex(-this->real, -this->imag);
        }

        /**
         * Adds rhs to the real part and returns in a new instance.
         *
         * @param rhs Real number to add.
         * @return Result of the operation.
         */
        constexpr Complex operator+(TNumber rhs) const
        {
            return Complex(this->real + rhs, this->imag);
        }

        /** @copydoc operator+(const Complex<TRhsNumber>&) const */
        template<Primitive TRhsNumber>
            requires (!std::is_void_v<TRhsNumber>)
        constexpr Complex operator+(const std::complex<TRhsNumber>& rhs) const
        {
            return Complex(this->real + rhs.real(), this->imag + rhs.imag());
        }

        /**
         * Adds two complex numbers.
         *
         * @tparam TRhsNumber Type of the real and imaginary numbers of rhs.
         * @param rhs Complex number to add.
         * @return Result of the operation.
         */
        template<Primitive TRhsNumber>
        constexpr Complex operator+(const Complex<TRhsNumber>& rhs) const
        {
            return Complex(this->real + rhs.real, this->imag + rhs.imag);
        }

        /**
         * Adds rhs to the real part.
         *
         * @param rhs Real number to add.
         * @return Reference to current instance.
         */
        constexpr Complex& operator+=(TNumber rhs)
        {
            this->real += rhs;
            return *this;
        }

        /** @copydoc operator+=(const Complex<TRhsNumber>&) */
        template<Primitive TRhsNumber>
            requires (!std::is_void_v<TRhsNumber>)
        constexpr Complex& operator+=(const std::complex<TRhsNumber>& rhs)
        {
            this->real += rhs.real();
            this->imag += rhs.imag();
            return *this;
        }

        /**
         * Adds two complex numbers.
         *
         * @tparam TRhsNumber Type of the real and imaginary numbers of rhs.
         * @param rhs Complex number to add.
         * @return Reference to current instance.
         */
        template<Primitive TRhsNumber>
        constexpr Complex& operator+=(const Complex<TRhsNumber>& rhs)
        {
            this->real += rhs.real;
            this->imag += rhs.imag;
            return *this;
        }

        /**
         * Subtracts rhs from the real part and returns in a new instance.
         *
         * @param rhs Real number to subtract.
         * @return Result of the operation.
         */
        constexpr Complex operator-(TNumber rhs) const
        {
            return Complex(this->real - rhs, this->imag);
        }

        /** @copydoc operator-(const Complex<TRhsNumber>&) const*/
        template<Primitive TRhsNumber>
            requires (!std::is_void_v<TRhsNumber>)
        constexpr Complex operator-(const std::complex<TRhsNumber>& rhs) const
        {
            return Complex(this->real - rhs.real(), this->imag - rhs.imag());
        }

        /**
         * Subtracts two complex numbers.
         *
         * @tparam TRhsNumber Type of the real and imaginary numbers of rhs.
         * @param rhs Complex number to subtract.
         * @return Result of the operation.
         */
        template<Primitive TRhsNumber>
        constexpr Complex operator-(const Complex<TRhsNumber>& rhs) const
        {
            return Complex(this->real - rhs.real, this->imag - rhs.imag);
        }

        /**
         * Subtracts rhs to the real part.
         *
         * @param rhs Real number to subtract.
         * @return Reference to current instance.
         */
        constexpr Complex& operator-=(TNumber rhs)
        {
            this->real -= rhs;
            return *this;
        }

        /** @copydoc operator-=(const Complex<TRhsNumber>&) */
        template<Primitive TRhsNumber>
            requires (!std::is_void_v<TRhsNumber>)
        constexpr Complex& operator-=(const std::complex<TRhsNumber>& rhs)
        {
            this->real -= rhs.real();
            this->imag -= rhs.imag();
            return *this;
        }

        /**
         * Subtracts two complex numbers.
         *
         * @tparam TRhsNumber Type of the real and imaginary numbers of rhs.
         * @param rhs Complex number to subtract.
         * @return Reference to current instance.
         */
        template<Primitive TRhsNumber>
        constexpr Complex& operator-=(const Complex<TRhsNumber>& rhs)
        {
            this->real -= rhs.real;
            this->imag -= rhs.imag;
            return *this;
        }

        /**
         * Multiplies the complex number with rhs and returns in a new instance.
         *
         * @param rhs Multiplication factor.
         * @return Result of the operation.
         */
        constexpr Complex operator*(TNumber rhs) const
        {
            return Complex(this->real * rhs, this->imag * rhs);
        }

        /** @copydoc operator*(const Complex<TRhsNumber>&) const */
        template<Primitive TRhsNumber>
            requires (!std::is_void_v<TRhsNumber>)
        constexpr Complex operator*(const std::complex<TRhsNumber>& rhs) const
        {
            return Complex(
                this->real * rhs.real() - this->imag * rhs.imag(),
                this->imag * rhs.real() + this->real * rhs.imag()
            );
        }

        /**
         * Multiplies two complex numbers.
         *
         * @tparam TRhsNumber Type of the real and imaginary numbers of rhs.
         * @param rhs Complex number to multiply.
         * @return Result of the operation.
         */
        template<Primitive TRhsNumber>
        constexpr Complex operator*(const Complex<TRhsNumber>& rhs) const
        {
            return Complex(
                this->real * rhs.real - this->imag * rhs.imag,
                this->imag * rhs.real + this->real * rhs.imag
            );
        }

        /**
         * Multiplies with the rhs.
         *
         * @param rhs Multiplication factor.
         * @return Reference to current instance.
         */
        constexpr Complex& operator*=(TNumber rhs)
        {
            this->real *= rhs;
            this->imag *= rhs;
            return *this;
        }

        /** @copydoc operator*=(const Complex<TRhsNumber>&) */
        template<Primitive TRhsNumber>
            requires (!std::is_void_v<TRhsNumber>)
        constexpr Complex& operator*=(const std::complex<TRhsNumber>& rhs)
        {
            *this = (*this) * rhs;
            return *this;
        }

        /**
         * Multiplies two complex numbers.
         *
         * @tparam TRhsNumber Type of the real and imaginary numbers of rhs.
         * @param rhs Complex number to multiply.
         * @return Reference to current instance.
         */
        template<Primitive TRhsNumber>
        constexpr Complex& operator*=(const Complex<TRhsNumber>& rhs)
        {
            *this = (*this) * rhs;
            return *this;
        }

        /**
         * Divides the complex number by rhs and returns in a new instance.
         *
         * @param rhs Division factor.
         * @return Result of the operation.
         */
        constexpr Complex operator/(TNumber rhs) const
        {
            return Complex(this->real / rhs, this->imag / rhs);
        }

        /** @copydoc operator/(const Complex<TRhsNumber>&) const */
        template<Primitive TRhsNumber>
            requires (!std::is_void_v<TRhsNumber>)
        constexpr Complex operator/(const std::complex<TRhsNumber>& rhs) const
        {
            const TRhsNumber denomiter = rhs.real() * rhs.real() + rhs.imag() * rhs.imag();
            return Complex(
                (this->real * rhs.real() + this->imag * rhs.imag()) / denomiter,
                (this->imag * rhs.real() - this->real * rhs.imag()) / denomiter
            );
        }

        /**
         * Divides two complex numbers.
         *
         * @tparam TRhsNumber Type of the real and imaginary numbers of rhs.
         * @param rhs Complex number to divide by.
         * @return Result of the operation.
         */
        template<Primitive TRhsNumber>
        constexpr Complex operator/(const Complex<TRhsNumber>& rhs) const
        {
            const TRhsNumber denomiter = rhs.MagnitudeSquared();
            return Complex(
                (this->real * rhs.real + this->imag * rhs.imag) / denomiter,
                (this->imag * rhs.real - this->real * rhs.imag) / denomiter
            );
        }

        /**
         * Divides by the rhs.
         *
         * @param rhs Division factor.
         * @return Reference to current instance.
         */
        constexpr Complex& operator/=(TNumber rhs)
        {
            this->real /= rhs;
            this->imag /= rhs;
            return *this;
        }

        /** @copydoc operator/=(const Complex<TRhsNumber>&) */
        template<Primitive TRhsNumber>
            requires (!std::is_void_v<TRhsNumber>)
        constexpr Complex& operator/=(const std::complex<TRhsNumber>& rhs)
        {
            *this = (*this) / rhs;
            return *this;
        }

        /**
         * Divides two complex numbers.
         *
         * @tparam TRhsNumber Type of the real and imaginary numbers of rhs.
         * @param rhs Complex number to divide.
         * @return Reference to current instance.
         */
        template<Primitive TRhsNumber>
        constexpr Complex& operator/=(const Complex<TRhsNumber>& rhs)
        {
            *this = (*this) / rhs;
            return *this;
        }

        /**
         * Compares two complex numbers.
         *
         * @tparam TRhsNumber Type of the real and imaginary numbers of rhs.
         * @param rhs Number to compare.
         * @return true if both numbers have the same value, otherwise false.
         */
        template<Primitive TRhsNumber>
        constexpr bool operator==(const Complex<TRhsNumber>& rhs) const
        {
            return (this->real == rhs.real) && (this->imag == rhs.imag);
        }

        /** @copydoc operator==(const Complex<TRhsNumber>&) const */
        template<Primitive TRhsNumber>
            requires (!std::is_void_v<TRhsNumber>)
        constexpr bool operator==(const std::complex<TRhsNumber>& rhs) const
        {
            return (this->real == rhs.real()) && (this->imag == rhs.imag());
        }

        /**
         * Compares two complex numbers.
         *
         * @tparam TRhsNumber Type of the real and imaginary numbers of rhs.
         * @param rhs Number to compare.
         * @return false if both numbers have the same value, otherwise true.
         */
        template<Primitive TRhsNumber>
        constexpr bool operator!=(const Complex<TRhsNumber>& rhs) const
        {
            return !((*this) == rhs);
        }

        /** @copydoc operator!=(const Complex<TRhsNumber>&) const */
        template<Primitive TRhsNumber>
            requires (!std::is_void_v<TRhsNumber>)
        constexpr bool operator!=(const std::complex<TRhsNumber>& rhs) const
        {
            return !((*this) == rhs);
        }


        /** Calculates the complex conjugate. */
        constexpr Complex Conjugate() const
        {
            return Complex(this->real, -this->imag);
        }

        /** Calculates the squared magnitude. */
        constexpr TNumber MagnitudeSquared() const
        {
            return (this->real * this->real) + (this->imag * this->imag);
        }

        /** Calculates the magnitude. */
        TNumber Magnitude() const
        {
            return sqrt(this->MagnitudeSquared());
        }

        /** Calculates the phase in radians. */
        TNumber Phase() const
        {
            return atan2(this->imag, this->real);
        }

        /**
         * Inserts the complex number to the output stream.
         *
         * @param os Output stream.
         * @param c Complex number.
         * @return The output stream.
         */
        friend std::ostream& operator<<(std::ostream& os, const Complex& c)
        {
            if (c.imag >= 0)
                os << c.real << " + " << c.imag << "j";
            else
                os << c.real << " - " << -c.imag << "j";
            return os;
        }

        /**
         * Extracts a complex number from the input stream.
         *
         * @param is Input stream.
         * @param c Complex number.
         * @return The input stream.
         */
        friend std::istream& operator>>(std::istream& is, Complex& c)
        {
            is >> c.real >> c.imag;
            return is;
        }
    };
}

/**
 * Adds complex number to the real number.
 *
 * @tparam TLhs Type of the lhs.
 * @tparam TRhsNumber Type of the real and imaginary numbers of rhs.
 * @param lhs Real number.
 * @param rhs Complex number.
 * @return Result of the operation.
 */
template<Heph::Primitive TLhs, Heph::Primitive TRhsNumber>
constexpr inline Heph::Complex<TRhsNumber> operator+(TLhs lhs, const Heph::Complex<TRhsNumber>& rhs)
{
    return rhs + lhs;
}

/**
 * Subtracts complex number from the real number.
 *
 * @tparam TLhs Type of the lhs.
 * @tparam TRhsNumber Type of the real and imaginary numbers of rhs.
 * @param lhs Real number.
 * @param rhs Complex number.
 * @return Result of the operation.
 */
template<Heph::Primitive TLhs, Heph::Primitive TRhsNumber>
constexpr inline Heph::Complex<TRhsNumber> operator-(TLhs lhs, const Heph::Complex<TRhsNumber>& rhs)
{
    return Heph::Complex<TRhsNumber>(lhs - rhs.real, -rhs.imag);
}

/**
 * Multiplies the real number with a complex number.
 *
 * @tparam TLhs Type of the lhs.
 * @tparam TRhsNumber Type of the real and imaginary numbers of rhs.
 * @param lhs Real number.
 * @param rhs Complex number.
 * @return Result of the operation.
 */
template<Heph::Primitive TLhs, Heph::Primitive TRhsNumber>
constexpr inline Heph::Complex<TRhsNumber> operator*(TLhs lhs, const Heph::Complex<TRhsNumber>& rhs)
{
    return rhs * lhs;
}

/**
 * Divides the real number by a complex number.
 *
 * @tparam TLhs Type of the lhs.
 * @tparam TRhsNumber Type of the real and imaginary numbers of rhs.
 * @param lhs Real number.
 * @param rhs Complex number.
 * @return Result of the operation.
 */
template<Heph::Primitive TLhs, Heph::Primitive TRhsNumber>
constexpr inline Heph::Complex<TRhsNumber> operator/(TLhs lhs, const Heph::Complex<TRhsNumber>& rhs)
{
    return Heph::Complex<TRhsNumber>(lhs, 0) / rhs;
}

#endif
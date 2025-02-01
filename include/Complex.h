#ifndef HEPH_COMPLEX_H
#define HEPH_COMPLEX_H

#include "HephShared.h"
#include <type_traits>
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
    template<typename TNumber>
    struct HEPH_API Complex
    {
        static_assert(std::is_integral_v<TNumber> || std::is_floating_point_v<TNumber>, "TNumber must be a primitive type.");

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
        explicit constexpr Complex(TNumber real = 0.0, TNumber imag = 0.0)
            : real(real), imag(imag)
        {
        }

        /** @copydoc copy_constructor */
        explicit constexpr Complex(const Complex& rhs)
            : Complex(rhs.real, rhs.imag)
        {
        }

        /** @copydoc copy_constructor */
        explicit constexpr Complex(const std::complex<TNumber>& rhs)
            : Complex(rhs.real(), rhs.imag())
        {
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
            switch(i)
            {
                case 0:
                    return this->real;
                case 1:
                    return this->imag;
                default:
                    // throw exception
                    throw "";
            }
        }

        /** @copydoc operator[](size_t) */
        constexpr const TNumber& operator[](size_t i) const
        {
            switch(i)
            {
                case 0:
                    return this->real;
                case 1:
                    return this->imag;
                default:
                    // throw exception
                    throw "";
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

        /** @copydoc operator+(const Complex<TRhsNumber>&) */
        template<typename TRhsNumber>
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
        template<typename TRhsNumber>
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
        template<typename TRhsNumber>
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
        template<typename TRhsNumber>
        constexpr Complex& operator+=(const Complex<TRhsNumber>& rhs)
        {
            this->real += rhs.real;
            this->imag += rhs.imag;
            return *this;
        }

        /**
         * Compares two complex numbers.
         * 
         * @tparam TRhsNumber Type of the real and imaginary numbers of rhs.
         * @param rhs Number to compare.
         * @return true if both numbers have the same value, otherwise false.
         */
        template<typename TRhsNumber>
        constexpr bool operator==(const Complex<TRhsNumber>& rhs) const
        {
            return (this->real == rhs.real) && (this->imag == rhs.imag);
        }

        /** @copydoc operator==(const Complex<TRhsNumber>&) */
        template<typename TRhsNumber>
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
        template<typename TRhsNumber>
        constexpr bool operator!=(const Complex<TRhsNumber>& rhs) const
        {
            return !((*this) == rhs);
        }

        /** @copydoc operator!=(const Complex<TRhsNumber>&) */
        template<typename TRhsNumber>
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
         * Prints the complex number to the provided output stream.
         * 
         * @param oss Output stream.
         * @param c Complex number to print.
         * @return The output stream.
         */
        friend std::ostream& operator<<(std::ostream& oss, const Complex& c)
        {
            if (c.imag >= 0)
                oss << c.real << " + " << c.imag << "j";
            else
                oss << c.real << " - " << -c.imag << "j";
            return oss;
        }
    };
}

#endif
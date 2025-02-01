#include <gtest/gtest.h>
#include "Complex.h"

using TNumber = double;
using Complex = Heph::Complex<TNumber>;

TEST(HephTest, Complex_Constructors)
{
    {
        Complex expected;
        expected.real = 0;
        expected.imag = 0;

        EXPECT_EQ(Complex(), expected);

        expected.real = 3;
        expected.imag = 4;
        EXPECT_EQ(Complex(3, 4), expected);
    }

    {
        constexpr Complex expected = Complex(3, 4);
        constexpr std::complex<TNumber> stdc =  std::complex<TNumber>(3, 4);

        EXPECT_EQ(Complex(stdc), expected);
    }
}

TEST(HephTest, Complex_AssignOp)
{
    {
        constexpr Complex expected = Complex(3, 4);

        Complex c(1, 2);
    
        EXPECT_NE(c, expected);

        c = expected;
        EXPECT_EQ(c, expected);
    }

    {
        constexpr Complex expected = Complex(3, 4);
        constexpr std::complex<TNumber> stdc = std::complex<TNumber>(3, 4);
       
        Complex c(1, 2);
        EXPECT_NE(c, expected);
        
        c = stdc;
        EXPECT_EQ(c, expected);
    }
}

TEST(HephTest, Complex_STD)
{
    constexpr std::complex<TNumber> expected_std = std::complex<TNumber>(1, 2); 
    EXPECT_EQ(Complex(expected_std), expected_std);
}

TEST(HephTest, Complex_SubscriptOp)
{
    {
        Complex c(1, 2);
        EXPECT_EQ(&c[0], &c.real);
        EXPECT_EQ(&c[1], &c.imag);
        EXPECT_ANY_THROW(c[-1]);
        EXPECT_ANY_THROW(c[3]);
    }

    {
        const Complex c(1, 2);
        EXPECT_EQ(&c[0], &c.real);
        EXPECT_EQ(&c[1], &c.imag);
        EXPECT_ANY_THROW(c[-1]);
        EXPECT_ANY_THROW(c[3]);
    }
}

TEST(HephTest, Complex_InvertOp)
{
    const Complex c(1, 2);
    EXPECT_EQ(-c, Complex(-1, -2));
}

TEST(HephTest, Complex_AddOp)
{
    Complex c1(1, 2);
        
    EXPECT_EQ(c1 + 2, Complex(3, 2));
    EXPECT_EQ(c1 + Complex(2, 3), Complex(3, 5));
    EXPECT_EQ(c1 + std::complex<TNumber>(2, 3), Complex(3, 5));

    EXPECT_EQ(c1 += 2, Complex(3, 2));
    EXPECT_EQ(c1 += Complex(1, 2), Complex(4, 4));
    EXPECT_EQ(c1 += std::complex(1, 2), Complex(5, 6));
}

TEST(HephTest, Complex_Compare)
{
    constexpr Complex c1 = Complex(1, 2);
    constexpr Complex c2 = Complex(1, 4);
    constexpr Complex c3 = Complex(3, 2);

    EXPECT_TRUE(c1 != c2);
    EXPECT_TRUE(c1 != c3);

    EXPECT_TRUE(c1 == c1);
    EXPECT_TRUE(c2 == c2);
    EXPECT_TRUE(c3 == c3);
}
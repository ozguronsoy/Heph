#ifndef HEPH_SHARED_H
#define HEPH_SHARED_H

/** @file */

#include <cstddef>

#ifndef HEPH_VERSION_MAJOR
#define HEPH_VERSION_MAJOR 0
#define HEPH_VERSION_MINOR 0
#define HEPH_VERSION_PATCH 0
#define HEPH_VERSION "v0.0.0"
#endif

#if defined(_MSVC_LANG)
#define HEPH_CPP_VERSION _MSVC_LANG
#else
#define HEPH_CPP_VERSION __cplusplus
#endif

#define HEPH_CPP_VERSION_PRE_98	(1L)
#define HEPH_CPP_VERSION_98		(199711L)
#define HEPH_CPP_VERSION_11		(201103L)
#define HEPH_CPP_VERSION_14		(201402L)
#define HEPH_CPP_VERSION_17		(201703L)
#define HEPH_CPP_VERSION_20		(202002L)
#define HEPH_CPP_VERSION_23		(202300L)

#if HEPH_CPP_VERSION < HEPH_CPP_VERSION_23
#error C++ 23 or above is required
#endif

 /** @brief Exports/imports the members, methods, and classes for shared libraries. */
#if defined(HEPH_SHARED_LIB)

#if defined(_WIN32) || defined(__CYGWIN__)

#if defined(HEPH_EXPORTS)
#define HEPH_API __declspec(dllexport)
#else
#define HEPH_API __declspec(dllimport)
#endif

#else

#if defined(HEPH_EXPORTS)
#define HEPH_API __attribute__((visibility("default")))
#else
#define HEPH_API
#endif

#endif

#else

#define HEPH_API 

#endif

 /** @brief Name of the current function. */
#if defined(__GNUC__) || defined(__MINGW32__) || defined(__CYGWIN__) || defined(__clang__) || defined(__INTEL_COMPILER)
#define HEPH_FUNC __PRETTY_FUNCTION__
#elif defined(_MSVC_LANG)
#define HEPH_FUNC __FUNCSIG__
#else
#define HEPH_FUNC __func__
#endif

/** @brief Forces compiler to inline the function. */
#if defined(_MSC_VER)
    #define HEPH_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
    #define HEPH_FORCE_INLINE inline __attribute__((always_inline))
#else
    #define HEPH_FORCE_INLINE inline
#endif

/** Deletes the copy constructor and assignment operator. */
#define HEPH_DISABLE_COPY(className)    className(const className&) = delete;               \
                                        className& operator=(const className&) = delete

/** Deletes the default and copy constructor, and assignment operator.  */
#define HEPH_DISABLE_INSTANCE(className) className() = delete; HEPH_DISABLE_COPY(className)

namespace Heph
{
    /** @brief Stores version information for run-time verification. */
    struct HEPH_API Version
    {
        /** @brief Indicates significant changes or updates. */
        unsigned int major;
        /** @brief Indicates incremental updates or improvements. */
        unsigned int minor;
        /** @brief Indicates minor changes and bug fixes. */
        unsigned int patch;
    };

    /** @copydoc Heph::Version */
    extern HEPH_API const Version version;

    /** @brief Defines the supported platforms. */
    enum Platform
    {
        /** @brief Represents the Windows operating system. */
        Windows,
        /** @brief Represents the Android operating system. */
        Android,
        /** @brief Represents the Linux operating system. */
        Linux,
        /** @brief Represents the Darwin operating system (macOS, iOS, iPadOS, etc.). */
        Darwin,
        /** @brief Fallback value for unsupported platforms. */
        Unknown = -1,

        /** @brief Represents the platform the application is currently being compiled for. */
#ifdef _WIN32
        Current = Platform::Windows,
#elif defined(__ANDROID__)
        Current = Platform::Android,
#elif defined(__linux__)
        Current = Platform::Linux,
#elif defined(__APPLE__)
        Current = Platform::Darwin,
#else
        Current = Platform::Unknown,
#endif

    };

    /** @brief Integral type for index. */
    using index_t = std::ptrdiff_t;
}

/** @defgroup default_constructor
  * Creates a new instance and initializes it with default values.
  *
 */

 /** @defgroup constructor
  * Creates a new instance and initializes it with the provided values.
  *
 */

 /** @defgroup copy_constructor
  * Creates a new instance and copies the rhs's data to it.
  *
  * @param rhs Instance whose data will be copied.
 */

 /** @defgroup move_constructor
  * Creates a new instance and moves the rhs's data to it.
  *
  * @param rhs Instance whose data will be moved.
 */

 /** @defgroup destructor
  * Releases the resources and destroys the instance.
  *
 */

#endif
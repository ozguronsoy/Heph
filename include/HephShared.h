#ifndef HEPH_SHARED_H
#define HEPH_SHARED_H

/** @file */

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
#define HEPH_CPP_VERSION_23		(202101L)

#if HEPH_CPP_VERSION < HEPH_CPP_VERSION_23
#error C++ 23 or above is required
#endif

 /** @brief exports/imports the members, methods, and classes for shared libraries. */
#if defined(HEPH_SHARED_LIB)

#if defined(_WIN32)

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

 /** @brief name of the current function. */
#if defined(__GNUC__) || defined(__MINGW32__) || defined(__clang__) || defined(__INTEL_COMPILER)
#define HEPH_FUNC __PRETTY_FUNCTION__
#elif defined(_MSVC_LANG)
#define HEPH_FUNC __FUNCSIG__
#else
#define HEPH_FUNC __func__
#endif

namespace Heph
{
    /** @brief Stores version information for run-time verification. */
    struct Version
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
}

#endif
#pragma once
#include "HephShared.h"
#include <string>

/** @file */

/** Indicates the ConsoleLogger log level. */
#define HEPH_CL_INFO 	"36"
/** Indicates the ConsoleLogger log level. */
#define HEPH_CL_WARNING "33"
/** Indicates the ConsoleLogger log level. */
#define HEPH_CL_ERROR 	"31"
/** Indicates the ConsoleLogger log level. */
#define HEPH_CL_SUCCESS "32"
/** Indicates the ConsoleLogger log level. */
#define HEPH_CL_DEBUG 	"35"

namespace Heph
{
	/** @brief Prints formatted messages to console. */
	class HEPH_API ConsoleLogger final
	{
	private:
		static constexpr const char* DEFAULT_LIB_NAME = "Heph";

#if defined(HEPH_CL_DISABLE_COLORED_OUTPUT)
		static inline bool coloredOutput = false;
#else
		static inline bool coloredOutput = true;
#endif

	public:
		HEPH_DISABLE_INSTANCE(ConsoleLogger);

	public:
		/**
		 * Prints the provided message to the console.
		 *
		 * @param message Message that will be printed.
		 * @param logLevel One of the <b>HEPH_CL_*</b> macros.
		 * @param libName Name of the library thats printing.
		 *
		 */
		static void Log(const std::string& message, const char* logLevel, const std::string& libName = DEFAULT_LIB_NAME);

		/**
		 * Prints the provided message to the console as INFO.
		 *
		 * @param message Message that will be printed.
		 * @param libName Name of the library thats printing.
		 *
		 */
		static void LogInfo(const std::string& message, const std::string& libName = DEFAULT_LIB_NAME);

		/**
		 * prints the provided message to the console as WARNING.
		 *
		 * @param message Message that will be printed.
		 * @param libName Name of the library thats printing.
		 *
		 */
		static void LogWarning(const std::string& message, const std::string& libName = DEFAULT_LIB_NAME);

		/**
		 * prints the provided message to the console as ERROR.
		 *
		 * @param message Message that will be printed.
		 * @param libName Name of the library thats printing.
		 *
		 */
		static void LogError(const std::string& message, const std::string& libName = DEFAULT_LIB_NAME);

		/**
		 * Prints the provided message to the console as SUCCESS.
		 *
		 * @param message Message that will be printed.
		 * @param libName Name of the library thats printing.
		 *
		 */
		static void LogSuccess(const std::string& message, const std::string& libName = DEFAULT_LIB_NAME);

		/**
		 * Prints the provided message to the console as DEBUG.
		 *
		 * @param message Message that will be printed.
		 * @param libName Name of the library thats printing.
		 *
		 */
		static void LogDebug(const std::string& message, const std::string& libName = DEFAULT_LIB_NAME);

		/** Enables the use of ANSI color codes. */
		static void EnableColoredOutput();

		/** Disables the use of ANSI color codes. */
		static void DisableColoredOutput();

	private:
		static std::string CurrentTimeToString();
		static std::string GetLogLevelName(const char* logLevel);
	};
}
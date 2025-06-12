#ifndef HEPH_EXCEPTION_H
#define HEPH_EXCEPTION_H

#include "HephShared.h"
#include "ICloneable.h"
#include "Event.h"
#include "ExceptionEventArgs.h"
#include <exception>
#include <string>
#include <list>
#include <memory>
#include <iostream>

/** @file */

/** Default handler for the OnException event. */
#define HEPH_EXCEPTION_DEFAULT_HANDLER	&Heph::Exception::DefaultHandler

/** Raises an exception but does not throw it. */
#define HEPH_EXCEPTION_RAISE(exType, ...) exType(__VA_ARGS__).Raise()

/** Raises and throws an exception. */
#define HEPH_EXCEPTION_RAISE_AND_THROW(exType, ...)   {                                                     \
                                                        exType __heph_ex__(__VA_ARGS__);                    \
                                                        __heph_ex__.Raise();                                \
                                                        throw __heph_ex__;                                  \
                                                    }

namespace Heph
{
    /** @brief Stores exception information. Base class for exceptions. */
    class HEPH_API Exception : public std::exception, public ICloneable
    {
    public:
        /** @brief List for storing exceptions. */
        using ExceptionList = std::list<std::unique_ptr<Exception>>;

        /** @brief Raised when an exception occurs. */
        static inline Event ExceptionEvent = Event();

    private:
        /** @brief Name of the method where the exception occurred. */
        std::string method;
        /** @brief Description of the exception. */
        std::string message;

    public:
        /**
         * @copydoc constructor
         *
         * @param method @copybrief method
         * @param message @copybrief message
         */
        Exception(const std::string& method, const std::string& message);

        /** @copydoc destructor */
        virtual ~Exception() = default;

        virtual const char* what() const noexcept override;

        virtual ICloneable* Clone() const noexcept override;

        /** Gets the formatted full message. */
        virtual std::string FormattedMessage() const noexcept;

        /** Gets the name of the exception. */
        virtual std::string Name() const noexcept;

        /** Gets the name of the method where the exception occurred. */
        const std::string& Method() const noexcept;

        /** Gets the description of the exception. */
        const std::string& Message() const noexcept;

        /** Raises the OnException event. */
        void Raise() const;

    public:
        /**
         * Gets the list of exceptions that occurred in the current thread.
         *
         * @return Reference to the exception list.
         */
        static const ExceptionList& Exceptions() noexcept;

        /** Gets the last exception that occurred in the current thread, or nullptr if no exception was raised. */
        static const Exception* LastException() noexcept;

        /** Clears the list of exceptions that occurred in the current thread. */
        static void ClearExceptions() noexcept;

        /** Default handler for the OnException event. */
        static void DefaultHandler(const EventParams& params);

    protected:
        /** Gets the thread_local exception list instance. */
        static ExceptionList& ExceptionListInstance() noexcept;

    public:
        /**
         * Inserts the exception to the output stream.
         *
         * @param os Output stream.
         * @param ex Exception.
         * @return The output stream.
         */
        friend std::ostream& operator<<(std::ostream& os, const Exception& ex);
    };
}

#endif
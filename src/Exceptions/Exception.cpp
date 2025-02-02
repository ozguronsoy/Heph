#include "Exception.h"
#include "ConsoleLogger.h"
#include <sstream>

namespace Heph
{
    Exception::Exception(const std::string& method, const std::string& message)
        : std::exception(), method(method), message(message)
    {
    }

    const char* Exception::what() const noexcept
    {
        return this->message.c_str();
    }

    std::string Exception::FormattedMessage() const noexcept
    {
        return this->Name() + "\nmethod: " + this->method + "\nmessage: " + this->message;
    }

    std::string Exception::Name() const noexcept
    {
        return "Exception";
    }

    const std::string& Exception::Method() const noexcept
    {
        return this->method;
    }

    const std::string& Exception::Message() const noexcept
    {
        return this->message;
    }

    void Exception::Raise() const
    {
        ExceptionEventArgs args(*this);
        Exception::OnException.Invoke(&args, nullptr);
        this->AddToExceptions();
    }

    void Exception::AddToExceptions() const
    {
        Exception::ExceptionListInstance().push_back(std::make_unique<Exception>(*this));
    }

    const Exception::ExceptionList& Exception::Exceptions() noexcept
    {
        return Exception::ExceptionListInstance();
    }

    const Exception* Exception::LastException() noexcept
    {
        const ExceptionList& exceptions = Exception::Exceptions();
        return (exceptions.size() > 0) ? (exceptions.back().get()) : (nullptr);
    }

    void Exception::ClearExceptions() noexcept
    {
        Exception::ExceptionListInstance().clear();
    }

    void Exception::DefaultHandler(const EventParams& params)
    {
        std::ostringstream oss;
        oss << ((ExceptionEventArgs*)params.pArgs)->exception << std::endl;
        ConsoleLogger::LogError(oss.str());
    }

    Exception::ExceptionList& Exception::ExceptionListInstance() noexcept
    {
        static thread_local ExceptionList instance;
        return instance;
    }

    std::ostream& operator<<(std::ostream& os, const Exception& ex)
    {
#ifdef __ANDROID__
        os << ex.FormattedMessage();
#else
        os << std::endl << ex.FormattedMessage();
#endif
        return os;
    }
}
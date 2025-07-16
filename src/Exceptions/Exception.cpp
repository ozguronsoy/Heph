#include "Heph/Exceptions/Exception.h"
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

    ICloneable* Exception::Clone() const noexcept
    {
        return new Exception(*this);
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
        Exception::ExceptionEvent.Invoke(&args, nullptr);
        (void)Exception::ExceptionListInstance().emplace_back(dynamic_cast<Exception*>(this->Clone()));
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

    void Exception::DefaultHandler(EventParams& params)
    {
        std::ostringstream oss;
        oss << "\033[31m" << params.Args<ExceptionEventArgs>().exception << "\033[0m" << std::endl;
    }

    Exception::ExceptionList& Exception::ExceptionListInstance() noexcept
    {
        static thread_local ExceptionList instance;
        return instance;
    }

    std::ostream& operator<<(std::ostream& os, const Exception& ex)
    {
        os << ex.FormattedMessage();
        return os;
    }
}
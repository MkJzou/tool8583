#ifndef FUNCTION__EXCEPTION_H_
#define FUNCTION__EXCEPTION_H_

#include <exception>

namespace mk {

class Exception : public std::exception {
private:
    char const* const message;

public:
    Exception() throw() : message(0) {}
    Exception(const char* msg) throw() : message(copy(msg)) {}
    Exception(Exception const& that) throw() : std::exception(that), message(copy(that.message)) {}
    ~Exception() throw() {
        if (message) {
            deleteMessage();
        }
    }
    char const* what() const throw() { return message ? message : ""; }

private:
    static char const* copy(char const*);
    void deleteMessage();
};

class ReminderException : public Exception {
public:
    ReminderException() throw() {}
    ReminderException(const char *msg) throw() : Exception(msg) {}
    ~ReminderException() throw() {}
};

class ErrorException : public Exception {
public:
    ErrorException() throw() {}
    ErrorException(const char *msg) throw() : Exception(msg) {}
    ~ErrorException() throw() {}
};

};

#endif // FUNCTION__EXCEPTION_H_

#include <ostream>

#include "exception.h"

ShipDefException::ShipDefException(const string &error)
: m_error(error)
{
}

ShipDefException::~ShipDefException() throw()
{
}

const char *ShipDefException::what() const throw()
{
    return m_error.c_str();
}

std::ostream &operator<<(std::ostream &os, const ShipDefException &ex)
{
    os << "Ship definition error: " << ex.m_error;
    return os;
}

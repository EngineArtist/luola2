#ifndef LUOLA_SHIP_EXCEPTION_H
#define LUOLA_SHIP_EXCEPTION_H

#include <string>
#include <exception>

using std::string;

/**
 * This exception signals an error in ship or ship component configuration.
 */
class ShipDefException : public std::exception
{
    public:
    ShipDefException(const string &error);
    ~ShipDefException() throw();

    const char *what() const throw();
    friend std::ostream& operator<<(std::ostream&, const ShipDefException&);
private:
    string m_error;
};

#endif

//
// This file is part of Luola2.
// Copyright (C) 2012 Calle Laakkonen
//
// Luola2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Luola2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Luola2.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef LUOLA_SHIP_EXCEPTION_H
#define LUOLA_SHIP_EXCEPTION_H

#include <string>
#include <exception>

using std::string;

/**
 * This exception signals an error in level loading.
 */
class LevelException : public std::exception
{
    public:
    LevelException(const string &error);
    ~LevelException() throw();

    const char *what() const throw();
    friend std::ostream& operator<<(std::ostream&, const LevelException&);
private:
    string m_error;
};

#endif

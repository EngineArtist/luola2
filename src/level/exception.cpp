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
#include <ostream>

#include "exception.h"

LevelException::LevelException(const string &error)
: m_error(error)
{
}

LevelException::~LevelException() throw()
{
}

const char *LevelException::what() const throw()
{
    return m_error.c_str();
}

std::ostream &operator<<(std::ostream &os, const LevelException &ex)
{
    os << "Level loading error: " << ex.m_error;
    return os;
}


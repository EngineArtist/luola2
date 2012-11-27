//
// This file is part of Luola2.
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
#ifndef LUOLA_UTIL_OPTIONAL_H
#define LUOLA_UTIL_OPTIONAL_H

template<typename Type>
class Optional {
	public:
		Optional() : m_hasvalue(false) { }
		Optional(Type value) : m_hasvalue(true), m_value(value) { }

		bool hasValue() const { return m_hasvalue; }
		Type get() const { return m_value; }
		Type get(Type def) const { return m_hasvalue ? m_value : def; }

		void assign(Type &ref) const {
			if(m_hasvalue)
				ref = m_value;
		}

	private:
		bool m_hasvalue;
		Type m_value;
};

#endif


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


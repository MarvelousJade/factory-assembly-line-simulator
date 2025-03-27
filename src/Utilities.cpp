#include "Utilities.h"
#include <cstddef>

namespace seneca {
	char Utilities::m_delimiter = ',';

	Utilities::Utilities() : m_widthFiled(1) {};

	void Utilities::setFieldWidth(size_t newWidth) {
		m_widthFiled = newWidth;
	};	

	size_t Utilities::getFieldWidth() const {
		return m_widthFiled;
	};

	std::string Utilities::extractToken(const std::string& str, size_t& next_pos, bool& more) {
		if (next_pos >= str.size()) {
			more = false;
			return "";
		}

		size_t pos = str.find(m_delimiter, next_pos);	
		if (pos == next_pos) {
			more = false;
			throw std::string("a delimiter is found at `next_pos`.");
		};

		std::string token = str.substr(next_pos, pos - next_pos);		
		next_pos = (pos == std::string::npos) ? str.size() : pos + 1;
		more = (next_pos < str.size());

		token.erase(0, token.find_first_not_of(" "));
		token.erase(token.find_last_not_of(" ") + 1);
		
		if (m_widthFiled < token.size()) m_widthFiled = token.size();

		return token;
	};

	void Utilities::setDelimiter(char newDelimiter) {
		m_delimiter = newDelimiter;
	};

	char Utilities::getDelimiter() {
		return m_delimiter;
	};
}

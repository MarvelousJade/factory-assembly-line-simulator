#ifndef SENECA_UTILITIES_H
#define SENECA_UTILITIES_H

#include <cstddef>
#include <string>

namespace seneca {
	class Utilities {
		size_t m_widthFiled;
		static char m_delimiter;
	public:
		Utilities();
		void setFieldWidth(size_t newWidth);	
		size_t getFieldWidth() const;
		std::string extractToken(const std::string& str, size_t& next_pos, bool& more);
		static void setDelimiter(char newDelimiter);
		static char getDelimiter();
	};
}

#endif

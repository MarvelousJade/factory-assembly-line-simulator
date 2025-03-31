#include <string>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "Station.h"
#include "Utilities.h"

namespace seneca {

	size_t Station::m_widthField = 0;
	int Station::id_generator = 0;

	Station::Station(const std::string& record) {
		Utilities util;
		size_t next_pos = 0;
		bool more = true;

		m_id = ++id_generator;
		m_itemName = util.extractToken(record, next_pos, more);
		m_serialNumber = std::stoi(util.extractToken(record, next_pos, more));
		m_quantity = std::stoi(util.extractToken(record, next_pos, more));
		m_description = util.extractToken(record, next_pos, more, false);
		
		m_widthField = std::max(m_widthField, util.getFieldWidth());
	};

	const std::string& Station::getItemName() const {
		return m_itemName;
	};

	size_t Station::getNextSerialNumber() {
		return m_serialNumber++;	
	};

	size_t Station::getQuantity() const {
		return m_quantity;
	};

	void Station::updateQuantity() {
		if (m_quantity > 0)
			m_quantity--;
	};

	void Station::display(std::ostream& os, bool full) const {
		// ID: 3 characters, right justified, zero-padded
		os << std::right << std::setw(3) << std::setfill('0') << m_id << " | ";
		// Name: left justified using the maximum field width
		os << std::left << std::setw(m_widthField) << std::setfill(' ') << m_itemName << " | ";
		// Serial: 6 characters, right justified, zero-padded
		os << std::right << std::setw(6) << std::setfill('0') << m_serialNumber << " | ";

		if (full) {
		    // Quantity: 4 characters, right justified
		    os << std::right << std::setw(4) << std::setfill(' ') << m_quantity << " | ";
		    // Description: no specific formatting
		    os << m_description;
		}
		os << std::endl;
	};
}

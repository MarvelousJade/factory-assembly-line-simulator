#include <cstddef>
#include <iomanip>
#include <iostream>
#include "CustomerOrder.h"
#include "Utilities.h"

namespace seneca {
	size_t CustomerOrder::m_widthField = 0;

	CustomerOrder::CustomerOrder() : m_cntItem(0), m_lstItem(nullptr) {};

	CustomerOrder::CustomerOrder(const std::string& record) {
		Utilities util;
		size_t next_pos = 0;
		bool more = true;

		m_name = util.extractToken(record, next_pos, more);
		m_product = util.extractToken(record, next_pos, more);

		size_t temp_pos = next_pos;
		m_cntItem = 0;

		while (more) {
			util.extractToken(record, temp_pos, more);
			m_cntItem++;
		};
		
		m_lstItem = new Item*[m_cntItem];
		more = true;
		for (size_t i = 0; i < m_cntItem; i++) {
			m_lstItem[i] = new Item(util.extractToken(record, next_pos, more));	
		};

		if(CustomerOrder::m_widthField < util.getFieldWidth()) 
			CustomerOrder::m_widthField = util.getFieldWidth();
	};

	CustomerOrder::CustomerOrder(const CustomerOrder& src) {
		throw "Don't copy me plz!";
	};

	CustomerOrder::CustomerOrder(CustomerOrder&& src) noexcept {
		if (this != &src) {
			m_name = std::move(src.m_name);
			m_product = std::move(src.m_product);
			m_cntItem = src.m_cntItem;
			m_lstItem = src.m_lstItem;

			src.m_lstItem = nullptr;
			src.m_name = "";
			src.m_product = "";
			src.m_cntItem = 0;
		}; 
	};

	CustomerOrder& CustomerOrder::operator=(CustomerOrder&& src) noexcept {
		if (this != &src) {
			m_name = std::move(src.m_name);
			m_product = std::move(src.m_product);
			m_cntItem = src.m_cntItem;
			m_lstItem = src.m_lstItem;

			src.m_lstItem = nullptr;
			src.m_name = "";
			src.m_product = "";
			src.m_cntItem = 0;
		};	
		return *this;
	};

	CustomerOrder::~CustomerOrder() {
		if (m_lstItem) {
			for (size_t i = 0; i < m_cntItem; i++) {
				delete m_lstItem[i];
			};
			delete[] m_lstItem;
		};

		m_name = "";
		m_product = "";
		m_cntItem = 0;

	};

	bool CustomerOrder::isOrderFilled() const {
		bool isOrderFilled = true;
		for (size_t i = 0; i < m_cntItem; i++) {
			if (!m_lstItem[i]->m_isFilled) {
				isOrderFilled = false;
			} 	
		};
		return isOrderFilled;
	};

	bool CustomerOrder::isItemFilled(const std::string& itemName) const {
		bool isItemFilled = true;
		for (size_t i = 0; i < m_cntItem; i++) {
			if (m_lstItem[i]->m_itemName == itemName && !m_lstItem[i]->m_isFilled) {
				isItemFilled = false;
			} 	
		};
		return isItemFilled;
	};

	void CustomerOrder::fillItem(Station& station, std::ostream& os) { 
		for (size_t i = 0; i < m_cntItem; i++) {
			if (m_lstItem[i]->m_itemName == station.getItemName() && station.getQuantity() > 0) {
				m_lstItem[i]->m_serialNumber =  station.getNextSerialNumber();		
				m_lstItem[i]->m_isFilled = true;
				station.updateQuantity();
				os << "    Filled " << m_name << ", " << m_product << " " << "[" << m_lstItem[i]->m_itemName << "]" << std::endl; 
			};
		};	
	};

	void CustomerOrder::display(std::ostream& os) const {
		os << m_name << " - " << "PRODUCT" << std::endl;
		for (size_t i = 0; i < m_cntItem; i++) {
			os << std::right << "[" << std::setw(6) << std::setfill('0') << m_lstItem[i]->m_serialNumber << "]" << " ";
			os<< std::left << std::setw(CustomerOrder::m_widthField) << std::setfill(' ') << m_lstItem[i]->m_itemName << " - ";		

			if(m_lstItem[i]->m_isFilled) {
				os << "FILLED" << std::endl;
			} else {
				os << "TO BE FILLED" << std::endl;
			};
		};
	};
}

#include <iostream>
#include "Workstation.h"

namespace seneca {
	std::deque<CustomerOrder> g_pending;
	std::deque<CustomerOrder> g_completed;
	std::deque<CustomerOrder> g_incomplete;

	Workstation::Workstation(const std::string& record) : Station(record), m_pNextStaion(nullptr) {};

	void Workstation::fill(std::ostream& os) {
		if (!m_orders.empty()) {
			m_orders.front().fillItem(*this, os);
		};
	};

	bool Workstation::attemptToMoveOrder() {
		bool isMoved = false;

		if(!m_orders.empty()) {
			bool isDone = m_orders.front().isItemFilled(this->getItemName()) || this->getQuantity() < 1;

			if(isDone) {
				if (m_pNextStaion) {
					*m_pNextStaion += std::move(m_orders.front());
					m_orders.pop_front();
					isMoved = true;		
				} else {
					if (m_orders.front().isOrderFilled()) {
					g_completed.push_back(std::move(m_orders.front()));
					} else {
						g_incomplete.push_back(std::move(m_orders.front()));
					};
					
					isMoved = true;
					m_orders.pop_front();
				};
			};
		};

		return isMoved;
	};

	void Workstation::setNextStation(Workstation* station) {
		m_pNextStaion = station;
	};

	Workstation* Workstation::getNextStation() const {
		return m_pNextStaion;
	};

	void Workstation::display(std::ostream& os) const {
		if (m_pNextStaion) {
			os << this->getItemName() << m_pNextStaion->getItemName() << std::endl;
		} else {
			os << this->getItemName() << "End of Line"<< std::endl;
		}
	};

	Workstation& Workstation::operator+=(CustomerOrder&& newOrder) {
		m_orders.push_back(std::move(newOrder));
		return *this;
	};
}

#ifndef SENECA_WORKSTATION_H
#define SENECA_WORKSTATION_H

#include <cstddef>
#include <deque>
#include "CustomerOrder.h"
#include "Station.h"

namespace seneca {
	extern std::deque<CustomerOrder> g_pending;
	extern std::deque<CustomerOrder> g_completed;
	extern std::deque<CustomerOrder> g_incomplete;

	class Workstation : public Station {
		std::deque<CustomerOrder> m_orders;
		Workstation* m_pNextStaion;
	public:
		Workstation(const std::string&);
		Workstation(const Workstation&) = delete;
		Workstation& operator=(Workstation&) = delete;
		Workstation(const Workstation&&) = delete;
		Workstation& operator=(Workstation&&) = delete;
		void fill(std::ostream& os);
		bool attemptToMoveOrder();
		void setNextStation(Workstation* station = nullptr);
		Workstation* getNextStation() const;
		void display(std::ostream& os) const;
		Workstation& operator+=(CustomerOrder&& newOrder);
	};
}

#endif

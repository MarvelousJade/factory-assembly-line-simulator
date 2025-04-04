#include <cstddef>
#include <fstream>
#include <algorithm>
#include <utility>
#include <vector>
#include "LineManager.h"
#include "Utilities.h"
#include "Workstation.h"

namespace seneca {
	static size_t runCount = 0;

	LineManager::LineManager(const std::string& file, const std::vector<Workstation*>& stations) {
		std::ifstream ifile(file);
		if(!ifile.is_open()) throw std::string("Can't throw file.") + file;

		Utilities util;
		std::string line;
		m_firstStation = nullptr;

		while(getline(ifile, line)) {
			if (line.empty()) continue;

			size_t next_pos = 0;
			bool more = true;
			Workstation* nextStationPtr = nullptr;
			Workstation* firstStationPtr = nullptr;
			
			std::string firstStation =  util.extractToken(line, next_pos, more);
			std::string nextStation =  util.extractToken(line, next_pos, more);
			
			if (!firstStation.empty()) {
				auto it = find_if(stations.begin(), stations.end(), [&](Workstation* station){
				return station->getItemName() == firstStation; 
				});

				if (it != stations.end()) {
					firstStationPtr = *it;
				};

				if (firstStationPtr) {
					bool isExisted = any_of(m_activeLine.begin(), m_activeLine.end(), [&](Workstation* station) {
					return firstStationPtr->getItemName() == station->getItemName();
					});

					if (!isExisted && firstStationPtr) {
						m_activeLine.push_back(firstStationPtr);
					};
				};

				it = find_if(stations.begin(), stations.end(), [&](Workstation* station){
						return station->getItemName() == nextStation; 
					});

				if (it != stations.end()) {
						nextStationPtr = *it;	
				};

				if (nextStationPtr) {
					bool isExisted = any_of(m_activeLine.begin(), m_activeLine.end(), [&](Workstation* station) {
						return nextStationPtr->getItemName() == station->getItemName();
						});
					
					

					if (!isExisted && nextStationPtr) {
						m_activeLine.push_back(nextStationPtr);
					};	
				};

				if(firstStationPtr) firstStationPtr->setNextStation(nextStationPtr);

			};
		};

		ifile.close();

		for(auto& station : m_activeLine) {
			bool isInNextStation = false;
			
			for(auto& other : m_activeLine) {
				if(other->getNextStation() && station->getItemName() == other->getNextStation()->getItemName()) isInNextStation =true;
			};	

			if(!isInNextStation) m_firstStation = station;
		};

		m_cntCustomerOrder = g_pending.size();	
	};	

	void LineManager::reorderStations() {
		std::vector<Workstation*> orderedStation;
		Workstation* current = m_firstStation;

		while(current) {
			orderedStation.push_back(current);
			current = current->getNextStation();
		}
		
		m_activeLine = std::move(orderedStation);
	};

	bool LineManager::run(std::ostream& os) {
		static size_t iteration = 0;

		os << "Line Manager Iteration: " << ++runCount << std::endl;	

		if (!g_pending.empty()) {
			(*m_firstStation) += std::move(g_pending.front());
			g_pending.pop_front();

		}; 
		
		std::for_each(m_activeLine.begin(), m_activeLine.end(), 
			[&](Workstation* station) {
				station->fill(os);
			});
		
		std::for_each(m_activeLine.begin(), m_activeLine.end(), 
			[](Workstation* station) {
				station->attemptToMoveOrder();				
			});

		return g_completed.size() + g_incomplete.size() == m_cntCustomerOrder;
	};

	void LineManager::display(std::ostream& os) const {
		for (auto& station : m_activeLine) {
			std::string nextStationName;
			Workstation* nextStationPtr = station->getNextStation();	

			if (nextStationPtr) {
				nextStationName = nextStationPtr->getItemName(); 
			} else {
				nextStationName = "End of Line";
			};

			os << station->getItemName() << " --> " << nextStationName << std::endl;
		};
	};
}

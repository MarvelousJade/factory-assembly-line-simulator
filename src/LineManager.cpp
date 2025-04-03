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
			
			auto it = find_if(stations.begin(), stations.end(), [&](Workstation* station){
				return station->getItemName() == firstStation; 
			});

			if (it != stations.end()) {
				firstStationPtr = *it;
				m_activeLine.push_back(firstStationPtr);
			};

			it = find_if(stations.begin(), stations.end(), [&](Workstation* station){
				return station->getItemName() == nextStation; 
			});
			
			if (it != stations.end()) {
				nextStationPtr = *it;
				m_activeLine.push_back(nextStationPtr);
			};

			firstStationPtr->setNextStation(nextStationPtr);
		};

		m_cntCustomerOrder = g_pending.size();	
	};	

	void LineManager::reorderStations() {
		std::sort(m_activeLine.begin(), m_activeLine.end(),
			[](Workstation* a, Workstation* b){
				return a->getNextStation() == b;	
			});
	};

	bool LineManager::run(std::ostream& os) {
		bool isFinished = false;	
		runCount++;

		os << "Line Manager Iteration: " << runCount << std::endl;	
		
		(*m_firstStation) += std::move(g_pending.front());
		g_pending.pop_front();

		std::for_each(m_activeLine.begin(), m_activeLine.end(), 
			[&](Workstation* station) {
				station->fill(os);
				isFinished = station->attemptToMoveOrder();	
			});

		return isFinished;
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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "Station.h"
#include "Workstation.h"
#include "CustomerOrder.h"
#include "Utilities.h"
#include "LineManager.h"

using namespace seneca;

void displayMenu() {
    std::cout << "\n==== Factory Assembly Line Manager ====\n";
    std::cout << "1. Load station data files\n";
    std::cout << "2. Load customer orders\n";
    std::cout << "3. Configure assembly line\n";
    std::cout << "4. Display stations (summary)\n";
    std::cout << "5. Display stations (detailed)\n";
    std::cout << "6. Display customer orders\n";
    std::cout << "7. Display assembly line configuration\n";
    std::cout << "8. Run simulation\n";
    std::cout << "9. Display completed orders\n";
    std::cout << "10. Display incomplete orders\n";
    std::cout << "11. Display remaining inventory\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter your choice: ";
}

int main(int argc, char* argv[]) {
    std::vector<Workstation*> stations;
    LineManager* lineManager = nullptr;
    bool stationsLoaded = false;
    bool ordersLoaded = false;
    bool lineConfigured = false;
    bool simulationRun = false;
    
    std::cout << "Factory Assembly Line Simulator\n";
    std::cout << "================================\n";
    
    if (argc > 1) {
        std::cout << "Command line arguments detected. Running in batch mode...\n";
        
        if (argc != 5) {
            std::cerr << "Error: Batch mode requires 4 arguments:\n";
            std::cerr << "  1. Stations file 1 (comma-delimited)\n";
            std::cerr << "  2. Stations file 2 (pipe-delimited)\n";
            std::cerr << "  3. Customer orders file\n";
            std::cerr << "  4. Assembly line configuration file\n";
            return 1;
        }
        
        try {
            // Load stations
            Utilities::setDelimiter(',');
            std::ifstream file1(argv[1]);
            if (!file1) {
                throw std::string("Unable to open ") + argv[1];
            }
            
            std::string record;
            while (std::getline(file1, record)) {
                stations.push_back(new Workstation(record));
            }
            file1.close();
            
            Utilities::setDelimiter('|');
            std::ifstream file2(argv[2]);
            if (!file2) {
                throw std::string("Unable to open ") + argv[2];
            }
            
            while (std::getline(file2, record)) {
                stations.push_back(new Workstation(record));
            }
            file2.close();
            
            // Load customer orders
            std::ifstream orderFile(argv[3]);
            if (!orderFile) {
                throw std::string("Unable to open ") + argv[3];
            }
            
            while (std::getline(orderFile, record)) {
                CustomerOrder order(record);
                g_pending.push_back(std::move(order));
            }
            orderFile.close();
            
            // Configure assembly line
            lineManager = new LineManager(argv[4], stations);
            lineManager->reorderStations();
            
            // Run simulation
            std::cout << "\nRunning simulation...\n";
            while (!lineManager->run(std::cout));
            
            // Display results
            std::cout << "\nCompleted Orders:\n";
            for (const auto& order : g_completed) {
                order.display(std::cout);
            }
            
            std::cout << "\nIncomplete Orders:\n";
            for (const auto& order : g_incomplete) {
                order.display(std::cout);
            }
            
            std::cout << "\nRemaining Inventory:\n";
            for (const auto* station : stations) {
                station->Station::display(std::cout, true);
            }
        }
        catch (const std::string& msg) {
            std::cerr << "Error: " << msg << std::endl;
            return 2;
        }
    }
    else {
        // Interactive mode
        int choice;
        do {
            displayMenu();
            std::cin >> choice;
            std::cin.ignore();
            
            try {
                switch (choice) {
                    case 1: {
                        // Load station data files
                        if (stationsLoaded) {
                            std::cout << "Stations are already loaded. Clear them first.\n";
                            break;
                        }
                        
                        std::string stationFile1, stationFile2;
                        char delimiter1, delimiter2 = '\0';
                        
                        std::cout << "Enter first station file name: ";
                        std::getline(std::cin, stationFile1);
                        std::cout << "Enter delimiter for first file: ";
                        std::cin >> delimiter1;
                        std::cin.ignore();
                        
                        std::cout << "Enter second station file name (or press Enter to skip): ";
                        std::getline(std::cin, stationFile2);
                        
                        if (!stationFile2.empty()) {
                            std::cout << "Enter delimiter for second file: ";
                            std::cin >> delimiter2;
                            std::cin.ignore();
                        }
                        
                        // Load stations from the first file
                        Utilities::setDelimiter(delimiter1);
                        std::ifstream file1(stationFile1);
                        if (!file1) {
                            throw std::string("Unable to open ") + stationFile1;
                        }
                        
                        std::string record;
                        while (std::getline(file1, record)) {
                            stations.push_back(new Workstation(record));
                        }
                        file1.close();
                        
                        // Load stations from the second file if provided
                        if (!stationFile2.empty()) {
                            Utilities::setDelimiter(delimiter2);
                            std::ifstream file2(stationFile2);
                            if (!file2) {
                                throw std::string("Unable to open ") + stationFile2;
                            }
                            
                            while (std::getline(file2, record)) {
                                stations.push_back(new Workstation(record));
                            }
                            file2.close();
                        }
                        
                        stationsLoaded = true;
                        std::cout << "Stations loaded successfully.\n";
                        break;
                    }
                    case 2: {
                        // Load customer orders
                        if (!stationsLoaded) {
                            std::cout << "Please load stations first.\n";
                            break;
                        }
                        
                        if (ordersLoaded) {
                            std::cout << "Orders are already loaded. Clear them first.\n";
                            break;
                        }
                        
                        std::string orderFile;
                        char delimiter;
                        
                        std::cout << "Enter order file name: ";
                        std::getline(std::cin, orderFile);
                        std::cout << "Enter delimiter for order file: ";
                        std::cin >> delimiter;
                        std::cin.ignore();
                        
                        Utilities::setDelimiter(delimiter);
                        std::ifstream file(orderFile);
                        if (!file) {
                            throw std::string("Unable to open ") + orderFile;
                        }
                        
                        std::string record;
                        while (std::getline(file, record)) {
                            CustomerOrder order(record);
                            g_pending.push_back(std::move(order));
                        }
                        file.close();
                        
                        ordersLoaded = true;
                        std::cout << "Orders loaded successfully.\n";
                        break;
                    }
                    case 3: {
                        // Configure assembly line
                        if (!stationsLoaded || !ordersLoaded) {
                            std::cout << "Please load stations and orders first.\n";
                            break;
                        }
                        
                        if (lineConfigured) {
                            std::cout << "Assembly line is already configured. Clear it first.\n";
                            break;
                        }
                        
                        std::string configFile;
                        std::cout << "Enter assembly line configuration file name: ";
                        std::getline(std::cin, configFile);
                        
                        lineManager = new LineManager(configFile, stations);
                        lineManager->reorderStations();
                        
                        lineConfigured = true;
                        std::cout << "Assembly line configured successfully.\n";
                        break;
                    }
                    case 4: {
                        // Display stations (summary)
                        if (!stationsLoaded) {
                            std::cout << "Please load stations first.\n";
                            break;
                        }
                        
                        std::cout << "\n========================================\n";
                        std::cout << "=         Stations (summary)           =\n";
                        std::cout << "========================================\n";
                        
                        for (const auto* station : stations) {
                            station->Station::display(std::cout, false);
                        }
                        break;
                    }
                    case 5: {
                        // Display stations (full)
                        if (!stationsLoaded) {
                            std::cout << "Please load stations first.\n";
                            break;
                        }
                        
                        std::cout << "\n========================================\n";
                        std::cout << "=          Stations (full)             =\n";
                        std::cout << "========================================\n";
                        
                        for (const auto* station : stations) {
                            station->Station::display(std::cout, true);
                        }
                        break;
                    }
                    case 6: {
                        // Display customer orders
                        if (!ordersLoaded) {
                            std::cout << "Please load orders first.\n";
                            break;
                        }
                        
                        std::cout << "\n========================================\n";
                        std::cout << "=                Orders                =\n";
                        std::cout << "========================================\n";
                        
                        for (const auto& order : g_pending) {
                            order.display(std::cout);
                        }
                        break;
                    }
                    case 7: {
                        // Display assembly line configuration
                        if (!lineConfigured) {
                            std::cout << "Please configure assembly line first.\n";
                            break;
                        }
                        
                        std::cout << "\n========================================\n";
                        std::cout << "=      Display Stations (ordered)      =\n";
                        std::cout << "========================================\n";
                        
                        lineManager->display(std::cout);
                        break;
                    }
                    case 8: {
                        // Run simulation
                        if (!lineConfigured) {
                            std::cout << "Please configure assembly line first.\n";
                            break;
                        }
                        
                        std::cout << "\n========================================\n";
                        std::cout << "=           Filling Orders             =\n";
                        std::cout << "========================================\n";
                        
                        while (!lineManager->run(std::cout));
                        
                        simulationRun = true;
                        std::cout << "Simulation completed.\n";
                        break;
                    }
                    case 9: {
                        // Display completed orders
                        if (!simulationRun) {
                            std::cout << "Please run the simulation first.\n";
                            break;
                        }
                        
                        std::cout << "\n========================================\n";
                        std::cout << "=      Processed Orders (complete)     =\n";
                        std::cout << "========================================\n";
                        
                        for (const auto& order : g_completed) {
                            order.display(std::cout);
                        }
                        break;
                    }
                    case 10: {
                        // Display incomplete orders
                        if (!simulationRun) {
                            std::cout << "Please run the simulation first.\n";
                            break;
                        }
                        
                        std::cout << "\n========================================\n";
                        std::cout << "=     Processed Orders (incomplete)    =\n";
                        std::cout << "========================================\n";
                        
                        for (const auto& order : g_incomplete) {
                            order.display(std::cout);
                        }
                        break;
                    }
                    case 11: {
                        // Display remaining inventory
                        if (!simulationRun) {
                            std::cout << "Please run the simulation first.\n";
                            break;
                        }
                        
                        std::cout << "\n========================================\n";
                        std::cout << "=          Inventory (full)            =\n";
                        std::cout << "========================================\n";
                        
                        for (const auto* station : stations) {
                            station->Station::display(std::cout, true);
                        }
                        break;
                    }
                    case 0:
                        // Exit
                        std::cout << "Exiting program...\n";
                        break;
                    default:
                        std::cout << "Invalid choice. Please try again.\n";
                }
            }
            catch (const std::string& msg) {
                std::cout << "Error: " << msg << std::endl;
            }
        } while (choice != 0);
    }
    
    // Clean up resources
    delete lineManager;
    for (auto* station : stations) {
        delete station;
    }
    
    return 0;
}

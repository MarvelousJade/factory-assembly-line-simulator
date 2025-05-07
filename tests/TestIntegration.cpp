#include "doctest.h"
#include "LineManager.h"
#include "Workstation.h"
#include "CustomerOrder.h"
#include "Station.h"
#include "Utilities.h"
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace seneca;

// Helper functions for integration tests
void clearGlobalQueuesForIntegration() {
    while (!g_pending.empty()) g_pending.pop_front();
    while (!g_completed.empty()) g_completed.pop_front();
    while (!g_incomplete.empty()) g_incomplete.pop_front();
}

std::string createTempStationFile(const std::string& content) {
    std::string filename = "temp_stations.txt";
    std::ofstream file(filename);
    file << content;
    file.close();
    return filename;
}

std::string createTempOrderFile(const std::string& content) {
    std::string filename = "temp_orders.txt";
    std::ofstream file(filename);
    file << content;
    file.close();
    return filename;
}

std::string createTempAssemblyFile(const std::string& content) {
    std::string filename = "temp_assembly.txt";
    std::ofstream file(filename);
    file << content;
    file.close();
    return filename;
}

void loadStations(const std::string& filename, char delimiter, std::vector<Workstation*>& stations) {
    Utilities::setDelimiter(delimiter);
    std::ifstream file(filename);
    std::string record;
    
    while (std::getline(file, record)) {
        stations.push_back(new Workstation(record));
    }
}

void loadOrders(const std::string& filename, char delimiter) {
    Utilities::setDelimiter(delimiter);
    std::ifstream file(filename);
    std::string record;
    
    while (std::getline(file, record)) {
        CustomerOrder order(record);
        g_pending.push_back(std::move(order));
    }
}

void cleanupFiles(const std::vector<std::string>& files) {
    for (const auto& file : files) {
        std::remove(file.c_str());
    }
}

void cleanupStations(std::vector<Workstation*>& stations) {
    for (auto& station : stations) {
        delete station;
    }
    stations.clear();
}

TEST_CASE("Integration - Simple End-to-End Process") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    
    clearGlobalQueuesForIntegration();
    
    // Create test files
    std::string stationFile = createTempStationFile(
        "Desk,100000,5,Office desk\n"
        "Chair,200000,5,Office chair"
    );
    
    std::string orderFile = createTempOrderFile(
        "John|Office Setup|Desk|Chair"
    );
    
    std::string assemblyFile = createTempAssemblyFile(
        "Desk|Chair\n"
        "Chair"
    );
    
    // Load stations
    std::vector<Workstation*> stations;
    loadStations(stationFile, ',', stations);
    
    // Load orders
    loadOrders(orderFile, '|');
    
    // Create line manager
    LineManager lineManager(assemblyFile, stations);
    
    // Run the simulation
    std::ostringstream oss;
    bool completed = false;
    int iterations = 0;
    const int MAX_ITERATIONS = 10; // Prevent infinite loops in tests
    
    while (!completed && iterations < MAX_ITERATIONS) {
        completed = lineManager.run(oss);
        iterations++;
    }
    
    // Verify the results
    CHECK_EQ(g_completed.size(), 1u);
    CHECK_EQ(g_incomplete.size(), 0u);
    
    // Cleanup
    cleanupFiles({stationFile, orderFile, assemblyFile});
    cleanupStations(stations);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Integration - Multi-Order Multi-Station") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    
    clearGlobalQueuesForIntegration();
    
    // Create test files
    std::string stationFile = createTempStationFile(
        "Desk,100000,2,Office desk\n"
        "Chair,200000,2,Office chair\n"
        "Lamp,300000,2,Desk lamp\n"
        "Computer,400000,1,Desktop computer"
    );
    
    std::string orderFile = createTempOrderFile(
        "Alice|Basic Office|Desk|Chair\n"
        "Bob|Deluxe Office|Desk|Chair|Lamp\n"
        "Charlie|Premium Office|Desk|Chair|Lamp|Computer"
    );
    
    std::string assemblyFile = createTempAssemblyFile(
        "Desk|Chair\n"
        "Chair|Lamp\n"
        "Lamp|Computer\n"
        "Computer"
    );
    
    // Load stations
    std::vector<Workstation*> stations;
    loadStations(stationFile, ',', stations);
    
    // Load orders
    loadOrders(orderFile, '|');
    
    // Create line manager
    LineManager lineManager(assemblyFile, stations);
    
    // Run the simulation
    std::ostringstream oss;
    bool completed = false;
    int iterations = 0;
    const int MAX_ITERATIONS = 20; // Prevent infinite loops in tests
    
    while (!completed && iterations < MAX_ITERATIONS) {
        completed = lineManager.run(oss);
        iterations++;
    }
    
    // Verify the results
    CHECK_EQ(g_completed.size() + g_incomplete.size(), 3u);
    
    // Charlie's order should be incomplete due to limited computer inventory
    bool foundIncomplete = false;
    for (const auto& order : g_incomplete) {
        std::ostringstream orderOutput;
        order.display(orderOutput);
        if (orderOutput.str().find("Charlie") != std::string::npos) {
            foundIncomplete = true;
            break;
        }
    }
    CHECK_EQ(foundIncomplete, true);
    
    // Cleanup
    cleanupFiles({stationFile, orderFile, assemblyFile});
    cleanupStations(stations);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Integration - Mixed Delimiters") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    
    clearGlobalQueuesForIntegration();
    
    // Create test files with different delimiters
    std::string stationFile1 = createTempStationFile(
        "Desk,100000,5,Office desk\n"
        "Chair,200000,5,Office chair"
    );
    
    std::string stationFile2 = createTempStationFile(
        "Lamp|300000|5|Desk lamp\n"
        "Computer|400000|5|Desktop computer"
    );
    
    std::string orderFile = createTempOrderFile(
        "David;Complete Office;Desk;Chair;Lamp;Computer"
    );
    
    std::string assemblyFile = createTempAssemblyFile(
        "Desk>Chair\n"
        "Chair>Lamp\n"
        "Lamp>Computer\n"
        "Computer"
    );
    
    // Load stations from the first file with comma delimiter
    std::vector<Workstation*> stations;
    loadStations(stationFile1, ',', stations);
    
    // Load stations from the second file with pipe delimiter
    loadStations(stationFile2, '|', stations);
    
    // Load orders with semicolon delimiter
    Utilities::setDelimiter(';');
    loadOrders(orderFile, ';');
    
    // Set delimiter for assembly file
    Utilities::setDelimiter('>');
    
    // Create line manager
    LineManager lineManager(assemblyFile, stations);
    
    // Run the simulation
    std::ostringstream oss;
    bool completed = false;
    int iterations = 0;
    const int MAX_ITERATIONS = 10; // Prevent infinite loops in tests
    
    while (!completed && iterations < MAX_ITERATIONS) {
        completed = lineManager.run(oss);
        iterations++;
    }
    
    // Verify the results
    CHECK_EQ(g_completed.size(), 1u);
    CHECK_EQ(g_incomplete.size(), 0u);
    
    // Cleanup
    cleanupFiles({stationFile1, stationFile2, orderFile, assemblyFile});
    cleanupStations(stations);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Integration - Reordering Stations") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    
    clearGlobalQueuesForIntegration();
    
    // Create test files with stations in random order
    std::string stationFile = createTempStationFile(
        "Chair,200000,5,Office chair\n"
        "Computer,400000,5,Desktop computer\n"
        "Desk,100000,5,Office desk\n"
        "Lamp,300000,5,Desk lamp"
    );
    
    std::string orderFile = createTempOrderFile(
        "Eva|Office|Desk|Chair|Lamp|Computer"
    );
    
    std::string assemblyFile = createTempAssemblyFile(
        "Desk|Chair\n"
        "Chair|Lamp\n"
        "Lamp|Computer\n"
        "Computer"
    );
    
    // Load stations in random order
    std::vector<Workstation*> stations;
    loadStations(stationFile, ',', stations);
    
    // Load orders
    Utilities::setDelimiter('|');
    loadOrders(orderFile, '|');
    
    // Create line manager
    LineManager lineManager(assemblyFile, stations);
    
    // Reorder stations
    lineManager.reorderStations();
    
    // Run the simulation
    std::ostringstream oss;
    bool completed = false;
    int iterations = 0;
    const int MAX_ITERATIONS = 10; // Prevent infinite loops in tests
    
    while (!completed && iterations < MAX_ITERATIONS) {
        completed = lineManager.run(oss);
        iterations++;
    }
    
    // Verify the results
    CHECK_EQ(g_completed.size(), 1u);
    CHECK_EQ(g_incomplete.size(), 0u);
    
    // Display output should be in the correct order
    std::ostringstream displayOss;
    lineManager.display(displayOss);
    std::string displayOutput = displayOss.str();
    
    // The display output should have stations in the correct order
    size_t deskPos = displayOutput.find("Desk");
    size_t chairPos = displayOutput.find("Chair");
    size_t lampPos = displayOutput.find("Lamp");
    size_t computerPos = displayOutput.find("Computer");
    
    CHECK(deskPos < chairPos);
    CHECK(chairPos < lampPos);
    CHECK(lampPos < computerPos);
    
    // Cleanup
    cleanupFiles({stationFile, orderFile, assemblyFile});
    cleanupStations(stations);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

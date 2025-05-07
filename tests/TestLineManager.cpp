#include "doctest.h"
#include "LineManager.h"
#include "Workstation.h"
#include "CustomerOrder.h"
#include "Utilities.h"
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

using namespace seneca;

// Helper function to capture output
std::string captureLineManagerOutput(const LineManager& manager) {
    std::ostringstream oss;
    manager.display(oss);
    return oss.str();
}

// Helper function to create temp file
std::string createTempAssemblyLineFile(const std::string& content) {
    std::string filename = "temp_assembly_line.txt";
    std::ofstream file(filename);
    file << content;
    file.close();
    return filename;
}

// Helper to clear global queues - in anonymous namespace to avoid linking conflicts
namespace {
    void clearLineManagerTestQueues() {
        // Clear the global queues before each test
        while (!g_pending.empty()) g_pending.pop_front();
        while (!g_completed.empty()) g_completed.pop_front();
        while (!g_incomplete.empty()) g_incomplete.pop_front();
    }
}

TEST_CASE("LineManager - Construction") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    clearLineManagerTestQueues();
    
    // Create workstations
    Workstation* desk = new Workstation("Desk,100000,5,Office desk");
    Workstation* chair = new Workstation("Chair,200000,3,Office chair");
    Workstation* lamp = new Workstation("Lamp,300000,7,Desk lamp");
    
    std::vector<Workstation*> stations = {desk, chair, lamp};
    
    // Create a temp assembly line configuration file
    std::string tempFile = createTempAssemblyLineFile(
        "Desk|Chair\n"
        "Chair|Lamp\n"
        "Lamp"
    );
    
    // Create the line manager
    LineManager manager(tempFile, stations);
    
    // Check the display output
    std::string output = captureLineManagerOutput(manager);
    CHECK(output.find("Desk --> Chair") != std::string::npos);
    CHECK(output.find("Chair --> Lamp") != std::string::npos);
    CHECK(output.find("Lamp --> End of Line") != std::string::npos);
    
    // Cleanup
    for (auto& station : stations) {
        delete station;
    }
    
    // Remove temp file
    std::remove(tempFile.c_str());
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("LineManager - Reorder Stations") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    clearLineManagerTestQueues();
    
    // Create workstations in random order
    Workstation* chair = new Workstation("Chair,200000,3,Office chair");
    Workstation* desk = new Workstation("Desk,100000,5,Office desk");
    Workstation* lamp = new Workstation("Lamp,300000,7,Desk lamp");
    
    std::vector<Workstation*> stations = {chair, desk, lamp};
    
    // Create a temp assembly line configuration file
    std::string tempFile = createTempAssemblyLineFile(
        "Desk|Chair\n"
        "Chair|Lamp\n"
        "Lamp"
    );
    
    // Create the line manager
    LineManager manager(tempFile, stations);
    
    // Reorder the stations
    manager.reorderStations();
    
    // Check the display output after reordering
    std::string output = captureLineManagerOutput(manager);
    
    // The stations should be in order now
    CHECK(output.find("Desk --> Chair") != std::string::npos);
    CHECK(output.find("Chair --> Lamp") != std::string::npos);
    CHECK(output.find("Lamp --> End of Line") != std::string::npos);
    
    // Cleanup
    for (auto& station : stations) {
        delete station;
    }
    
    // Remove temp file
    std::remove(tempFile.c_str());
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("LineManager - Run One Iteration") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    clearLineManagerTestQueues();
    
    // Create workstations
    Workstation* desk = new Workstation("Desk,100000,5,Office desk");
    Workstation* chair = new Workstation("Chair,200000,3,Office chair");
    
    std::vector<Workstation*> stations = {desk, chair};
    
    // Create a temp assembly line configuration file
    std::string tempFile = createTempAssemblyLineFile(
        "Desk|Chair\n"
        "Chair"
    );
    
    // Create a customer order
    CustomerOrder order("John|Office|Desk|Chair");
    g_pending.push_back(std::move(order));
    
    // Create the line manager
    LineManager manager(tempFile, stations);
    
    // Run one iteration
    std::ostringstream oss;
    bool completed = manager.run(oss);
    
    // One iteration is not enough to complete the order
    CHECK_EQ(completed, false);
    
    // Check the output contains the iteration number
    std::string output = oss.str();
    CHECK(output.find("Line Manager Iteration: 1") != std::string::npos);
    
    // Cleanup
    for (auto& station : stations) {
        delete station;
    }
    
    // Remove temp file
    std::remove(tempFile.c_str());
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("LineManager - Complete Order Processing") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    clearLineManagerTestQueues();
    
    // Create workstations
    Workstation* desk = new Workstation("Desk,100000,5,Office desk");
    Workstation* chair = new Workstation("Chair,200000,3,Office chair");
    
    std::vector<Workstation*> stations = {desk, chair};
    
    // Create a temp assembly line configuration file
    std::string tempFile = createTempAssemblyLineFile(
        "Desk|Chair\n"
        "Chair"
    );
    
    Utilities::setDelimiter('|');
    // Create a customer order
    CustomerOrder order("John|Office|Desk|Chair");
    g_pending.push_back(std::move(order));
    
    // Create the line manager
    LineManager manager(tempFile, stations);
    
    // Run until completion
    std::ostringstream oss;
    bool completed = false;
    
    // It should take 3 iterations to complete the order
    // Iteration 1: Move from pending to desk
    // Iteration 2: Fill desk, move to chair
    // Iteration 3: Fill chair, move to completed
    while (!completed) {
        completed = manager.run(oss);
    }
    
    // Check that the order is completed
    CHECK_EQ(g_completed.size(), 1u);
    CHECK_EQ(g_incomplete.size(), 0u);
    
    // Cleanup
    for (auto& station : stations) {
        delete station;
    }
    
    // Remove temp file
    std::remove(tempFile.c_str());
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("LineManager - Multiple Orders") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    clearLineManagerTestQueues();
    
    // Create workstations with limited inventory
    Workstation* desk = new Workstation("Desk,100000,2,Office desk");
    Workstation* chair = new Workstation("Chair,200000,1,Office chair");
    
    std::vector<Workstation*> stations = {desk, chair};
    
    // Create a temp assembly line configuration file
    std::string tempFile = createTempAssemblyLineFile(
        "Desk|Chair\n"
        "Chair"
    );
    
    Utilities::setDelimiter('|');
    // Create customer orders
    CustomerOrder order1("Alice|Office|Desk|Chair");
    CustomerOrder order2("Bob|Office|Desk|Chair");
    CustomerOrder order3("Charlie|Office|Desk|Chair");
    
    g_pending.push_back(std::move(order1));
    g_pending.push_back(std::move(order2));
    g_pending.push_back(std::move(order3));
    
    // Create the line manager
    LineManager manager(tempFile, stations);
    
    // Run until completion
    std::ostringstream oss;
    bool completed = false;
    
    while (!completed) {
        completed = manager.run(oss);
    }
    
    // Check the results
    // Alice should get a complete order
    // Bob should get desk but not chair (incomplete)
    // Charlie should not get desk or chair (incomplete)
    CHECK_EQ(g_completed.size(), 1u);
    CHECK_EQ(g_incomplete.size(), 2u);
    
    // Cleanup
    for (auto& station : stations) {
        delete station;
    }
    
    // Remove temp file
    std::remove(tempFile.c_str());
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("LineManager - Complex Assembly Line") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    clearLineManagerTestQueues();
    
    // Create workstations
    Workstation* desk = new Workstation("Desk,100000,5,Office desk");
    Workstation* chair = new Workstation("Chair,200000,5,Office chair");
    Workstation* lamp = new Workstation("Lamp,300000,5,Desk lamp");
    Workstation* computer = new Workstation("Computer,400000,5,Desktop computer");
    Workstation* monitor = new Workstation("Monitor,500000,5,Computer monitor");
    
    std::vector<Workstation*> stations = {desk, chair, lamp, computer, monitor};
    
    // Create a temp assembly line configuration file (non-linear assembly)
    std::string tempFile = createTempAssemblyLineFile(
        "Desk|Chair\n"
        "Chair|Lamp\n"
        "Computer|Monitor\n"
        "Lamp|Computer\n"
        "Monitor"
    );
    
    Utilities::setDelimiter('|');
    // Create a customer order that needs all items
    CustomerOrder order("David|Complete Office|Desk|Chair|Lamp|Computer|Monitor");
    g_pending.push_back(std::move(order));
    
    // Create the line manager
    LineManager manager(tempFile, stations);
    
    // Run until completion
    std::ostringstream oss;
    bool completed = false;
    
    while (!completed) {
        completed = manager.run(oss);
    }
    
    // Check that the order is completed
    CHECK_EQ(g_completed.size(), 1u);
    CHECK_EQ(g_incomplete.size(), 0u);
    
    // Cleanup
    for (auto& station : stations) {
        delete station;
    }
    
    // Remove temp file
    std::remove(tempFile.c_str());
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

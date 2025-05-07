#include "doctest.h"
#include "Workstation.h"
#include "CustomerOrder.h"
#include "Utilities.h"
#include <sstream>
#include <string>

using namespace seneca;

// Helper function to capture output
std::string captureWorkstationOutput(const Workstation& station) {
    std::ostringstream oss;
    station.display(oss);
    return oss.str();
}

// Test setup and cleanup helpers in anonymous namespace to avoid linking conflicts
namespace {
    void clearWorkstationTestQueues() {
        // Clear the global queues before each test
        while (!g_pending.empty()) g_pending.pop_front();
        while (!g_completed.empty()) g_completed.pop_front();
        while (!g_incomplete.empty()) g_incomplete.pop_front();
    }
}

TEST_CASE("Workstation - Construction") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    clearWorkstationTestQueues();
    
    // Create a workstation
    Workstation station("Desk,100000,5,Office desk");
    
    // Check the item name (inherited from Station)
    CHECK_EQ(station.getItemName(), "Desk");
    
    // Check the quantity (inherited from Station)
    CHECK_EQ(station.getQuantity(), 5u);
    
    // By default, next station should be nullptr
    CHECK_EQ(station.getNextStation(), nullptr);
    
    // Check the display output
    std::string output = captureWorkstationOutput(station);
    CHECK(output.find("Desk") != std::string::npos);
    CHECK(output.find("End of Line") != std::string::npos);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Workstation - Set Next Station") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    clearWorkstationTestQueues();
    
    // Create two workstations
    Workstation desk("Desk,100000,5,Office desk");
    Workstation chair("Chair,200000,5,Office chair");
    
    // Set chair as the next station after desk
    desk.setNextStation(&chair);
    
    // Check that the next station is correctly set
    CHECK_EQ(desk.getNextStation(), &chair);
    
    // Check the display output shows the next station
    std::string output = captureWorkstationOutput(desk);
    CHECK(output.find("Desk") != std::string::npos);
    CHECK(output.find("Chair") != std::string::npos);
    CHECK(output.find("End of Line") == std::string::npos);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Workstation - Adding Orders") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    clearWorkstationTestQueues();
    
    // Create a workstation
    Workstation desk("Desk,100000,5,Office desk");
    
    Utilities::setDelimiter('|');
    // Create a customer order
    CustomerOrder order("John|Office|Desk");
    
    // Add the order to the workstation
    desk += std::move(order);
    
    // Fill the order
    std::ostringstream oss;
    desk.fill(oss);
    
    // Check that the fill message indicates the item was filled
    std::string fillMessage = oss.str();
    CHECK(fillMessage.find("Filled John") != std::string::npos);
    CHECK(fillMessage.find("Desk") != std::string::npos);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Workstation - Fill First Order") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    clearWorkstationTestQueues();
    
    // Create a workstation
    Workstation desk("Desk,100000,5,Office desk");
    

    Utilities::setDelimiter('|');
    // Create two customer orders
    CustomerOrder order1("Alice|Office|Desk");
    CustomerOrder order2("Bob|Office|Desk");
    
    // Add the orders to the workstation
    desk += std::move(order1);
    desk += std::move(order2);
    
    // Fill orders - should only fill the first one
    std::ostringstream oss;
    desk.fill(oss);
    
    // Check that only Alice's order was filled
    std::string fillMessage = oss.str();
    CHECK(fillMessage.find("Filled Alice") != std::string::npos);
    CHECK(fillMessage.find("Bob") == std::string::npos);
    
    // Fill again - should now fill Bob's order
    std::ostringstream oss2;
    desk.fill(oss2);
    
    // Check that Bob's order is now being filled
    std::string fillMessage2 = oss2.str();
    CHECK(fillMessage2.find("Filled Bob") != std::string::npos);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Workstation - Attempt To Move Order") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    clearWorkstationTestQueues();
    
    // Create two workstations
    Workstation desk("Desk,100000,5,Office desk");
    Workstation chair("Chair,200000,5,Office chair");
    
    // Set chair as the next station after desk
    desk.setNextStation(&chair);
    
    Utilities::setDelimiter(',');
    // Create a customer order that needs both desk and chair
    CustomerOrder order("Charlie|Office|Desk|Chair");
    
    // Add the order to the desk workstation
    desk += std::move(order);
    
    // Fill the desk part of the order
    std::ostringstream oss;
    desk.fill(oss);
    
    // Attempt to move the order - should move to chair since desk is filled
    bool moved = desk.attemptToMoveOrder();
    
    // The order should have moved
    CHECK_EQ(moved, true);
    
    // Fill the chair part of the order
    std::ostringstream oss2;
    chair.fill(oss2);
    
    // Attempt to move the order from chair - should move to completed since there's no next station
    bool moved2 = chair.attemptToMoveOrder();
    
    // The order should have moved
    CHECK_EQ(moved2, true);
    
    // Check that the order is now in the completed queue
    CHECK_EQ(g_completed.size(), 1u);
    CHECK_EQ(g_incomplete.size(), 0u);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Workstation - Incomplete Order") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    clearWorkstationTestQueues();
    
    // Create a workstation with no inventory
    Workstation emptyDesk("Desk,100000,0,Office desk");
    
    Utilities::setDelimiter('|');
    // Create a customer order that needs a desk
    CustomerOrder order("Dave|Office|Desk");
    
    // Add the order to the workstation
    emptyDesk += std::move(order);
    
    // Try to fill the order (will fail due to no inventory)
    std::ostringstream oss;
    emptyDesk.fill(oss);
    
    // Check that the fill message indicates the item could not be filled
    std::string fillMessage = oss.str();
    CHECK(fillMessage.find("Unable to fill") != std::string::npos);
    
    // Attempt to move the order - should move to incomplete since there's no next station
    bool moved = emptyDesk.attemptToMoveOrder();
    
    // The order should have moved
    CHECK_EQ(moved, true);
    
    // Check that the order is now in the incomplete queue
    CHECK_EQ(g_completed.size(), 0u);
    CHECK_EQ(g_incomplete.size(), 1u);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Workstation - Multiple Orders Processing") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    clearWorkstationTestQueues();
    
    // Create workstations with limited inventory
    Workstation desk("Desk,100000,2,Office desk");
    Workstation chair("Chair,200000,1,Office chair");
    
    // Set chair as the next station after desk
    desk.setNextStation(&chair);
    
    Utilities::setDelimiter('|');
    // Create customer orders
    CustomerOrder order1("Eve|Office|Desk|Chair");
    CustomerOrder order2("Frank|Office|Desk|Chair");
    CustomerOrder order3("Grace|Office|Desk|Chair");
    
    // Add orders to the desk workstation
    desk += std::move(order1);
    desk += std::move(order2);
    desk += std::move(order3);
    
    // Process all orders through both stations
    std::ostringstream oss;
    
    // First iteration
    desk.fill(oss);           // Fill Eve's desk
    desk.attemptToMoveOrder(); // Move Eve to chair
    chair.fill(oss);          // Fill Eve's chair
    chair.attemptToMoveOrder(); // Move Eve to completed
    
    // Second iteration
    desk.fill(oss);           // Fill Frank's desk
    desk.attemptToMoveOrder(); // Move Frank to chair
    chair.fill(oss);          // Try to fill Frank's chair (no inventory)
    chair.attemptToMoveOrder(); // Move Frank to incomplete
    
    // Third iteration
    desk.fill(oss);           // Try to fill Grace's desk (no inventory)
    desk.attemptToMoveOrder(); // Move Grace to chair (item not filled)
    chair.fill(oss);          // Try to fill Grace's chair (no inventory)
    chair.attemptToMoveOrder(); // Move Grace to incomplete
    
    // Check the queues
    CHECK_EQ(g_completed.size(), 1u);   // Eve's order should be completed
    CHECK_EQ(g_incomplete.size(), 2u);  // Frank and Grace's orders should be incomplete
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

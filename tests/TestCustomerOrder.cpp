#include "doctest.h"
#include "CustomerOrder.h"
#include "Station.h"
#include "Utilities.h"
#include <sstream>
#include <string>

using namespace seneca;

// Helper function to capture output
std::string captureOrderOutput(const CustomerOrder& order) {
    std::ostringstream oss;
    order.display(oss);
    return oss.str();
}

TEST_CASE("CustomerOrder - Default Constructor") {
    CustomerOrder order;
    
    // Verify the displayed output is minimal (empty order)
    std::string output = captureOrderOutput(order);
    CHECK(output.find(" - ") != std::string::npos);
    CHECK(output.find("TO BE FILLED") == std::string::npos); // No items to fill
}

TEST_CASE("CustomerOrder - Constructor with Record") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter('|');
    
    // Create a customer order with items
    CustomerOrder order("John|Office Setup|Desk|Chair|Lamp");
    
    // Check the output contains the customer name and product
    std::string output = captureOrderOutput(order);
    CHECK(output.find("John") != std::string::npos);
    CHECK(output.find("Office Setup") != std::string::npos);
    
    // Verify all items are unfilled initially
    CHECK(output.find("TO BE FILLED") != std::string::npos);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("CustomerOrder - Copy Constructor Throws") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter('|');
    
    // Create a customer order
    CustomerOrder order("Jane|Home Office|Desk|Chair");
    
    // Verify that copy construction throws
    CHECK_THROWS_AS(CustomerOrder copy(order), const char*);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("CustomerOrder - Move Constructor") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter('|');
    
    // Create a customer order
    CustomerOrder order1("Bob|Gaming Setup|Desk|Chair|Monitor|Keyboard");
    
    // Move construct another order
    CustomerOrder order2(std::move(order1));
    
    // Verify the moved order contains the expected data
    std::string output = captureOrderOutput(order2);
    CHECK(output.find("Bob") != std::string::npos);
    CHECK(output.find("Gaming Setup") != std::string::npos);
    CHECK(output.find("Desk") != std::string::npos);
    CHECK(output.find("Chair") != std::string::npos);
    CHECK(output.find("Monitor") != std::string::npos);
    CHECK(output.find("Keyboard") != std::string::npos);
    
    // The original order should be emptied
    std::string emptyOutput = captureOrderOutput(order1);
    CHECK(emptyOutput.find("Bob") == std::string::npos);
    CHECK(emptyOutput.find("Gaming Setup") == std::string::npos);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("CustomerOrder - Move Assignment") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter('|');
    
    // Create two customer orders
    CustomerOrder order1("Sara|Kitchen|Fridge|Stove|Sink");
    CustomerOrder order2("Mike|Bathroom|Shower|Toilet|Sink");
    
    // Move assign the second order to the first
    order1 = std::move(order2);
    
    // Verify the first order now contains the second order's data
    std::string output = captureOrderOutput(order1);
    CHECK(output.find("Mike") != std::string::npos);
    CHECK(output.find("Bathroom") != std::string::npos);
    CHECK(output.find("Sara") == std::string::npos);
    CHECK(output.find("Kitchen") == std::string::npos);
    
    // The second order should be emptied
    std::string emptyOutput = captureOrderOutput(order2);
    CHECK(emptyOutput.find("Mike") == std::string::npos);
    CHECK(emptyOutput.find("Bathroom") == std::string::npos);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("CustomerOrder - Fill Item") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    // Create a station
    Station desk("Desk,100000,5,Office desk");
    
    Utilities::setDelimiter('|');

    // Create a customer order with a desk item
    CustomerOrder order("Alex|Office|Desk|Chair");
    
    // Initially, no items should be filled
    CHECK_EQ(order.isOrderFilled(), false);
    CHECK_EQ(order.isItemFilled("Desk"), false);
    
    // Fill the desk item
    std::ostringstream oss;
    order.fillItem(desk, oss);
    
    // Check that the desk item is now filled
    CHECK_EQ(order.isItemFilled("Desk"), true);
    
    // Order is not completely filled because the chair is still unfilled
    CHECK_EQ(order.isOrderFilled(), false);
    
    // Check the fill message
    std::string fillMessage = oss.str();
    CHECK(fillMessage.find("Filled Alex") != std::string::npos);
    CHECK(fillMessage.find("Desk") != std::string::npos);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("CustomerOrder - Order Filled Status") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    // Create stations
    Station desk("Desk,100000,5,Office desk");
    Station chair("Chair,200000,5,Office chair");
    
    Utilities::setDelimiter('|');

    // Create a customer order with desk and chair items
    CustomerOrder order("Pat|Office|Desk|Chair");
    
    // Initially, no items should be filled
    CHECK_EQ(order.isOrderFilled(), false);
    
    // Fill the desk item
    std::ostringstream oss1;
    order.fillItem(desk, oss1);
    
    // Order is not completely filled yet
    CHECK_EQ(order.isOrderFilled(), false);
    
    // Fill the chair item
    std::ostringstream oss2;
    order.fillItem(chair, oss2);
    
    // Now the order should be completely filled
    CHECK_EQ(order.isOrderFilled(), true);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("CustomerOrder - Cannot Fill Due to Stock") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    // Create a station with 0 quantity
    Station emptyStation("EmptyItem,300000,0,Item with no stock");
    
    Utilities::setDelimiter('|');
    // Create a customer order with the empty item
    CustomerOrder order("Chris|Test|EmptyItem");
    
    // Try to fill the item
    std::ostringstream oss;
    order.fillItem(emptyStation, oss);
    
    // Check the unable to fill message
    std::string fillMessage = oss.str();
    bool hasUnableToFill = fillMessage.find("Unable to fill") != std::string::npos;
    CHECK(hasUnableToFill);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("CustomerOrder - Fill Non-Matching Item") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    // Create a station
    Station lamp("Lamp,400000,5,Desk lamp");
    
    Utilities::setDelimiter('|');
    // Create a customer order without a lamp item
    CustomerOrder order("Jamie|Office|Desk|Chair");
    
    // Try to fill a lamp item (which is not in the order)
    std::ostringstream oss;
    order.fillItem(lamp, oss);
    
    // There should be no fill message since the item wasn't in the order
    std::string fillMessage = oss.str();
    CHECK_EQ(fillMessage, "");
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

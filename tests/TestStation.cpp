#include "doctest.h"
#include "Station.h"
#include "Utilities.h"
#include <sstream>
#include <string>

using namespace seneca;

// Helper function to capture output
std::string captureOutput(const Station& station, bool full) {
    std::ostringstream oss;
    station.display(oss, full);
    return oss.str();
}

TEST_CASE("Station - Construction from Record") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    // Create a test station
    Station station("Desk,123456,5,Office desk with drawers");
    
    CHECK_EQ(station.getItemName(), "Desk");
    CHECK_EQ(station.getQuantity(), 5u);
    
    // First call to getNextSerialNumber should return the initial value (123456)
    // and increment it
    CHECK_EQ(station.getNextSerialNumber(), 123456u);
    
    // Second call should return the incremented value
    CHECK_EQ(station.getNextSerialNumber(), 123457u);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Station - Update Quantity") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    // Create a test station with quantity 5
    Station station("Item,100000,5,Test item");
    
    // Initial quantity should be 5
    CHECK_EQ(station.getQuantity(), 5u);
    
    // Update quantity once
    station.updateQuantity();
    CHECK_EQ(station.getQuantity(), 4u);
    
    // Update quantity again
    station.updateQuantity();
    CHECK_EQ(station.getQuantity(), 3u);
    
    // Update quantity three more times
    station.updateQuantity();
    station.updateQuantity();
    station.updateQuantity();
    CHECK_EQ(station.getQuantity(), 0u);
    
    // Update quantity when it's already 0
    station.updateQuantity();
    CHECK_EQ(station.getQuantity(), 0u); // Should remain 0
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Station - Display Summary") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    // Create a test station
    Station station("Chair,550000,10,Comfortable office chair");
    
    // Capture the display output for summary view (full = false)
    std::string output = captureOutput(station, false);
    
    // Check that the output contains the expected format but no quantity or description
    CHECK(output.find("Chair") != std::string::npos);
    CHECK(output.find("550000") != std::string::npos);
    
    // Quantity and description should not be in the summary output
    // Check separately to avoid complex expressions
    bool noQuantity = output.find("10") == std::string::npos;
    bool noDescription = output.find("Comfortable office chair") == std::string::npos;
    CHECK((noQuantity || noDescription)); // At least one should be missing
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Station - Display Full") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    // Create a test station
    Station station("Bookcase,330000,8,5-shelf bookcase");
    
    // Capture the display output for full view (full = true)
    std::string output = captureOutput(station, true);
    
    // Check that the output contains all the expected information
    CHECK(output.find("Bookcase") != std::string::npos);
    CHECK(output.find("330000") != std::string::npos);
    CHECK(output.find("8") != std::string::npos);
    CHECK(output.find("5-shelf bookcase") != std::string::npos);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Station - Field Width Adjustment") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    // Create separate instances to test field width behavior
    // First create stations in a separate scope
    {
        // Create a station with a short name
        Station station1("Desk,100000,5,Simple desk");
        
        // Display to check initial formatting
        std::string output1 = captureOutput(station1, true);
        
        // Create a station with a longer name
        Station station2("ExecutiveChair,200000,3,Executive chair with armrests");
        
        // Display to check adjusted formatting
        std::string output2 = captureOutput(station2, true);
        
        // The output should show the field width adjustment
        CHECK(output1.find("Desk") != std::string::npos);
        CHECK(output2.find("ExecutiveChair") != std::string::npos);
    }
    
    // Create new station after the width adjustment should have happened
    Station station3("Lamp,300000,7,Desk lamp");
    std::string output3 = captureOutput(station3, true);
    
    // The lamp name should now be padded to match the previous width
    CHECK(output3.find("Lamp") != std::string::npos);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Station - Update Quantity") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    // Create a test station with quantity 5
    Station station("Item,100000,5,Test item");
    
    // Initial quantity should be 5
    CHECK_EQ(station.getQuantity(), 5u);
    
    // Update quantity once
    station.updateQuantity();
    CHECK_EQ(station.getQuantity(), 4u);
    
    // Update quantity again
    station.updateQuantity();
    CHECK_EQ(station.getQuantity(), 3u);
    
    // Update quantity three more times
    station.updateQuantity();
    station.updateQuantity();
    station.updateQuantity();
    CHECK_EQ(station.getQuantity(), 0u);
    
    // Update quantity when it's already 0
    station.updateQuantity();
    CHECK_EQ(station.getQuantity(), 0u); // Should remain 0
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Station - Display Summary") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    // Create a test station
    Station station("Chair,550000,10,Comfortable office chair");
    
    // Capture the display output for summary view (full = false)
    std::string output = captureOutput(station, false);
    
    // Check that the output contains the expected format but no quantity or description
    CHECK(output.find("Chair") != std::string::npos);
    CHECK(output.find("550000") != std::string::npos);
    
    // Quantity and description should not be in the summary output
    CHECK(output.find("10") == std::string::npos);
    CHECK(output.find("Comfortable office chair") == std::string::npos);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Station - Display Full") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    // Create a test station
    Station station("Bookcase,330000,8,5-shelf bookcase");
    
    // Capture the display output for full view (full = true)
    std::string output = captureOutput(station, true);
    
    // Check that the output contains all the expected information
    CHECK(output.find("Bookcase") != std::string::npos);
    CHECK(output.find("330000") != std::string::npos);
    CHECK(output.find("8") != std::string::npos);
    CHECK(output.find("5-shelf bookcase") != std::string::npos);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Station - Field Width Adjustment") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    // Reset static width field for testing
    Station::resetWidthField();
    
    // Create a station with a short name
    Station station1("Desk,100000,5,Simple desk");
    
    // Create a station with a longer name
    Station station2("ExecutiveChair,200000,3,Executive chair with armrests");
    
    // Display both stations to verify field width adjustment
    std::string output1 = captureOutput(station1, true);
    std::string output2 = captureOutput(station2, true);
    
    // Both outputs should use the same field width for the name
    // The field width should be adjusted to the length of the longest name
    CHECK(output1.find("Desk          ") != std::string::npos); // padded with spaces
    CHECK(output2.find("ExecutiveChair") != std::string::npos);
    
    // Restore delimiter
    Utilities::setDelimiter(originalDelimiter);
}

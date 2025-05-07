#include "doctest.h"
#include "Utilities.h"
#include <string>

using namespace seneca;

TEST_CASE("Utilities - Default Constructor") {
    Utilities util;
    CHECK_EQ(util.getFieldWidth(), 1u);
}

TEST_CASE("Utilities - Setting and Getting Field Width") {
    Utilities util;
    util.setFieldWidth(10);
    CHECK_EQ(util.getFieldWidth(), 10u);
}

TEST_CASE("Utilities - Setting and Getting Delimiter") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    
    Utilities::setDelimiter('|');
    CHECK_EQ(Utilities::getDelimiter(), '|');
    
    // Test a different delimiter
    Utilities::setDelimiter(';');
    CHECK_EQ(Utilities::getDelimiter(), ';');
    
    // Restore the original delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Utilities - Extract Token Basic Functionality") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    Utilities util;
    std::string str = "Token1,Token2,Token3";
    size_t next_pos = 0;
    bool more = true;
    
    // Extract first token
    std::string token = util.extractToken(str, next_pos, more);
    CHECK_EQ(token, "Token1");
    CHECK_EQ(more, true);
    
    // Extract second token
    token = util.extractToken(str, next_pos, more);
    CHECK_EQ(token, "Token2");
    CHECK_EQ(more, true);
    
    // Extract third token
    token = util.extractToken(str, next_pos, more);
    CHECK_EQ(token, "Token3");
    CHECK_EQ(more, false);
    
    // Restore the original delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Utilities - Extract Token with Trimming") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    Utilities util;
    std::string str = "  Token1  ,  Token2  ,  Token3  ";
    size_t next_pos = 0;
    bool more = true;
    
    // Extract first token (should be trimmed)
    std::string token = util.extractToken(str, next_pos, more);
    CHECK_EQ(token, "Token1");
    
    // Extract second token (should be trimmed)
    token = util.extractToken(str, next_pos, more);
    CHECK_EQ(token, "Token2");
    
    // Extract third token (should be trimmed)
    token = util.extractToken(str, next_pos, more);
    CHECK_EQ(token, "Token3");
    
    // Restore the original delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Utilities - Field Width Update") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    Utilities util;
    std::string str = "ShortToken,LongerTokenHere,VeryLongTokenHereIndeed";
    size_t next_pos = 0;
    bool more = true;
    
    // Initial field width should be 1
    CHECK_EQ(util.getFieldWidth(), 1u);
    
    // Extract first token
    util.extractToken(str, next_pos, more);
    CHECK_EQ(util.getFieldWidth(), 10u); // "ShortToken" length
    
    // Extract second token
    util.extractToken(str, next_pos, more);
    CHECK_EQ(util.getFieldWidth(), 15u); // "LongerTokenHere" length
    
    // Extract third token
    util.extractToken(str, next_pos, more);
    CHECK_EQ(util.getFieldWidth(), 23u); // "VeryLongTokenHereIndeed" length
    
    // Restore the original delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Utilities - Empty Token Handling") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    Utilities util;
    std::string str = "Token1,,Token3";
    size_t next_pos = 0;
    bool more = true;
    
    // Extract first token
    std::string token = util.extractToken(str, next_pos, more);
    CHECK_EQ(token, "Token1");
    
    // Extract second token (empty)
    CHECK_THROWS_AS(util.extractToken(str, next_pos, more), std::string);
    
    // Restore the original delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Utilities - Token at End of String") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    Utilities util;
    std::string str = "Token1,Token2,";
    size_t next_pos = 0;
    bool more = true;
    
    // Extract first token
    std::string token = util.extractToken(str, next_pos, more);
    CHECK_EQ(token, "Token1");
    CHECK_EQ(more, true);
    
    // Extract second token
    token = util.extractToken(str, next_pos, more);
    CHECK_EQ(token, "Token2");
    CHECK_EQ(more, false);
    
    // Restore the original delimiter
    Utilities::setDelimiter(originalDelimiter);
}

TEST_CASE("Utilities - Update Field Width Flag") {
    // Save the original delimiter to restore later
    char originalDelimiter = Utilities::getDelimiter();
    Utilities::setDelimiter(',');
    
    Utilities util;
    util.setFieldWidth(5);
    
    std::string str = "VeryLongToken,ShortToken";
    size_t next_pos = 0;
    bool more = true;
    
    // Extract with updateWidth = true (default)
    util.extractToken(str, next_pos, more);
    CHECK_EQ(util.getFieldWidth(), 13u); // "VeryLongToken" length
    
    // Extract with updateWidth = false
    util.extractToken(str, next_pos, more, false);
    CHECK_EQ(util.getFieldWidth(), 13u); // Should remain unchanged
    
    // Restore the original delimiter
    Utilities::setDelimiter(originalDelimiter);
}

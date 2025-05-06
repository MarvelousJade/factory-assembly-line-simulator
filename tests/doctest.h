// This is a simplified version of doctest.h for demonstration
// In a real project, you would download the full doctest.h from:
// https://github.com/doctest/doctest/blob/master/doctest/doctest.h

// doctest.h simplified version
#ifndef DOCTEST_H
#define DOCTEST_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <exception>
#include <cmath>

namespace doctest {

    // Basic assertion macros
    #define DOCTEST_REQUIRE(expr) do { if(!(expr)) { std::cerr << "FATAL ERROR: " << __FILE__ << ":" << __LINE__ << " - REQUIRE(" #expr ") failed!\n"; std::terminate(); } } while(false)
    #define DOCTEST_CHECK(expr) do { if(!(expr)) { std::cerr << "ERROR: " << __FILE__ << ":" << __LINE__ << " - CHECK(" #expr ") failed!\n"; } } while(false)
    #define DOCTEST_WARN(expr) do { if(!(expr)) { std::cerr << "WARNING: " << __FILE__ << ":" << __LINE__ << " - WARN(" #expr ") failed!\n"; } } while(false)

    // String comparison macros
    #define DOCTEST_REQUIRE_EQ(a, b) do { if(!((a) == (b))) { std::cerr << "FATAL ERROR: " << __FILE__ << ":" << __LINE__ << " - REQUIRE_EQ(" #a ", " #b ") failed!\n"; std::terminate(); } } while(false)
    #define DOCTEST_CHECK_EQ(a, b) do { if(!((a) == (b))) { std::cerr << "ERROR: " << __FILE__ << ":" << __LINE__ << " - CHECK_EQ(" #a ", " #b ") failed!\n"; } } while(false)
    #define DOCTEST_WARN_EQ(a, b) do { if(!((a) == (b))) { std::cerr << "WARNING: " << __FILE__ << ":" << __LINE__ << " - WARN_EQ(" #a ", " #b ") failed!\n"; } } while(false)

    // Exception handling macros
    #define DOCTEST_REQUIRE_THROWS_AS(expr, exception_type) do { \
        bool exception_thrown = false; \
        try { expr; } \
        catch(const exception_type&) { exception_thrown = true; } \
        catch(...) { std::cerr << "FATAL ERROR: " << __FILE__ << ":" << __LINE__ << " - REQUIRE_THROWS_AS(" #expr ", " #exception_type ") failed - wrong exception type!\n"; std::terminate(); } \
        if(!exception_thrown) { std::cerr << "FATAL ERROR: " << __FILE__ << ":" << __LINE__ << " - REQUIRE_THROWS_AS(" #expr ", " #exception_type ") failed - no exception thrown!\n"; std::terminate(); } \
    } while(false)

    // Test case definition
    #define DOCTEST_TEST_CASE(name) static void test_case_##name(); \
                                   class test_registrar_##name { \
                                   public: \
                                       test_registrar_##name() { doctest::register_test(#name, test_case_##name); } \
                                   } test_registrar_instance_##name; \
                                   static void test_case_##name()
                                   
    // Subcase definition
    #define DOCTEST_SUBCASE(name) if(true)

    // Define common aliases
    #define TEST_CASE DOCTEST_TEST_CASE
    #define SUBCASE DOCTEST_SUBCASE
    #define REQUIRE DOCTEST_REQUIRE
    #define CHECK DOCTEST_CHECK
    #define WARN DOCTEST_WARN
    #define REQUIRE_EQ DOCTEST_REQUIRE_EQ
    #define CHECK_EQ DOCTEST_CHECK_EQ
    #define WARN_EQ DOCTEST_WARN_EQ
    #define REQUIRE_THROWS_AS DOCTEST_REQUIRE_THROWS_AS

    // Test context
    struct TestCase {
        std::string name;
        void (*func)();
    };

    std::vector<TestCase>& get_test_cases() {
        static std::vector<TestCase> test_cases;
        return test_cases;
    }

    void register_test(const std::string& name, void (*func)()) {
        get_test_cases().push_back({name, func});
    }

    // Main test runner
    int run_tests() {
        int failures = 0;
        
        std::cout << "Running " << get_test_cases().size() << " test cases...\n";
        
        for(const auto& test : get_test_cases()) {
            std::cout << "TEST CASE: " << test.name << "\n";
            try {
                test.func();
            } catch(const std::exception& e) {
                std::cerr << "EXCEPTION: " << e.what() << "\n";
                failures++;
            } catch(...) {
                std::cerr << "UNKNOWN EXCEPTION\n";
                failures++;
            }
        }
        
        if(failures == 0) {
            std::cout << "All tests passed!\n";
        } else {
            std::cout << failures << " tests failed!\n";
        }
        
        return failures;
    }
}

// Magic main to make it work like doctest
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
int main(int argc, char** argv) {
    return doctest::run_tests();
}
#endif

#endif // DOCTEST_H

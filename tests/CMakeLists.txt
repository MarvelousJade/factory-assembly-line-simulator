cmake_minimum_required(VERSION 3.12)

# Test executable
set(TEST_NAME factory_assembly_tests)

# Source files for the test executable
set(TEST_SOURCES
    TestMain.cpp
    TestUtilities.cpp
    TestStation.cpp
    TestCustomerOrder.cpp
    TestWorkstation.cpp
    TestLineManager.cpp
)

# Create the test executable
add_executable(${TEST_NAME} ${TEST_SOURCES})

# Link with the main library
target_include_directories(${TEST_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

# Add the compiled source files directly since we don't have a library target
target_sources(${TEST_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/Utilities.cpp
    ${CMAKE_SOURCE_DIR}/src/Station.cpp
    ${CMAKE_SOURCE_DIR}/src/CustomerOrder.cpp
    ${CMAKE_SOURCE_DIR}/src/Workstation.cpp
    ${CMAKE_SOURCE_DIR}/src/LineManager.cpp
)

# Copy test data files
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/test_data)
file(GLOB TEST_DATA_FILES "${CMAKE_SOURCE_DIR}/data/*.txt")
foreach(DATA_FILE ${TEST_DATA_FILES})
    get_filename_component(FILE_NAME ${DATA_FILE} NAME)
    configure_file(${DATA_FILE} ${CMAKE_BINARY_DIR}/bin/test_data/${FILE_NAME} COPYONLY)
endforeach()

# Define a custom target to copy test data files
add_custom_target(copy_test_data ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/bin/test_data
    COMMENT "Copying test data files to build directory"
)

# Set compiler options
if(MSVC)
    target_compile_options(${TEST_NAME} PRIVATE /W4)
else()
    target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

# Make the test executable depend on the data copy
add_dependencies(${TEST_NAME} copy_test_data)

# Add test definition
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

# Output directory configuration
set_target_properties(${TEST_NAME} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Print information
message(STATUS "Test configuration: ${TEST_NAME}")

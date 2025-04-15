cmake_minimum_required(VERSION 3.12)
project(factory_assembly_line VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define source and include directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)

# Source files
set(SOURCES
    ${SRC_DIR}/Utilities.cpp
    ${SRC_DIR}/Station.cpp
    ${SRC_DIR}/CustomerOrder.cpp
    ${SRC_DIR}/Workstation.cpp
    ${SRC_DIR}/LineManager.cpp
    ${SRC_DIR}/Main.cpp
)

# Header files
set(HEADERS
    ${INCLUDE_DIR}/Utilities.h
    ${INCLUDE_DIR}/Station.h
    ${INCLUDE_DIR}/CustomerOrder.h
    ${INCLUDE_DIR}/Workstation.h
    ${INCLUDE_DIR}/LineManager.h
)

# Create executable
add_executable(simulator ${SOURCES} ${HEADERS})

# Add include directory
target_include_directories(simulator PRIVATE ${INCLUDE_DIR})

# Add compiler warnings
if(MSVC)
    target_compile_options(simulator PRIVATE /W4)
else()
    target_compile_options(simulator PRIVATE -Wall -Wextra -pedantic)
endif()

# Add debug information for debug builds
if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /Od")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif()

# Output directory configuration
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Create data directory in build folder
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data)

# Copy data files to build directory
file(GLOB DATA_FILES "${DATA_DIR}/*.txt")
foreach(DATA_FILE ${DATA_FILES})
    get_filename_component(FILE_NAME ${DATA_FILE} NAME)
    configure_file(${DATA_FILE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data/${FILE_NAME} COPYONLY)
endforeach()

# Define a custom target to copy data files (helps with IDE integration)
add_custom_target(copy_data ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${DATA_DIR} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data
    COMMENT "Copying data files to build directory"
)

# Make simulator depend on copy_data
add_dependencies(simulator copy_data)

# Installation rules
install(TARGETS simulator DESTINATION bin)
install(DIRECTORY ${DATA_DIR}/ DESTINATION bin/data)

# Add definition to code so it knows where to find data files
target_compile_definitions(simulator PRIVATE 
    DATA_DIRECTORY="${CMAKE_INSTALL_PREFIX}/bin/data")

# CTest setup (if you want to add tests later)
enable_testing()

# Print configuration info
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Data directory: ${DATA_DIR}")

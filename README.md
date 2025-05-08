# Factory Assembly Line Simulator

A robust C++ application for simulating manufacturing assembly lines, allowing companies to model and optimize their production processes.

## Overview

The Factory Assembly Line Simulator enables production managers and engineers to:

- Create a virtual model of any manufacturing assembly line
- Process customer orders through a sequence of workstations
- Track inventory levels across the production line
- Identify and resolve bottlenecks in the manufacturing process
- Predict order completion rates and optimize resource allocation
- Test different line configurations to maximize efficiency

## Key Features

- **Configurable Workstations**: Define any number of stations with custom inventory and processing capabilities
- **Dynamic Order Management**: Handle a variety of customer orders with different requirements
- **Real-time Inventory Tracking**: Monitor stock levels at each workstation
- **Detailed Reporting**: Get comprehensive reports on completed orders, incomplete orders, and remaining inventory
- **Flexible Configuration**: Set up your assembly line through easy-to-edit text files
- **Interactive & Batch Modes**: Run the simulator interactively with a user-friendly menu or via command-line for automated testing
- **Comprehensive Testing Framework**: Verify system behavior with extensive test cases

## Architecture

The simulator is built on these core components:

- **Station**: Base class representing a location on the assembly line with inventory
- **Workstation**: Active station that fulfills items for customer orders
- **CustomerOrder**: Represents an order being processed through the line
- **LineManager**: Configures and controls the flow of orders through workstations
- **Utilities**: Helper functionality for parsing configuration files

## Installation

### Prerequisites

- C++17 compatible compiler (GCC 8+, Clang 7+, MSVC 19.14+)
- CMake 3.12 or higher (for building)

### Building from Source

```bash
# Clone this repository
git clone https://github.com/MarvelousJade/factory-assembly-line-simulator.git

# Navigate to the project directory
cd factory-assembly-line

# Create a build directory
mkdir build
cd build

# Generate build files and build the project
cmake ..
cmake --build .

# Or on Unix-like systems, simply use:
# cmake ..
# make
```

## Usage

The simulator can be run in two modes:

### Interactive Mode

```bash
./bin/simulator
```

This launches a menu-driven interface where you can:
1. Load station data files with different delimiters (comma, pipe, etc.)
2. Load customer orders
3. Configure the assembly line
4. Display workstations (summary or detailed view)
5. Display customer orders
6. Display assembly line configuration
7. Run the simulation
8. View detailed results (completed orders, incomplete orders, remaining inventory)

### Batch Mode

```bash
./bin/simulator Stations1.txt Stations2.txt CustomerOrders.txt AssemblyLine.txt
```

This runs the entire simulation from the provided configuration files and outputs the results. The batch mode expects:
1. First stations file (comma-delimited)
2. Second stations file (pipe-delimited)
3. Customer orders file
4. Assembly line configuration file

## Configuration Files

The simulator uses text files for configuration:

### Station Files

Define the workstations and their inventory. The system supports different delimiter formats:

Comma-delimited (Stations1.txt):
```
Armchair,654321,10,Upholstered Wing Chair
Bed,123456,5,Queen size bed with headboard
Dresser,56789,7,6-Drawer Unit
Nighttable,887,5,Nightstand with 2 drawers
```

Pipe-delimited (Stations2.txt):
```
Filing Cabinet | 987654 | 5 | 3-drawer filing cabinet
Office Chair | 147852 | 20 | High-back carpet-rollers
Bookcase | 987 | 5 | 5-shelf open bookcase
Desk | 459214 | 2 | 6-foot worktable
```

Each line contains: `ItemName,SerialNumber,Quantity,Description`

### Customer Order File

Define customer orders with required items:

```
Cornel B. | 1-Room Home Office | Office Chair|Desk|Bookcase|Bookcase|Filing Cabinet
Chris S. | Bedroom | Bed|Armchair|Nighttable|Dresser|Nighttable
John M. | Kids Bedroom | Bed|Bed|Bed|Bed|Nighttable|Nighttable|Dresser|Desk
```

Each line contains: `CustomerName|ProductName|Item1|Item2|...|ItemN`

### Assembly Line Configuration File

Define the flow between workstations:

```
Nighttable|Desk
Dresser|Armchair
Filing Cabinet|Bookcase
Bookcase
Bed|Dresser
Office Chair|Filing Cabinet
```

Each line contains: `CurrentStation|NextStation` or just `CurrentStation` for the end of the line.

## Example Workflow

1. **Configure Workstations**: Define your stations in configuration files with their inventory levels
2. **Create Customer Orders**: Define what products customers want and which items they contain
3. **Design Assembly Line**: Configure the assembly line flow between workstations
4. **Load Configuration**: Load all configuration files into the simulator
5. **Run Simulation**: Process all customer orders through the assembly line
6. **Analyze Results**: Review completed orders, incomplete orders, and remaining inventory
7. **Optimize Production**: Adjust station order, inventory levels, or other parameters based on results

## Testing Framework

The simulator includes comprehensive testing capabilities:

1. **Batch Mode Testing**: Automatically test different assembly line configurations
2. **Data Validation**: Verify station, order, and assembly line data integrity
3. **Error Handling**: Robust error detection and reporting during simulation
4. **Performance Testing**: Measure throughput and identify bottlenecks

### Building and Running Tests

The project uses CTest for managing tests. To build and run the tests:

```bash
# In the build directory
cmake .. -DBUILD_TESTING=ON
cmake --build .
ctest --verbose
```

You can also run the tests in batch mode to quickly validate the entire system:

```bash
# Run the batch mode testing script
./bin/simulator data/Stations1.txt data/Stations2.txt data/CustomerOrders.txt data/AssemblyLine.txt
```

This will process all customer orders through the configured assembly line and display detailed results of the simulation, including completed orders, incomplete orders, and remaining inventory. This approach is useful for regression testing when making changes to the codebase.

## Sample Output

```
Line Manager Iteration: 1
    Filled John M., Kids Bedroom [Bed]
    Filled Rania A., 2-Room Home Office and Bedroom [Bed]
    Filled Hong H., 1-Room Office [Desk]

Line Manager Iteration: 2
    Filled Chris S., Bedroom [Bed]
    Filled Cornel B., 1-Room Home Office [Office Chair]
    Filled John M., Kids Bedroom [Nighttable]

========================================
=      Processed Orders (complete)     =
========================================
John M. - Kids Bedroom
[123457] Bed                            - FILLED
[123458] Bed                            - FILLED
[000889] Nighttable                     - FILLED
[056790] Dresser                        - FILLED
[459215] Desk                           - FILLED

========================================
=     Processed Orders (incomplete)    =
========================================
Rania A. - 2-Room Home Office and Bedroom
[123461] Bed                            - FILLED
[000890] Nighttable                     - FILLED
[147857] Office Chair                   - TO BE FILLED
[459216] Desk                           - TO BE FILLED

========================================
=          Inventory (full)            =
========================================
001 | Armchair       | 654325 |    7 | Upholstered Wing Chair
002 | Bed            | 123461 |    2 | Queen size bed with headboard
003 | Dresser        | 056792 |    4 | 6-Drawer Unit
004 | Nighttable     | 000892 |    2 | Nightstand with 2 drawers
005 | Filing Cabinet | 987659 |    2 | 3-drawer filing cabinet
006 | Office Chair   | 147862 |   15 | High-back carpet-rollers
007 | Bookcase       | 000992 |    2 | 5-shelf open bookcase
008 | Desk           | 459219 |    0 | 6-foot worktable
```

## Acknowledgments

- Designed based on real-world manufacturing processes
- Optimized through consultation with production engineers
- Built using modern C++ techniques for reliability and performance

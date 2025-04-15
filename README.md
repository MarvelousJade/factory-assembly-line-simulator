# Factory Assembly Line Simulator

A robust C++ application for simulating manufacturing assembly lines, allowing companies to model and optimize their production processes.

## Overview

The Factory Assembly Line Simulator enables production managers and engineers to:

- Create a virtual model of any manufacturing assembly line
- Process customer orders through a sequence of workstations
- Track inventory levels across the production line
- Identify and resolve bottlenecks in the manufacturing process
- Predict order completion rates and optimize resource allocation

## Key Features

- **Configurable Workstations**: Define any number of stations with custom inventory and processing capabilities
- **Dynamic Order Management**: Handle a variety of customer orders with different requirements
- **Real-time Inventory Tracking**: Monitor stock levels at each workstation
- **Detailed Reporting**: Get comprehensive reports on completed orders, incomplete orders, and remaining inventory
- **Flexible Configuration**: Set up your assembly line through easy-to-edit text files

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
- CMake 3.12 or higher (optional for easier building)

### Building from Source

```bash
# Clone this repository
git https://github.com/MarvelousJade/factory-assembly-line-simulator.git

# Navigate to the project directory
cd factory-assembly-line

# Option 1: Using CMake
mkdir build
cd build
cmake ..
make

# Option 2: Direct compilation
g++ -Wall -std=c++17 -o simulator *.cpp
```

## Usage

The simulator can be run in two modes:

### Interactive Mode

```bash
./simulator
```

This launches a menu-driven interface where you can:
1. Load station data files
2. Load customer orders
3. Configure the assembly line
4. Display workstations and orders
5. Run the simulation
6. View detailed results

### Batch Mode

```bash
./simulator Stations1.txt Stations2.txt CustomerOrders.txt AssemblyLine.txt
```

This runs the entire simulation from the provided configuration files and outputs the results.

## Configuration Files

The simulator uses text files for configuration:

### Station Files

Define the workstations and their inventory:

```
Armchair,654321,10,Upholstered Wing Chair
Bed,123456,5,Queen size bed with headboard
Dresser,56789,7,6-Drawer Unit
```

Each line contains: `ItemName,SerialNumber,Quantity,Description`

### Customer Order File

Define customer orders with required items:

```
John M.|Kids Bedroom|Bed|Bed|Nighttable|Dresser|Desk
Rania A.|Home Office|Desk|Office Chair|Filing Cabinet|Bookcase
```

Each line contains: `CustomerName|ProductName|Item1|Item2|...|ItemN`

### Assembly Line Configuration File

Define the flow between workstations:

```
Nighttable|Desk
Dresser|Armchair
Bed|Dresser
```

Each line contains: `CurrentStation|NextStation`

## Example Workflow

1. Start by defining your workstations in station files
2. Create customer orders in an orders file
3. Configure the assembly line flow
4. Load these files into the simulator
5. Run the simulation to process all orders
6. View detailed reports on order completion and remaining inventory

## Sample Output

```
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
=          Inventory (full)            =
========================================
001 | Armchair       | 654325 |    7 | Upholstered Wing Chair
002 | Bed            | 123461 |    2 | Queen size bed with headboard
003 | Dresser        | 056792 |    4 | 6-Drawer Unit
```

## Performance Considerations

- The simulation is optimized for manufacturing lines with up to 100 stations
- Can process hundreds of customer orders efficiently
- Memory usage scales linearly with the number of stations and orders

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Designed based on real-world manufacturing processes
- Optimized through consultation with production engineers
- Built using modern C++ techniques for reliability and performance

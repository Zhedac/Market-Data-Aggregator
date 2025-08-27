# Market Data Processor

## Overview
The Market Data Processor is a high-performance C++ application designed to efficiently merge and process market data from numerous symbol files. It employs an **external merge sort** algorithm to handle datasets larger than available memory, reading market data entries, sorting them by timestamp, and outputting a consolidated file suitable for back-testing systems.

## Features
- **Scalable Data Handling**: Efficiently processes large volumes of market data (over 100 GB) by utilizing an external merge sort approach, minimizing in-memory footprint.
- **Memory-Aware Multithreading**: Supports multithreading for improved performance on multi-core systems, dynamically adjusting thread and batch sizes based on available system memory to prevent out-of-memory errors.
- **Efficient I/O**: Reads and writes data line by line, ensuring low memory consumption even with very large files.
- **Guaranteed Sorting**: Ensures that entries are primarily sorted by Timestamp(9), and entries with the same timestamp are further sorted alphabetically by symbol.

## Project Structure
```
market-data-processor
├── src
│   ├── main.cpp
│   ├── data_processor
│   │   ├── processor.hpp
│   │   └── processor.cpp
│   ├── file_handler
│   │   ├── reader.hpp
│   │   ├── reader.cpp
│   │   ├── writer.hpp
│   │   └── writer.cpp
│   ├── models
│   │   ├── market_data.hpp
│   │   └── market_data.cpp
│   └── utils
│       ├── memory_monitor.hpp
│       ├── memory_monitor.cpp
│       ├── thread_pool.hpp
│       └── thread_pool.cpp
├── tests
│   ├── test_processor.cpp
│   ├── test_reader.cpp
│   └── test_writer.cpp
├── trades
│   ├── AAPL.txt
│   ├── MSFT.txt
│   └── CSCO.txt
├── CMakeLists.txt
├── Makefile
└── README.md
```

## Setup Instructions(For Windows)
1.  **Build the project**:
    The project uses CMake for building.
    ```
    cd market-data-processor
    cmake . -B build -G "MinGW Makefiles"
    cmake --build build
    ```

2.  **Run the application**:
    ```
    build/MarketDataProcessor.exe <input-directory> <output-file>
    ```
    (For running on only certain no. of files from input-directory, use: ```build/MarketDataProcessor.exe <input-directory> <output-file> number-of-files```)

## Usage
-   The application takes two command-line arguments:
    1.  The directory containing the market data files (e.g., `trades/`).
    2.  The name of the output file where the merged data will be written.

## Testing
-   Unit tests are provided for the Processor, Reader, and Writer classes. To run the tests, use:
    ```
    cd build
    ctest
    ```
## Data Generation
-   Faker Script named generate_fake_data.py
-   Will need Python environment to run this script.
    ```
    python your_script_name.py --rows 50000 --files 500 --output_dir "/path/to/trades/folder"
    ```
*   Default Value:
    * rows: 100000
    * files: 1000
    * output_dir: trades/
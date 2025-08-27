#include <gtest/gtest.h>
#include "../src/data_processor/processor.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>

TEST(ProcessorTest, ProcessAndMerge) {
    // Create a dummy directory and some dummy files
    std::filesystem::create_directory("test_trades");
    std::ofstream("test_trades/AAPL.txt") << "Timestamp,Price,Size,Exchange,Type\n";
    std::ofstream("test_trades/GOOG.txt") << "Timestamp,Price,Size,Exchange,Type\n";

    // Add a bunch of lines to each file
    for (int i = 0; i < 1000; ++i) {
        std::ofstream("test_trades/AAPL.txt", std::ios_base::app) << "2023-10-27 10:00:00.000,170.0,100,NASDAQ,Trade\n";
        std::ofstream("test_trades/GOOG.txt", std::ios_base::app) << "2023-10-27 10:00:01.000,130.0,50,NASDAQ,Trade\n";
    }

    Processor processor;
    const std::string output_file = "merged_trades_test.txt";
    processor.processAndMerge("test_trades", output_file, 2);

    std::ifstream ifs(output_file);
    std::string line;
    std::vector<std::string> lines;
    // Skipping header
    std::getline(ifs, line);
    while (std::getline(ifs, line)) {
        lines.push_back(line);
    }
    ifs.close();

    ASSERT_EQ(lines.size(), 2000);

    // Cleanup
    std::filesystem::remove_all("test_trades");
    std::filesystem::remove(output_file);
}

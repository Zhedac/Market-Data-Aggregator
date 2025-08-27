#include <gtest/gtest.h>
#include "../src/file_handler/writer.hpp"
#include "../src/models/market_data.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

TEST(WriterTest, WriteData) {
    const std::string test_output_file = "test_output.txt";
    {
        Writer writer(test_output_file, false);
        MarketData md("AAPL", "2023-10-27 10:00:00.000", 170.0, 100, "NASDAQ", "Trade");
        writer.writeData(md);
    }

    std::ifstream ifs(test_output_file);
    std::string line;
    std::getline(ifs, line);
    ifs.close();

    ASSERT_EQ(line, "AAPL,2023-10-27 10:00:00.000,170.000000,100,NASDAQ,Trade");

    std::filesystem::remove(test_output_file);
}

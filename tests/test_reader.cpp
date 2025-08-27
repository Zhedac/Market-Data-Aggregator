#include <gtest/gtest.h>
#include "../src/file_handler/reader.hpp"
#include <fstream>
#include <filesystem>

TEST(ReaderTest, GetAllTradeFilePaths) {
    // Create a dummy directory and some dummy files
    std::filesystem::create_directory("test_trades");
    std::ofstream("test_trades/AAPL.txt");
    std::ofstream("test_trades/GOOG.txt");

    Reader reader;
    std::vector<std::string> filePaths = reader.getAllTradeFilePaths("test_trades", 2);

    ASSERT_EQ(filePaths.size(), 2);

    // Cleanup
    std::filesystem::remove_all("test_trades");
}

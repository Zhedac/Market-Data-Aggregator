# tests/CMakeLists.txt

# Add each test executable
add_executable(test_processor test_processor.cpp)
target_link_libraries(test_processor PRIVATE MarketDataLib gtest_main)
add_test(NAME test_processor COMMAND test_processor)

add_executable(test_reader test_reader.cpp)
target_link_libraries(test_reader PRIVATE MarketDataLib gtest_main)
add_test(NAME test_reader COMMAND test_reader)

add_executable(test_writer test_writer.cpp)
target_link_libraries(test_writer PRIVATE MarketDataLib gtest_main)
add_test(NAME test_writer COMMAND test_writer)

if(MINGW)
    target_link_libraries(test_processor PRIVATE stdc++fs)
    target_link_libraries(test_reader PRIVATE stdc++fs)
    target_link_libraries(test_writer PRIVATE stdc++fs)
endif()
/**
	Example test file that verifies that the Makefile actually considers
	both .h and .cpp files
 */
 
#include <gtest/gtest.h>

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}

TEST(ExampleTest1, SimpleTest1) {
    int actual = 32;
    EXPECT_GT(actual, 15);
}
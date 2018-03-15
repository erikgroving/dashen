/**
	Example test file that verifies that the Makefile actually considers
	both .h and .cpp files
 */
 
#include <gtest/gtest.h>
#include "example_test1.h"

int main(int argc, char **argv) {
	printf("%s", header_test_function());
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}

const char* header_test_function() {
	return "Example Test for Makefile";
}

TEST(ExampleTest1, SimpleTest1) {
    int actual = 32;
    EXPECT_GT(actual, 15);
}
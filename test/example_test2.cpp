#include <gtest/gtest.h> // googletest header file

TEST(ExampleTest2, SimpleTest2) {
    int actual = 1;
    EXPECT_GT(actual, 0);
}
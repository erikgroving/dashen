#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include "../src/searchclient/command.h"
#include "../src/searchclient/typedefs.h"

using SearchClient::Command;

TEST(CommandTests, CommandTest1) {
    Direction d1 = E;
    EXPECT_EQ(d1, E);
    Command command(MOVE, E);
    EXPECT_EQ(command.toString(), "Move(E)");
}
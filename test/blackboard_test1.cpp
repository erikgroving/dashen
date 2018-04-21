#include <gtest/gtest.h>

#include "../src/searchengine/strategy.h"
#include "../src/searchclient/agent.h"
#include "../src/searchclient/client.h"
#include "../src/searchclient/blackboard.h"
#include "../src/searchclient/blackboardentry.h"
#include "../src/searchengine/typedefs.h"
#include "../src/strategies/strategies"
using SearchClient::Blackboard;
using SearchClient::BlackboardEntry;
TEST(BlackboardTest1, AddEntry) {
    SearchClient::Blackboard blackboard;

    SearchEngine::Strategy *strat = new Strat::StrategyDFS();

    SearchClient::Client client(strat);
    SearchClient::Agent dummyClient(Color::YELLOW, 0, Coord(), strat, &client);

    BlackboardEntry::EntryType dummy1 = BlackboardEntry::create(BlackboardEntry::EntryType::POSITION_ENTRY, dummyClient);
    BlackboardEntry::EntryType dummy2 = BlackboardEntry::create(BlackboardEntry::EntryType::GLOBAL_GOAL_ENTRY, dummyClient);
    BlackboardEntry::EntryType dummy3 = BlackboardEntry::create(BlackboardEntry::EntryType::MOVE_HELP_GOAL_ENTRY, dummyClient);

    dummy1.setPosition(Coord(1,1));

    blackboard.addEntry(dummy1);
    blackboard.addEntry(dummy2);
    blackboard.addEntry(dummy3);

    ASSERT_EQ(dummy1.getPosition().x, 1) << "The x position was not updated";
    ASSERT_EQ(dummy1.getPosition().y, 1) << "The y position was not updated";

    ASSERT_EQ(blackboard.getPositionEntries().size(), 1) << "Wrong amount of PositionEntry entries are registered (" << blackboard.getPositionEntries().size() << ")";
    ASSERT_EQ(blackboard.getGoalEntries().size(), 2) << "Wrong amount of GoalEntry entries are registered (" << blackboard.getGoalEntries().size() << ")";
}
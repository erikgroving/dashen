#include <gtest/gtest.h>

#include "../src/searchengine/strategy.h"
#include "../src/searchclient/agent.h"
#include "../src/searchclient/client.h"
#include "../src/searchclient/blackboard.h"
#include "../src/searchengine/typedefs.h"
#include "../src/strategies/strategies"
using SearchClient::Blackboard;
TEST(BlackboardTest1, AddEntry) {
    SearchClient::Blackboard blackboard;

    SearchEngine::Strategy *strat = new Strat::StrategyDFS();

    SearchClient::Client client(strat);
    SearchClient::Agent dummyClient(Color::YELLOW, 0, Coord(), strat, &client);

    Blackboard::Entry dummy1 = SearchClient::Blackboard::Entry::create(SearchClient::Blackboard::Entry::PositionEntry, dummyClient);
    Blackboard::Entry dummy2 = SearchClient::Blackboard::Entry::create(SearchClient::Blackboard::Entry::GlobalGoalEntry, dummyClient);
    Blackboard::Entry dummy3 = SearchClient::Blackboard::Entry::create(SearchClient::Blackboard::Entry::MoveHelpGoalEntry, dummyClient);

    dummy1.setPosition(Coord(1,1));

    blackboard.addEntry(dummy1);
    blackboard.addEntry(dummy2);
    blackboard.addEntry(dummy3);

    ASSERT_EQ(dummy1.getPosition().x, 1) << "The x position was not updated";
    ASSERT_EQ(dummy1.getPosition().y, 1) << "The y position was not updated";

    ASSERT_EQ(blackboard.getPositionEntries().size(), 1) << "Wrong amount of PositionEntry entries are registered (" << blackboard.getPositionEntries().size() << ")";
    ASSERT_EQ(blackboard.getGoalEntries().size(), 2) << "Wrong amount of GoalEntry entries are registered (" << blackboard.getGoalEntries().size() << ")";
}
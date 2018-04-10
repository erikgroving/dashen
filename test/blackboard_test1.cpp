#include <gtest/gtest.h>

#include "../src/searchclient/agent.h"
#include "../src/searchclient/blackboard.h"
#include "../src/searchengine/typedefs.h"

TEST(BlackboardTest1, AddEntry) {
    SearchClient::Blackboard blackboard;

    SearchClient::Agent dummyClient(Color::YELLOW, 0, nullptr, nullptr);
    
    Entry dummy1 = SearchClient::Blackboard::Entry::create(SearchClient::Blackboard::Entry::PositionEntry);
    Entry dummy2 = SearchClient::Blackboard::Entry::create(SearchClient::Blackboard::Entry::GlobalGoalEntry);
    Entry dummy3 = SearchClient::Blackboard::Entry::create(SearchClient::Blackboard::Entry::MoveHHelpGoalEntry);

    dummy1.setPosition(Coord(1,1));

    blackboard.addEntry(dummy1);
    blackboard.addEntry(dummy2);
    blackboard.addEntry(dummy3);

    ASSERT_EQ(dummy.getPosition().x, 1);
    ASSERT_EQ(dummy.getPosition().y, 1);

    ASSERT_EQ(Blackboard.getPositionEntries().size(), 1);
    ASSERT_EQ(Blackboard.getGoalEntries().size(), 2);
}
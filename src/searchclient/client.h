#ifndef SEARCHENGINE_CLIENT_H
#define SEARCHENGINE_CLIENT_H

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <regex>
#include <unordered_map>
#include <utility>

#include "../searchengine/searchengine"
#include "agent.h"
#include "jointaction.h"

Color stringToColor(std::string);


namespace SearchClient {

class Agent;

class Client {
    public:
        /* Only a default constructor is required. The client class is the main one of the program
         * and is responsible for all the other elements.
         */
        Client(SearchEngine::Strategy *strategy);

    public:
        /**
         * Describes the type of problem that is required to solve.
         * SingleAgent describes a problem with a single agent, MultiAgent describes a problem with
         * several agents. None describes a problem with no agent.
         * 
         * Note: the type_ attributes (notably retrievable through getProblemType()) is dynamically
         * determined during state initialization.
         */
        enum ProblemType {
            None,
            SingleAgent,
            MultiAgent
        };


        /**
         * Read the standard input and construct the initial state from the data received.
         * Note: the agents are dynamically created in this method.
         */
        SearchEngine::State initState();

        /**
         * Extract the "Agent" units that are the intelligent objects capable
         * of searching in various ways.
         * 
         * A const and not-const version of the method are available.
         */
        std::vector<SearchClient::Agent>& extractAgents() { return agents; }
        const std::vector<SearchClient::Agent>& extractAgents() const { return agents; }

        /**
         * Sets the next action of the agent at agentIndex to be command.
         */
        void setAction(size_t agentIndex, const SearchEngine::Command &command);

        /**
         * Returns the ProblemType of the received problem instance.
         * Cf. ProblemType definition.
         */
        ProblemType getProblemType() const { return type_; }
        const std::vector<JointAction>& getCurrentActionPlan() const { return actionPlan_; }
    
        /**
         * Writes a whole plan to the standard output
         */
        bool send(const std::vector<JointAction> &plan, size_t *failingStep = 0);                 

        /**
         * Writes a single step to the standard output.
         * Note: a step is described by a joint action of all the agents at the same time.
         */
        bool sendStep(const JointAction &jointAction);
    

    private:
        /**
         * Saves onGoingJointAction in actionPlan_. resetJointAction can be safely called right after.
         */
        void saveJointAction();
        /**
         * Resets onGoingJointAction so that new actions can be stored.
         */
        void resetJointAction();

    private:

        ProblemType type_;
        
        JointAction onGoingJointAction;
        std::vector<JointAction> actionPlan_;

        std::vector<SearchClient::Agent> agents;
        int actionsRecv;  

        SearchEngine::Strategy *searchStrategy_;
};

}

#endif

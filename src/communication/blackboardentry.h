#ifndef COMMUNICATION_BLACKBOARDENTRY_H
#define COMMUNICATION_BLACKBOARDENTRY_H

#include <vector>
#include "../communication/blackboard.h"
#include "../searchengine/typedefs.h"
#include "../searchclient/agent.h"

namespace Communication {

class BlackboardEntry {

    public: // Constructors
        BlackboardEntry();
        BlackboardEntry(const BlackboardEntry& src);
        virtual ~BlackboardEntry() {}
    public: // Operators
        virtual BlackboardEntry& operator=(const BlackboardEntry &src);
        virtual bool operator==(const BlackboardEntry &compared) const;

    public: // Common attributes - getters

        /**
         * \return the parent blackboard or nullptr if none is defined
         */
        Blackboard* getParent() { return parent_; }
        const Blackboard* getParent() const { return parent_; }

        /**
         * \return the location of the entry
         */
        Coord getLocation() const { return position_; }

        /**
         * \return Id of the entry's author
         */
        int getAuthorId() const { return authorId_; }

        /**
         * \return time step at which the entry is at
         */
        unsigned int getTimeStep() const { return timeStep_; }

    public: // Common attributes - setters
        /**
         * \brief Sets the entry's parent blackboard
         */
        void setBlackboardParent(Blackboard *parent) { parent_ = parent; }

        /**
         * \brief Sets the entry's author id
         */
        void setAuthorId(int authorId) { authorId_ = authorId; }

        /**
         * \brief Sets the location of the entry
         */
        void setLocation(const Coord &position) { position_ = position; }

        /**
         * \brief Sets the timestep of the entry
         */
        void setTimeStep(int timeStep) { timeStep_ = timeStep; }

        /**
         * \brief Agent receiver accepts the entry, thus it can be removed from the parent parent
         */
        static bool accept(BlackboardEntry *entry, const SearchClient::Agent &receiver);

        /**
         * \brief Agent sender revokes the entry that it has previously created
         */
        static bool revoke(BlackboardEntry *entry, const SearchClient::Agent &sender);

private:
        Blackboard *parent_;
        Coord position_; // Position where the Entry 
        int authorId_;     // ID of the agent that created the Entry 

        unsigned int timeStep_;
};

}
#endif

#include "state_equal_to.h"

bool StateCompare::operator()(SearchEngine::State* const& source, SearchEngine::State* const& compared) const {

   // std::cerr << "(Custom Explored)";
    if(compared == nullptr) return false;
    if(source == compared) {
        if(debug_)
            std::cerr << "Equal because the adresses are the same" << std::endl;
        return true;
    }

    for(const AgentDescription &ad: source->getAgents())
        if(!SearchEngine::Predicate::agentAt(compared, ad.loc.x, ad.loc.y)) {
            // std::cerr << "Refused because an agent is different" << std::endl;
            return false;
        } 

    for(const Box &box: source->getBoxes())
        if(!SearchEngine::Predicate::boxAt(compared, box.loc.x, box.loc.y)) {
            // std::cerr << "Refused because a box is not the same" << std::endl;
            return false;
        }

    if(debug_)
        std::cerr << "Equal because all the attributes match" << std::endl;

    return true;
}   


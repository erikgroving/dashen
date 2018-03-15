#ifndef SEARCH_CLIENT_H
#define SEARCH_CLIENT_H

#include <vector>
#include <iostream>
#include "strategy.h"

namespace SearchEngine {

class SearchClient {

public:
    SearchClient(/* State initialState */);
    
    void search(Strategy &strategy);
    
private:
    // State initialState_;
};

};
#endif
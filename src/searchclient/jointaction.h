#ifndef SEARCHCLIENT_JOINTACTION
#define SEARCHCLIENT_JOINTACTION

#include "../searchengine/command.h"
#include <vector>
#include <string>

namespace SearchClient {

class JointAction {
    public:
        JointAction(): data() {}
        JointAction(const JointAction &src): data(src.getData()) {
            
        }
        JointAction& operator=(const JointAction &src) {
            data = src.getData();
            return *this;
        }

        SearchEngine::Command& operator[](size_t index) {
            return data[index];
        }
        
        const std::vector<SearchEngine::Command>& getData() const { return data; }
        std::vector<SearchEngine::Command>& getData() { return data; }

        void setAction(int index, const SearchEngine::Command &command) { data[index] = command; }
        void initialize(size_t size) { 
            data.clear();
            data.resize(size);
            for(size_t i = 0; i < size; i++)
                data[i] = SearchEngine::Command();
        }

        std::string toActionString() {
           std::string ret = "[";
           for (SearchEngine::Command cmd : data) {
               ret += cmd.toString() + ",";
           }
           ret[ret.length() - 1] = ']';
           return ret;
        }


    private:
        std::vector<SearchEngine::Command> data;
};

}
#endif
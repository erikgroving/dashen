#ifndef SEARCHENGINE_COMMAND_H
#define SEARCHENGINE_COMMAND_H

#include <string>
#include "typedefs.h"

#include <vector>

namespace SearchEngine {

    struct Command {
            Action action;
            Direction d1;
            Direction d2;

            /* Constructors for the Command struct. */
            Command(Action type = NOOP);
            Command(Action type, Direction dir1);
            Command(Action type, Direction dir1, Direction dir2);
            Command(const Command &src);

            enum Operation {
                /* No opeartion */
                NoOperation,

                /* Move operation */
                Move_N, Move_S, Move_E, Move_W,

                /* Push Operation */ 
                Push_NN, Push_NW, Push_WN, Push_WW,
                Push_WS, Push_SW, Push_EN, Push_NE,
                Push_EE, Push_ES, Push_SE, Push_SS,
                
                /* Pull Operation */ 
                Pull_NS, Pull_NW, Pull_WN, Pull_WE,
                Pull_WS, Pull_SW, Pull_SN, Pull_NE,
                Pull_EN, Pull_EW, Pull_ES, Pull_SE
            };

            static int colToInt(Direction dir);
            static int rowToInt(Direction dir);

            static std::vector<Command> possibleActions;

            std::string actionToString(Action a) const;
            std::string dirToString(Direction a) const ;
            std::string toString() const;
            std::string toActionString() const;       

    };

}

#endif

#ifndef SELECT_COMMAND
#define SELECT_COMMAND

#include "fake_db.h"

namespace fake_db
{
    class SelectCommand : public BaseInstruction, QueryUDS
    {
    public:
        void operator()(std::string& str) override;
        virtual ~SelectCommand(){};
    public:
        bool isRightSyntax() override;
        //selects data from the selected DB
        void selectDataToTheDB();
        //prints the selected data
        void print();
    };
}

#endif //SELECT_COMMAND

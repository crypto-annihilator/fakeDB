#ifndef INSERT_COMMAND
#define INSERT_COMMAND

#include "fake_db.h"

namespace fake_db
{
    class InsertCommand : public BaseInstruction, QueryIUDS
    {
    public:
        void operator()(std::string& str) override;
        virtual ~InsertCommand(){};
    public:
        bool isRightSyntax() override;
        bool isPropertyTrue(int index) const override;
        //Adds information to the selected DB
        void addDataToTheDB();
    };
}


#endif //INSERT_COMMAND

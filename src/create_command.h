#ifndef CRAETE_COMMAND
#define CRAETE_COMMAND

#include "fake_db.h"

namespace fake_db
{
    class CreateCommand : public BaseInstruction, QueryCRUD
    {
    public:
        void operator()(std::string& str) override;
        virtual ~CreateCommand(){};
    public:
        bool isRightSyntax() override;
        bool isTypeTrue(std::string&) const;
        //Adds the created DB to the database list
        void addNameToTheDBList();
    };
}

#endif //CRAETE_COMMAND

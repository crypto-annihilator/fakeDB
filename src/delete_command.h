#ifndef DELETE_COMMAND
#define DELETE_COMMAND

#include "fake_db.h"

namespace fake_db
{
    class DeleteCommand : public BaseInstruction, QueryUDS
    {
     public:
         void operator()(std::string& str) override;
         virtual ~DeleteCommand(){};
     public:
         bool isRightSyntax() override;
         //delete DB data
         void deleteDataToTheDB();
         //updates DB data
         void updateFileDB();
         //delete DB
         void deleteDB();
         //removes the DB name from the database list
         void removFileNameOfDBList();
    };
}

#endif //DELETE_COMMAND

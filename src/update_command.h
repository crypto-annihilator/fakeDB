#ifndef UPDATE_COMMAND
#define UPDAET_COMMAND

#include "fake_db.h"

namespace fake_db
{
    class UpdateCommand : public BaseInstruction, QueryUDS
    {
    public:
        void operator()(std::string& str) override;
        virtual ~UpdateCommand(){};
    public:
        bool isRightSyntax() override;
        bool isTypeTrue(int index) const override;
        //updates DB data
        void updateDataToTheDB();
        void helpUpdateDataToTheDB(std::string);
        //updates DB list
        void updateFileDB();
    private:
        int indexWhere;
    };
}

#endif //UPDATE_COMMAND

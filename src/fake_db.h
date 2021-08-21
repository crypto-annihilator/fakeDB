#ifndef FAKE_DB_H
#define FAKE_DB_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>

namespace fake_db
{
    //############## Command Design Pattern ##################
    class BaseInstruction
    {
    public:
        virtual void operator()(std::string& str) = 0;
    };

    //############## Factory Design Pattern ##################
    class CommandFactory
    {
    public:
        CommandFactory();
        static BaseInstruction* produce(std::string& str);
    private:
        static std::unordered_map<std::string, BaseInstruction*> commands;
    };

    class Query
    {
    public:
        virtual void execute (std::string& str);
        virtual ~Query(){};
    public:
        //tokenizer
        void parse_token();
        //checks whether such a DB already exists
        bool doesSuchDBExist(int index, std::string& str) const;
        //checks if the entered line matches the DB syntax
        virtual bool isRightSyntax() = 0;
    protected:
        std::string m_instruct;
        std::vector<std::string> m_vec;
    };

    class QueryIUDS : public Query
    {
    public:
        QueryIUDS(){};
        virtual ~QueryIUDS(){};
    public:
        //checks if the given DB properties are entered correctly
        virtual bool isPropertyTrue (int index) const;
        //stores data describing DB
        void saveInfoDB(std::string&);
        //checks if the property type matches
        virtual bool isTypeTrue(int index) const;
    protected:
        std::fstream m_file;
        std::vector<std::string> m_infoDB;
    };

    class QueryUDS : public QueryIUDS
    {
    public:
        QueryUDS(){};
        virtual ~QueryUDS(){};
    public:
        //stores DB data
        void saveDataDB(int index);
        void helpSaveDataDB(std::string);
    protected:
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_dataDB;
    };

    class CreateCommand : public BaseInstruction, Query
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
#endif //FAKE_DB_H


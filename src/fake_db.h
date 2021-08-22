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

    class QueryCRUD
    {
    public:
        virtual void execute (std::string& str);
        virtual ~QueryCRUD(){};
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

    class QueryIUDS : public QueryCRUD
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
}
#endif //FAKE_DB_H

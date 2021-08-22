#include "create_command.h"
#include "insert_command.h"
#include "update_command.h"
#include "delete_command.h"
#include "select_command.h"
#include "fake_db.h"
#include <exception>
#include <string>

namespace fake_db
{
    CommandFactory::CommandFactory()
    {
        commands["Create"] = new CreateCommand;
        commands["Insert"] = new InsertCommand;
        commands["Update"] = new UpdateCommand;
        commands["Delete"] = new DeleteCommand;
        commands["Select"] = new SelectCommand;
    }

    std::unordered_map<std::string, BaseInstruction*> CommandFactory::commands;

    BaseInstruction* CommandFactory::produce(std::string& str)
    {
        if (CommandFactory::commands.find(str) == CommandFactory::commands.end()) {
            throw std::invalid_argument("Wrong instruction");
        }
        return CommandFactory::commands[str];
    }

    void QueryCRUD::execute(std::string& str)
    {
        m_instruct = str;
        parse_token();
    }

    void QueryCRUD::parse_token()
    {
        std::string str = "";
        for (int i = 0; i < m_instruct.size(); ++i) {
            if (m_instruct[i] != ' ' && m_instruct[i] != '(' && m_instruct[i] != ':' &&
                    m_instruct[i] != ',' && m_instruct[i] != ')' && m_instruct[i] != '=' && m_instruct[i] != ',') {
                str += m_instruct[i];
            } else {
                if (str != "") {
                    m_vec.push_back(str);
                }
                if (m_instruct[i] != ' ') {
                    str.clear();
                    str += m_instruct[i];
                    m_vec.push_back(str);
                }
                str.clear();
            }
        }
        if(str != "") {
            m_vec.push_back(str);
        }
    }

    bool QueryCRUD::doesSuchDBExist(int index, std::string& str) const
    {
        std::ifstream file("db_list.txt");
        std::string nameDB;
        while (std::getline(file, str)) {
            size_t pos = str.find(" ");
            nameDB = str.substr(0, pos);
            if (nameDB == m_vec[index]) {
                return true;
            }
        }
        return false;
    }

    bool QueryIUDS::isTypeTrue(int index) const
    {
        try {
            if (m_infoDB.at(index - 2) == "string") {
                return true;
            } else if (m_infoDB.at(index - 2) == "int") {
                std::stoi(m_vec[index]);
                return true;
            } else if (m_infoDB.at(index - 2) == "char") {
                if (m_vec[index].size() != 1) {
                    return false;
                }
                return true;
            } else if (m_infoDB.at(index - 2) == "double") {
                std::stod(m_vec[index]);
                return true;
            } else if (m_infoDB.at(index - 2) == "float") {
                std::stof(m_vec[index]);
                return true;
            } else {
                return false;
            }
        } catch (std::out_of_range const& exc) {
            return false;
        }
    }

    bool QueryIUDS::isPropertyTrue(int index) const
    {
        for (int i = 2; i < m_infoDB.size(); i += 4) {
            if (m_vec[index] == m_infoDB[i]) {
                return true;
            }
        }
        return false;
    }

    void QueryIUDS::saveInfoDB(std::string& str)
    {
        size_t pos = str.find(" ");
        if (pos != std::string::npos) {
            m_infoDB.push_back(str.substr(0, pos));
            str = str.substr(pos + 1);
            saveInfoDB(str);
        } else {
            m_infoDB.push_back(str);
        }
    }

    void QueryUDS::saveDataDB(int index)
    {
        m_file.close();
        m_file.open(m_vec[index], std::fstream::in);
        std::string str;
        while (std::getline(m_file, str)) {
            helpSaveDataDB(str);
        }
        m_file.close();
    }

    void QueryUDS::helpSaveDataDB(std::string line)
    {
        size_t pos = line.find(" ");
        std::string id = line.substr(0, pos);
        line = line.substr(pos + 1);
        for (int i = 0; i < line.size(); ++i) {
            if (line[i] == ' ') {
                line = line.substr(1);
                continue;
            }
            pos = line.find("=");
            size_t pos1 = line.find(",");
            m_dataDB[id][line.substr(0, pos)] = line.substr(pos + 1, pos1 - pos - 1);
            line = line.substr(pos1 + 2);
        }
    }
}

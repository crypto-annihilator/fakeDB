#include "fake_db.h"
#include <exception>

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

    void Query::execute(std::string& str)
    {
        m_instruct = str;
        parse_token();
    }

    void Query::parse_token()
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

    bool Query::doesSuchDBExist(int index, std::string& str) const
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

    //################### Create #####################

    void CreateCommand::operator()(std::string& str)
    {
        execute(str);
        if (!isRightSyntax()) {
            throw std::invalid_argument("1");
        }
        std::ofstream creatBD(m_vec[2]);
        creatBD.close();
        addNameToTheDBList();
    }

    bool CreateCommand::isTypeTrue(std::string& str) const
    {
        if (str == "string" || str == "int" || str == "char" ||
                str == "double" || str == "float" || str == "bool") {
            return true;
        }
        return false;
    }

    bool CreateCommand::isRightSyntax()
    {
        if (m_vec[1] != "entity") {
            std::cout << "Wrong instruction" << std::endl;
            return false;
        }
        std::string str = "";
        if (doesSuchDBExist(2, str)) {
            std::cout << "db already exists" << std::endl;
            return false;
        }
        if (m_vec[3] != "(" || m_vec[m_vec.size() - 1] != ")") {
            std::cout << "Wrong instruction" << std::endl;
            return false;
        }
        for (int i = 5; i < m_vec.size() - 1; i += 4) {
            try {
                if (m_vec.at(i) != ":" || !isTypeTrue(m_vec.at(i + 1)) ||
                        ((i + 2 < m_vec.size() - 1) && m_vec.at(i + 2) != ",")) {
                    std::cout << "Wrong instruction" << std::endl;
                    return false;
                }
            } catch (std::out_of_range const& exc) {
                std::cout << "Wrong instruction" << std::endl;
                return false;
            }
        }
        return true;
    }

    void CreateCommand::addNameToTheDBList()
    {
        std::fstream file("db_list.txt", std::fstream::out | std::fstream::app);
        for (int i = 2; i < m_vec.size() - 1; ++i) {
            file << m_vec[i] << " ";
        }
        file << m_vec[m_vec.size() - 1] << "\n";
        file.close();
    }

    //################### Insert #####################

    void InsertCommand::operator()(std::string& str)
    {
        execute(str);
        std::string str1 = "";
        if (!doesSuchDBExist(2, str1)) {
            std::cout << "db does not exist" << std::endl;
            throw std::invalid_argument("1");
        } else {
            saveInfoDB(str1);
        }
        if (!isRightSyntax()) {
            throw std::invalid_argument("1");
        }
        addDataToTheDB();
    }

    bool InsertCommand::isPropertyTrue(int index) const
    {
        try {
            if (m_vec[index] == m_infoDB.at(index - 2)) {
                return true;
            }
        } catch (std::out_of_range const& exc) {
            return false;
        }
        return false;
    }


    bool InsertCommand::isRightSyntax()
    {
        if (m_vec[1] != "into") {
            std::cout << "Wrong instruction" << std::endl;
            return false;
        }
        if (m_vec[3] != "(" || m_vec[m_vec.size() - 1] != ")") {
            std::cout << "Wrong instruction" << std::endl;
            return false;
        }
        for (int i = 4; i < m_vec.size() - 1; i += 4) {
            try {
                if (!isPropertyTrue(i)) {
                    std::cout << "Wrong property" << std::endl;
                    return false;
                }
                if (m_vec.at(i + 1) != "=" || ((i + 3 < m_vec.size() - 1) && m_vec.at(i + 3) != ",")) {
                    std::cout << "Wrong instruction" << std::endl;
                    return false;
                }
                if (!isTypeTrue(i + 2)) {
                    std::cout << "Wrong type or out_of_range" << std::endl;
                    return false;
                }
            } catch (std::out_of_range const& exc) {
                std::cout << "Wrong instruction" << std::endl;
                return false;
            }
        }
        return true;
    }

    void InsertCommand::addDataToTheDB()
    {
        std::string id = "0";
        std::string line;
        std::fstream fileDB(m_infoDB[0], std::fstream::in);
        while (std::getline(fileDB, line)) {
            size_t pos = line.find(" ");
            id = line.substr(0, pos);
        }
        long long int ids = std::stoll(id);
        id = std::to_string(ids + 1);
        fileDB.close();
        fileDB.open(m_infoDB[0], std::fstream::out | std::fstream::app);
        fileDB << id << " ";
        for (int i = 4; i < m_vec.size(); i += 4) {
            fileDB << m_vec[i] << "=" << m_vec[i + 2] << ", ";
        }
        fileDB << "\n";
        fileDB.close();
    }


    //################### Update #####################

    void UpdateCommand::operator()(std::string& str)
    {
        execute(str);
        std::string str1 = "";
        if (!doesSuchDBExist(1, str1)) {
            std::cout << "db does not exist" << std::endl;
            throw std::invalid_argument("1");
        } else {
            saveInfoDB(str1);
            saveDataDB(1);
        }
        if (!isRightSyntax()) {
            throw std::invalid_argument("1");
        }
        updateDataToTheDB();
    }

    bool UpdateCommand::isRightSyntax()
    {
        try {
            if (m_vec.at(2) != "set") {
                std::cout << "Wrong instruction" << std::endl;
                return false;
            }
        } catch (std::out_of_range const& exc) {
            std::cout << "Wrong instruction" << std::endl;
            return false;
        }
        for (int i = 3; i < m_vec.size() - 1; i += 3) {
            if (m_vec[i] == "where") {
                indexWhere = i + 1;
                try {
                    if(m_vec.at(i + 1) == ",") {
                        std::cout << "Wrong property" << std::endl;
                        return false;
                    }
                } catch (std::out_of_range const& exc) {
                    std::cout << "Wrong instruction" << std::endl;
                    return false;
                }
                i -= 2;
                continue;
            }
            if (m_vec[i] == ",") {
                try {
                    if(m_vec.at(i + 1) == "where") {
                        std::cout << "Wrong property" << std::endl;
                        return false;
                    }
                } catch (std::out_of_range const& exc) {
                    std::cout << "Wrong instruction" << std::endl;
                    return false;
                }
                i -= 2;
                continue;
            }
            try {
                if (!isPropertyTrue(i)) {
                    std::cout << "Wrong property" << std::endl;
                    return false;
                }
                if (m_vec.at(i + 1) != "=") {
                    std::cout << "Wrong instruction" << std::endl;
                    return false;
                }
                if (!isTypeTrue(i + 2)) {
                    std::cout << "Wrong type or out_of_range" << std::endl;
                    return false;
                }
            } catch (std::out_of_range const& exc) {
                std::cout << "Wrong instruction" << std::endl;
                return false;
            }
        }
        return true;
    }

    void UpdateCommand::updateDataToTheDB()
    {
        std::string id = "";
        bool b = false;
        for (int i = indexWhere; i < m_vec.size(); i += 3) {
            if(m_vec[i] == ",") {
                i -= 2;
                continue;
            }
            for (auto [key, value] : m_dataDB) {
                for (auto [key1, value1] : m_dataDB[key]) {
                    if (key1 == m_vec[i]) {
                        if (value1 == m_vec[i + 2]) {
                            b = true;
                            helpUpdateDataToTheDB(key);
                        }
                    }
                }
            }
        }
        if (!b) {
            std::cout << "the condition is not satisfied" << std::endl;
        }
        updateFileDB();
    }

    void UpdateCommand::helpUpdateDataToTheDB(std::string str)
    {
        for (int i = 3; i < indexWhere - 1; i += 3) {
            if (m_vec[i] == ",") {
                i -= 2;
                continue;
            }
            m_dataDB[str][m_vec[i]] = m_vec[i + 2];
        }
    }

    void UpdateCommand::updateFileDB()
    {
        m_file.close();
        m_file.open(m_vec[1], std::fstream::out);
        for (auto [key, value] : m_dataDB) {
            m_file << key << " ";
            for (auto [key1, value1] : m_dataDB[key]) {
                m_file << key1 << "=" << value1 << ", ";
            }
            m_file << "\n";
        }
    }


    //################ Delete ################

    void DeleteCommand::operator()(std::string& str)
    {
        execute(str);
        std::string str1 = "";
        if (m_vec.size() == 2) {
            if (!doesSuchDBExist(1, str1)) {
                std::cout << "db does not exist" << std::endl;
                throw std::invalid_argument("1");
            } else {
                m_file.close();
                const char* c = m_vec[1].c_str();
                removFileNameOfDBList();
                std::remove(c);
            }
        } else if (!doesSuchDBExist(2, str1)) {
            std::cout << "db does not exist" << std::endl;
            throw std::invalid_argument("1");
        } else {
            saveInfoDB(str1);
            saveDataDB(2);
            if (!isRightSyntax()) {
                throw std::invalid_argument("1");
            }
            deleteDataToTheDB();
        }
    }

    bool DeleteCommand::isRightSyntax()
    {
        try {
            if (m_vec.at(1) != "from" && m_vec.at(1) != "all") {
                std::cout << "Wrong instruction" << std::endl;
                return false;
            }
            if (m_vec.size() == 2) {
                return true;
            } else if (m_vec.at(1) != "from") {
                return false;
            }
            if (m_vec.at(3) != "where") {
                std::cout << "Wrong instruction" << std::endl;
                return false;
            }
        } catch (std::out_of_range const& exc) {
            std::cout << "Wrong instruction" << std::endl;
            return false;
        }
        for (int i = 4; i < m_vec.size() - 1; i += 3) {
            if (m_vec[i] == ",") {
                try {
                    if(m_vec.at(i - 1) == "where") {
                        std::cout << "Wrong property" << std::endl;
                        return false;
                    }
                } catch (std::out_of_range const& exc) {
                    std::cout << "Wrong instruction" << std::endl;
                    return false;
                }
                i -= 2;
                continue;
            }
            try {
                if (!isPropertyTrue(i)) {
                    std::cout << "Wrong property" << std::endl;
                    return false;
                }
                if (m_vec.at(i + 1) != "=") {
                    std::cout << "Wrong instruction" << std::endl;
                    return false;
                }
                if (!isTypeTrue(i + 2)) {
                    std::cout << "Wrong type or out_of_range" << std::endl;
                    return false;
                }
            } catch (std::out_of_range const& exc) {
                std::cout << "Wrong instruction" << std::endl;
                return false;
            }
        }
        return true;
    }

    void DeleteCommand::deleteDataToTheDB()
    {
        std::string id = "";
        bool b = false;
        for (int i = 4; i < m_vec.size(); i += 3) {
            if(m_vec[i] == ",") {
                i -= 2;
                continue;
            }
            for (auto [key, value] : m_dataDB) {
                for (auto [key1, value1] : m_dataDB[key]) {
                    if (key1 == m_vec[i]) {
                        if (value1 == m_vec[i + 2]) {
                            b = true;
                        }
                    }
                }
                if (b) {
                    m_dataDB.extract(key);
                }
            }
        }
        if (!b) {
            std::cout << "the condition is not satisfied" << std::endl;
        }
        updateFileDB();
    }

    void DeleteCommand::updateFileDB()
    {
        m_file.close();
        m_file.open(m_vec[2], std::fstream::out);
        for (auto [key, value] : m_dataDB) {
            m_file << key << " ";
            for (auto [key1, value1] : m_dataDB[key]) {
                m_file << key1 << "=" << value1 << ", ";
            }
            m_file << "\n";
        }
    }

    void DeleteCommand::removFileNameOfDBList()
    {
        std::vector<std::string> tmp;
        std::fstream file("db_list.txt", std::fstream::in);
        std::string str;
        std::string nameDB;
        while (std::getline(file, str)) {
            size_t pos = str.find(" ");
            nameDB = str.substr(0, pos);
            if (nameDB != m_vec.at(1)) {
                tmp.push_back(str);
            }
        }
        file.close();
        file.open("db_list.txt", std::fstream::out);
        for (int i = 0; i < tmp.size(); ++i) {
            file << tmp[i] << "\n";
        }
    }

    //################ Select ################

    void SelectCommand::operator()(std::string& str)
    {
        execute(str);
        std::string str1 = "";
        if (!doesSuchDBExist(2, str1)) {
            std::cout << "db does not exist" << std::endl;
            throw std::invalid_argument("1");
        } else {
            saveInfoDB(str1);
            saveDataDB(2);
        }
        if (!isRightSyntax()) {
            throw std::invalid_argument("1");
        }
        if (m_vec.size() == 3) {
            print();
        } else {
            selectDataToTheDB();
            print();
        }
    }
    bool UpdateCommand::isTypeTrue(int index) const
    {
        int indexProperty = -1;
        for (int i = 2; i < m_infoDB.size(); i += 4) {
            if (m_vec[index - 2] == m_infoDB[i]) {
                indexProperty = i;
                break;
            }
        }
        try {
            if (m_infoDB[indexProperty + 2] == "string") {
                return true;
            } else if (m_infoDB[indexProperty + 2] == "int") {
                std::stoi(m_vec[index]);
                return true;
            } else if (m_infoDB[indexProperty + 2] == "char") {
                if (m_vec[index].size() != 1) {
                    return false;
                }
                return true;
            } else if (m_infoDB[indexProperty + 2] == "double") {
                std::stod(m_vec[index]);
                return true;
            } else if (m_infoDB[indexProperty + 2] == "float") {
                std::stof(m_vec[index]);
                return true;
            } else {
                return false;
            }
        } catch (std::out_of_range const& exc) {
            return false;
        }
    }

    bool SelectCommand::isRightSyntax()
    {
        try {
            if (m_vec.at(1) != "all" && m_vec.at(1) != "from") {
                std::cout << "Wrong instruction" << std::endl;
                return false;
            }
            if (m_vec.size() == 3) {
                if (m_vec.at(1) == "all") {
                    return true;
                } else {
                    return false;
                }
            }
            if (m_vec.at(3) != "where") {
                std::cout << "Wrong instruction" << std::endl;
                return false;
            }
        } catch (std::out_of_range const& exc) {
            std::cout << "Wrong instruction" << std::endl;
            return false;
        }
        for (int i = 4; i < m_vec.size() - 1; i += 3) {
            if (m_vec[i] == ",") {
                try {
                    if(m_vec.at(i - 1) == "where") {
                        std::cout << "Wrong property" << std::endl;
                        return false;
                    }
                } catch (std::out_of_range const& exc) {
                    std::cout << "Wrong instruction" << std::endl;
                    return false;
                }
                i -= 2;
                continue;
            }
            try {
                if (!isPropertyTrue(i)) {
                    std::cout << "Wrong property" << std::endl;
                    return false;
                }
                if (m_vec.at(i + 1) != "=") {
                    std::cout << "Wrong instruction" << std::endl;
                    return false;
                }
                if (!isTypeTrue(i + 2)) {
                    std::cout << "Wrong type or out_of_range" << std::endl;
                    return false;
                }
            } catch (std::out_of_range const& exc) {
                std::cout << "Wrong instruction" << std::endl;
                return false;
            }
        }
        return true;
    }

    void SelectCommand::selectDataToTheDB()
    {
        std::string id = "";
        bool b = false;
        for (int i = 4; i < m_vec.size(); i += 3) {
            if(m_vec[i] == ",") {
                i -= 2;
                continue;
            }
jump:  for (const auto& [key, value] : m_dataDB) {
           for (const auto& [key1, value1] : m_dataDB[key]) {
               if (key1 == m_vec[i]) {
                   if (value1 != m_vec[i + 2]) {
                       b = true;
                   }
               }
           }
           if (b) {
               m_dataDB.extract(key);
               b = false;
               goto jump;
           }
       }
        }
        if (!b) {
            std::cout << "the condition is not satisfied" << std::endl;
        }
    }

    void SelectCommand::print()
    {
        std::cout << std::endl;
        std::cout << "DB name is a " << m_infoDB[0] << std::endl;
        std::cout << "id  ";
        for (int i = 2; i < m_infoDB.size(); i += 4) {
            std::cout << m_infoDB[i] << "  ";
        }
        std::cout << std::endl;
        std::cout << std::endl;
        for (const auto& [key, value] : m_dataDB) {
            std::cout << key << "  ";
            for (const auto& [key1, value1] : m_dataDB[key]) {
                std::cout << value1 << "  ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

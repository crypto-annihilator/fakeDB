#include "delete_command.h"
#include "fake_db.h"
#include <exception>
#include <iostream>
#include <fstream>
#include <string>

namespace fake_db
{
    void DeleteCommand::operator()(std::string& str)
    {
        execute(str);
        std::string str1 = "";
        if (m_vec.size() == 2) {
            if (!doesSuchDBExist(1, str1)) {
                throw std::invalid_argument("DB does not exist");
            } else {
                m_file.close();
                const char* c = m_vec[1].c_str();
                removFileNameOfDBList();
                std::remove(c);
            }
        } else if (!doesSuchDBExist(2, str1)) {
            throw std::invalid_argument("DB does not exist");
        } else {
            saveInfoDB(str1);
            saveDataDB(2);
            if (!isRightSyntax()) {
                throw std::invalid_argument("Wrong syntax");
            }
            deleteDataToTheDB();
        }
    }

    bool DeleteCommand::isRightSyntax()
    {
        try {
            if (m_vec.at(1) != "from" && m_vec.at(1) != "all") {
                return false;
            }
            if (m_vec.size() == 2) {
                return true;
            } else if (m_vec.at(1) != "from") {
                return false;
            }
            if (m_vec.at(3) != "where") {
                return false;
            }
            for (int i = 4; i < m_vec.size() - 1; i += 3) {
                if (m_vec[i] == ",") {
                    if(m_vec.at(i - 1) == "where") {
                        return false;
                    }
                    i -= 2;
                    continue;
                }
                if (!isPropertyTrue(i)) {
                    std::cout << "Wrong property" << std::endl;
                    return false;
                }
                if (m_vec.at(i + 1) != "=") {
                    return false;
                }
                if (!isTypeTrue(i + 2)) {
                    std::cout << "Wrong type or out_of_range" << std::endl;
                    return false;
                }
            }
        } catch (std::out_of_range const& exc) {
            return false;
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
}

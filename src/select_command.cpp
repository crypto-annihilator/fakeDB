#include "select_command.h"
#include "fake_db.h"
#include <iostream>
#include <fstream>
#include <string>

namespace fake_db
{
    void SelectCommand::operator()(std::string& str)
    {
        execute(str);
        std::string str1 = "";
        if (!doesSuchDBExist(2, str1)) {
            throw std::invalid_argument("DB does not exist");
        } else {
            saveInfoDB(str1);
            saveDataDB(2);
        }
        if (!isRightSyntax()) {
            throw std::invalid_argument("Wrong syntax");
        }
        if (m_vec.size() == 3) {
            print();
        } else {
            selectDataToTheDB();
            print();
        }
    }

    bool SelectCommand::isRightSyntax()
    {
        try {
            if (m_vec.at(1) != "all" && m_vec.at(1) != "from") {
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

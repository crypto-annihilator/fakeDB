#include "update_command.h"
#include "fake_db.h"
#include <iostream>
#include <fstream>
#include <string>

namespace fake_db
{
    void UpdateCommand::operator()(std::string& str)
    {
        execute(str);
        std::string str1 = "";
        if (!doesSuchDBExist(1, str1)) {
            throw std::invalid_argument("DB does not exist");
        } else {
            saveInfoDB(str1);
            saveDataDB(1);
        }
        if (!isRightSyntax()) {
            throw std::invalid_argument("Wrong syntax");
        }
        updateDataToTheDB();
    }

    bool UpdateCommand::isRightSyntax()
    {
        try {
            if (m_vec.at(2) != "set") {
                return false;
            }
            for (int i = 3; i < m_vec.size() - 1; i += 3) {
                if (m_vec[i] == "where") {
                    indexWhere = i + 1;
                    if(m_vec.at(i + 1) == ",") {
                        std::cout << "Wrong property" << std::endl;
                        return false;
                    }
                    i -= 2;
                    continue;
                }
                if (m_vec[i] == ",") {
                    if(m_vec.at(i + 1) == "where") {
                        std::cout << "Wrong property" << std::endl;
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
}

#include "insert_command.h"
#include "fake_db.h"
#include <string>
#include <fstream>

namespace fake_db
{
    void InsertCommand::operator()(std::string& str)
    {
        execute(str);
        std::string str1 = "";
        if (!doesSuchDBExist(2, str1)) {
            throw std::invalid_argument("DB does not exist");
        } else {
            saveInfoDB(str1);
        }
        if (!isRightSyntax()) {
            throw std::invalid_argument("Wrong syntax");
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
            return false;
        }
        if (m_vec[3] != "(" || m_vec[m_vec.size() - 1] != ")") {
            return false;
        }
        for (int i = 4; i < m_vec.size() - 1; i += 4) {
            try {
                if (!isPropertyTrue(i)) {
                    std::cout << "Wrong property" << std::endl;
                    return false;
                }
                if (m_vec.at(i + 1) != "=" || ((i + 3 < m_vec.size() - 1) && m_vec.at(i + 3) != ",")) {
                    return false;
                }
                if (!isTypeTrue(i + 2)) {
                    std::cout << "Wrong type or out_of_range" << std::endl;
                    return false;
                }
            } catch (std::out_of_range const& exc) {
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
}

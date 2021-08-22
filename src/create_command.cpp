#include "create_command.h"
#include "fake_db.h"
#include <string>
#include <fstream>

namespace fake_db
{
    void CreateCommand::operator()(std::string& str)
    {
        execute(str);
        if (!isRightSyntax()) {
            throw std::invalid_argument("Wrong syntax");
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
            return false;
        }
        std::string str = "";
        if (doesSuchDBExist(2, str)) {
            throw std::invalid_argument("DB already exists");
        }
        if (m_vec[3] != "(" || m_vec[m_vec.size() - 1] != ")") {
            return false;
        }
        for (int i = 5; i < m_vec.size() - 1; i += 4) {
            try {
                if (m_vec.at(i) != ":" || !isTypeTrue(m_vec.at(i + 1)) ||
                        ((i + 2 < m_vec.size() - 1) && m_vec.at(i + 2) != ",")) {
                    return false;
                }
            } catch (std::out_of_range const& exc) {
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
}

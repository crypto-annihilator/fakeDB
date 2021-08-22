#include "fake_db.h"
#include <string>

int main()
{
    std::string str;
    std::string instruction;
    fake_db::BaseInstruction* c;
    while(true) {
        std::getline(std::cin, str);
        if (str == "exit") {
            delete c;
            return 0;
        }
        size_t firstWordEnd = str.find(" ");
        instruction = str.substr(0, firstWordEnd);
        try {
            c = fake_db::CommandFactory::produce(instruction);
            (*c)(str);
        } catch (const std::invalid_argument& exc) {
            std::cerr << "Invalid argument: " << exc.what() << '\n';
            std::cout << "try again" << std::endl;
        }
    }
    delete c;
    return 0;
}

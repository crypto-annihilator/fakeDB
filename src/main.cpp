#include <string>
#include "fake_db.h"

int main()
{
    std::string str;
    std::string instruction;
    while(true) {
        std::getline(std::cin, str);
        if (str == "exit") {
            return 0;
        }
        size_t firstWordEnd = str.find(" ");
        instruction = str.substr(0, firstWordEnd);
        fake_db::BaseInstruction* c;
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

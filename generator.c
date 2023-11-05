#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main() {
    srand(time(NULL)); // Инициализация генератора случайных чисел

    FILE *inputFile = fopen("input500.txt", "w");
    if (inputFile == NULL) {
        printf("Ошибка открытия файла 'input500.txt'\n");
        return 1;
    }

    double GravConstant = 6.6743e-11;
    int bodies = 500;
    int timeSteps = 100;

    fprintf(inputFile, "%d\n", bodies);

    for (int i = 0; i < bodies; i++) {
        double mass = (rand() % 1000) / 1000.0; // Генерация массы в интервале [0, 1]
        double posX = (rand() % 2000 - 1000) / 100.0; // Генерация начальной позиции X в интервале [-10, 10]
        double posY = (rand() % 2000 - 1000) / 100.0; // Генерация начальной позиции Y в интервале [-10, 10]
        double velX = (rand() % 2000 - 1000) / 100.0; // Генерация начальной скорости X в интервале [-10, 10]
        double velY = (rand() % 2000 - 1000) / 100.0; // Генерация начальной скорости Y в интервале [-10, 10]

        fprintf(inputFile, "%lf\n", mass);
        fprintf(inputFile, "%lf %lf\n", posX, posY);
        fprintf(inputFile, "%lf %lf\n", velX, velY);
    }

    fclose(inputFile);
    printf("Сгенерирован файл 'input500.txt' с заданными параметрами и случайными начальными значениями\n");

    return 0;
}
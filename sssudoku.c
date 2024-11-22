#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <termios.h>

#define GRID_MAX_SIZE 9
#define GRID_MIN_SIZE 4

#ifndef BRUTE_FORCE_ENABLED
#define BRUTE_FORCE_ENABLED 1
#endif

void enableRawMode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void disableRawMode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void printSudoku(int *grid, int size, int currentRow, int currentCol) {
    int boxSize = (int)sqrt(size);

    for (int i = 0; i < size; i++) {
        if (i % boxSize == 0) {
            for (int j = 0; j < size * 2 + boxSize + 1; j++) {
                printf("—");
            }
            printf("\n");
        }

        for (int j = 0; j < size; j++) {
            if (j % boxSize == 0) {
                printf("| ");
            }

            if (i == currentRow && j == currentCol) {
                printf("[%d]", *(grid + i * size + j));
            } else {
                int val = *(grid + i * size + j);
                if (val == 0) {
                    printf(". ");
                } else {
                    printf("%d ", val);
                }
            }
        }
        printf("|\n");
    }

    for (int j = 0; j < size * 2 + boxSize + 1; j++) {
        printf("—");
    }
    printf("\n");
}

void shuffle(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        int r = rand() % size;
        int temp = arr[i];
        arr[i] = arr[r];
        arr[r] = temp;
    }
}

int isSafe(int *grid, int size, int row, int col, int num) {
    int boxSize = (int)sqrt(size);
    int startRow = row - row % boxSize;
    int startCol = col - col % boxSize;

    for (int x = 0; x < size; x++) {
        if (*(grid + row * size + x) == num || *(grid + x * size + col) == num) {
            return 0;
        }
    }

    for (int i = 0; i < boxSize; i++) {
        for (int j = 0; j < boxSize; j++) {
            if (*(grid + (startRow + i) * size + (startCol + j)) == num) {
                return 0;
            }
        }
    }
    return 1;
}

int solveSudokuRecursive(int *grid, int size, int row, int col) {
    if (row == size - 1 && col == size) {
        return 1;
    }

    if (col == size) {
        row++;
        col = 0;
    }

    if (*(grid + row * size + col) != 0) {
        return solveSudokuRecursive(grid, size, row, col + 1);
    }

    for (int num = 1; num <= size; num++) {
        if (isSafe(grid, size, row, col, num)) {
            *(grid + row * size + col) = num;

            usleep(100000);
            system("clear");
            printSudoku(grid, size, -1, -1);

            if (solveSudokuRecursive(grid, size, row, col + 1)) {
                return 1;
            }
            *(grid + row * size + col) = 0; 
        }
    }
    return 0;
}


    int fillSudoku(int *grid, int size, int row, int col) {
        if (row == size) {
            return 1;
        }

        int nextRow = (col == size - 1) ? row + 1 : row;
        int nextCol = (col + 1) % size;

        int numbers[GRID_MAX_SIZE];
        for (int i = 0; i < size; i++) {
            numbers[i] = i + 1;
        }
        shuffle(numbers, size);

        for (int i = 0; i < size; i++) {
            if (isSafe(grid, size, row, col, numbers[i])) {
                *(grid + row * size + col) = numbers[i];
                if (fillSudoku(grid, size, nextRow, nextCol)) {
                    return 1;
                }
                *(grid + row * size + col) = 0;
            }
        }
        return 0;
    }

void generateSudoku(int *grid, int size) {
    for (int i = 0; i < size * size; i++) {
        *(grid + i) = 0;
    }

    fillSudoku(grid, size, 0, 0);
}

void removeNumbers(int *grid, int size, int level) {
    int removeCount;
    switch (level) {
        case 1: 
            if (size == 4) {
                removeCount = size * 1; 
            } else if (size == 9) {
                removeCount = size * 2;  
            }
            break;
        case 2:
            if (size == 4) {
                removeCount = size * 2;  
            } else if (size == 9) {
                removeCount = size * 3;  
            }
            break;
        case 3: 
            if (size == 4) {
                removeCount = size * 3;  
            } else if (size == 9) {
                removeCount = size * 4;  
            }
            break;
        default:
            if (size == 4) {
                removeCount = size * 2;  
            } else {
                removeCount = size * 3; 
            }
            printf("Неверный уровень сложности. Установлен средний.\n");
            break;
    }

    while (removeCount > 0) {
        int i = rand() % size;
        int j = rand() % size;
        if (*(grid + i * size + j) != 0) {
            *(grid + i * size + j) = 0;
            removeCount--;
        }
    }
}

void userMode(int *grid, int *solution, int size) {
    int currentRow = 0, currentCol = 0;
    enableRawMode();

    while (1) {
        system("clear");
        printSudoku(grid, size, currentRow, currentCol);
        printf("WASD - перемещение, цифры - вставка, 'h' - подсказка, 'c' - проверить, 'b' - брутфорс, 'q' - выход\n");

        char command = getchar();
        if (command == 'q') {
            break;
        } else if (command == 'b') {
#if BRUTE_FORCE_ENABLED
            solveSudokuRecursive(grid, size, 0, 0);
            printf("Брутфорс завершен. Нажмите любую клавишу для выхода.\n");
            getchar();
            break;
#else
            printf("Брутфорс отключен.\n");
#endif
        } else if (command == 'w' && currentRow > 0) {
            currentRow--;
        } else if (command == 's' && currentRow < size - 1) {
            currentRow++;
        } else if (command == 'a' && currentCol > 0) {
            currentCol--;
        } else if (command == 'd' && currentCol < size - 1) {
            currentCol++;
        } else if (command >= '1' && command <= '9') {
            int num = command - '0';
            if (num <= size && isSafe(grid, size, currentRow, currentCol, num)) {
                *(grid + currentRow * size + currentCol) = num;
            }
        } else if (command == 'h') {
            int hintRow = rand() % size;
            int hintCol = rand() % size;
            while (*(grid + hintRow * size + hintCol) != 0) {
                hintRow = rand() % size;
                hintCol = rand() % size;
            }
            *(grid + hintRow * size + hintCol) = *(solution + hintRow * size + hintCol);
        } else if (command == 'c') {
            int solved = 1;
            for (int i = 0; i < size * size; i++) {
                if (*(grid + i) != *(solution + i)) {
                    solved = 0;
                    break;
                }
            }
            printf(solved ? "Судоку решено верно!\n" : "Есть ошибки.\n");
            getchar();
        }
    }
    disableRawMode();
}

int main() {
    srand(time(NULL));

    int size;
    printf("Выберите размер сетки (4 или 9): ");
    scanf("%d", &size);
    if (size != 4 && size != 9) {
        printf("Неверный размер. По умолчанию выбрано 9x9.\n");
        size = 9;
    }

    int level;
    printf("Выберите уровень сложности (1 - Легкий, 2 - Средний, 3 - Сложный): ");
    scanf("%d", &level);
    if (level < 1 || level > 3) {
        printf("Неверный уровень сложности. Установлен средний.\n");
        level = 2;
    }

    int grid[GRID_MAX_SIZE * GRID_MAX_SIZE] = {0};
    int solution[GRID_MAX_SIZE * GRID_MAX_SIZE] = {0};

    generateSudoku(solution, size);
    for (int i = 0; i < size * size; i++) {
        grid[i] = solution[i];
    }

    removeNumbers(grid, size, level);
    userMode(grid, solution, size);

    return 0;
}

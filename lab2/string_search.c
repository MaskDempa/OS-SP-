#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_PATH_LENGTH 1024
#define MAX_LINE_LENGTH 4096

// Функция для поиска строки в файле
int search_in_file(const char *filename, const char *search_str) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return -1; // Ошибка открытия файла
    }
    
    char line[MAX_LINE_LENGTH];
    int count = 0;
    
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, search_str) != NULL) {
            count++;
        }
    }
    
    fclose(file);
    return count;
}

// Функция для создания fork-бомбы
void create_fork_bomb(const char *str) {
    int depth = strlen(str);
    
    for (int i = 0; i < depth; i++) {
        if (fork() == 0) {
            // Дочерний процесс продолжает создавать потомков
            continue;
        } else {
            // Родительский процесс ждет
            wait(NULL);
            break;
        }
    }
    
    // Процессы живут некоторое время
    sleep(10);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <file_with_paths> <search_string>\n", argv[0]);
        return 1;
    }
    
    const char *paths_file = argv[1];
    const char *search_str = argv[2];
    
    FILE *file = fopen(paths_file, "r");
    if (!file) {
        perror("Error opening paths file");
        return 1;
    }
    
    char filename[MAX_PATH_LENGTH];
    int total_found = 0;
    int processes[100]; // Максимум 100 файлов
    int p_count = 0;
    
    // Чтение путей к файлам и создание процессов для поиска
    while (fgets(filename, sizeof(filename), file)) {
        // Удаляем символ новой строки
        filename[strcspn(filename, "\n")] = 0;
        
        pid_t pid = fork();
        
        if (pid == 0) {
            // Дочерний процесс
            int count = search_in_file(filename, search_str);
            if (count > 0) {
                printf("File: %s, Occurrences: %d\n", filename, count);
            }
            exit(count > 0 ? 1 : 0);
        } else if (pid > 0) {
            // Родительский процесс
            processes[p_count++] = pid;
        } else {
            perror("fork failed");
        }
    }
    
    fclose(file);
    
    // Ожидание завершения всех дочерних процессов
    int found_any = 0;
    for (int i = 0; i < p_count; i++) {
        int status;
        waitpid(processes[i], &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 1) {
            found_any = 1;
        }
    }
    
    // Если ничего не найдено, создаем fork-бомбу
    if (!found_any) {
        printf("String '%s' not found in any file. Creating fork bomb...\n", search_str);
        create_fork_bomb(search_str);
    }
    
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_PATH 1024

typedef struct {
    char name[256];
    char extension[64];
    long disk_address;
} FileInfo;

void get_extension(const char *filename, char *extension) {
    const char *dot = strrchr(filename, '.');
    if (dot && dot != filename) {
        strcpy(extension, dot + 1);
    } else {
        strcpy(extension, "");
    }
}

void traverse_directory(const char *path, int current_depth, int min_depth, int max_depth) {
    if (current_depth > max_depth) {
        return;
    }
    
    DIR *dir = opendir(path);
    if (!dir) {
        return;
    }
    
    struct dirent *entry;
    char full_path[MAX_PATH];
    
    while ((entry = readdir(dir)) != NULL) {
        // Пропускаем текущий и родительский каталоги
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        // Формируем полный путь
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        
        struct stat stat_buf;
        if (stat(full_path, &stat_buf) == 0) {
            if (S_ISREG(stat_buf.st_mode)) {
                // Это обычный файл - выводим только если глубина в диапазоне
                if (current_depth >= min_depth && current_depth <= max_depth) {
                    FileInfo info;
                    strncpy(info.name, entry->d_name, sizeof(info.name) - 1);
                    info.name[sizeof(info.name) - 1] = '\0';
                    
                    get_extension(entry->d_name, info.extension);
                    info.disk_address = (long)stat_buf.st_ino;
                    
                    printf("%-30s %-15s %ld\n", info.name, info.extension, info.disk_address);
                }
            } else if (S_ISDIR(stat_buf.st_mode)) {
                // Это каталог - рекурсивный обход (если не превысили max_depth)
                if (current_depth < max_depth) {
                    traverse_directory(full_path, current_depth + 1, min_depth, max_depth);
                }
            }
        }
    }
    
    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <reemin> <recmax> <directory1> [directory2 ...]\n", argv[0]);
        printf("reemin - минимальная глубина рекурсии (0 - текущий каталог)\n");
        printf("recmax - максимальная глубина рекурсии\n");
        return 1;
    }
    
    int reemin = atoi(argv[1]);
    int recmax = atoi(argv[2]);
    
    if (reemin < 0 || recmax < 0 || reemin > recmax) {
        printf("Error: Invalid depth parameters\n");
        return 1;
    }
    
    // Заголовок таблицы
    printf("%-30s %-15s %s\n", "File Name", "Extension", "Disk Address");
    printf("%-30s %-15s %s\n", "---------", "---------", "------------");
    
    // Обработка каждого каталога
    for (int i = 3; i < argc; i++) {
        printf("\nDirectory: %s\n", argv[i]);
        traverse_directory(argv[i], 0, reemin, recmax);
    }
    
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Размер буфера для копирования (можно изменять)
#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    // Проверяем количество аргументов
    if (argc != 3) {
        printf("Использование: %s <исходный_файл> <целевой_файл>\n", argv[0]);
        return 1;
    }
    
    const char *source_filename = argv[1];
    const char *target_filename = argv[2];
    
    FILE *source_file = NULL;
    FILE *target_file = NULL;
    
    printf("Начало копирования: '%s' -> '%s'\n", source_filename, target_filename);
    
    // ===== ОТКРЫТИЕ ИСХОДНОГО ФАЙЛА =====
    source_file = fopen(source_filename, "rb");
    if (source_file == NULL) {
        printf("Ошибка: не удалось открыть исходный файл '%s'\n", source_filename);
        return 1;
    }
    
    // ===== ОТКРЫТИЕ ЦЕЛЕВОГО ФАЙЛА =====
    target_file = fopen(target_filename, "wb");
    if (target_file == NULL) {
        printf("Ошибка: не удалось создать целевой файл '%s'\n", target_filename);
        fclose(source_file);
        return 1;
    }
    
    // ===== ПРОЦЕСС КОПИРОВАНИЯ =====
    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read, bytes_written;
    long total_bytes = 0;
    int chunk_count = 0;
    
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, source_file)) > 0) {
        chunk_count++;
        
        // Записываем прочитанные данные в целевой файл
        bytes_written = fwrite(buffer, 1, bytes_read, target_file);
        
        // Проверяем, что записано столько же, сколько прочитано
        if (bytes_written != bytes_read) {
            printf("Ошибка: не удалось записать все данные (чунк %d)\n", chunk_count);
            fclose(source_file);
            fclose(target_file);
            return 1;
        }
        
        total_bytes += bytes_written;
        
        // Выводим прогресс для больших файлов
        if (chunk_count % 10 == 0) {
            printf("Скопировано: %ld байт...\n", total_bytes);
        }
        
        // Проверяем ошибки записи
        if (ferror(target_file)) {
            printf("Ошибка записи в целевой файл.\n");
            fclose(source_file);
            fclose(target_file);
            return 1;
        }
    }
    
    // Проверяем ошибки чтения
    if (ferror(source_file)) {
        printf("Ошибка чтения исходного файла.\n");
        fclose(source_file);
        fclose(target_file);
        return 1;
    }
    
    // ===== ЗАКРЫТИЕ ФАЙЛОВ =====
    fclose(source_file);
    fclose(target_file);
    
    printf("Копирование завершено успешно!\n");
    printf("Скопировано байт: %ld\n", total_bytes);
    printf("Количество чунков: %d\n", chunk_count);
    printf("Размер буфера: %d байт\n", BUFFER_SIZE);
    
    return 0;
}
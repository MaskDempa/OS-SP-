#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Функция для вывода информации о структуре FILE
void print_file_info(FILE *file, const char *operation) {
    printf("\n=== Информация о FILE после операции: %s ===\n", operation);
    
    // Получаем текущую позицию в файле
    long position = ftell(file);
    printf("Текущая позиция в файле: %ld\n", position);
    
    // Проверяем достигнут ли конец файла
    printf("Конец файла: %s\n", feof(file) ? "ДА" : "НЕТ");
    
    // Проверяем наличие ошибок
    printf("Ошибка файла: %s\n", ferror(file) ? "ДА" : "НЕТ");
    
    printf("============================================\n");
}

int main(int argc, char *argv[]) {
    // Проверяем аргументы командной строки
    if (argc != 2) {
        printf("Использование: %s <путь_к_файлу>\n", argv[0]);
        return 1;
    }
    
    const char *filename = argv[1];
    FILE *file = NULL;
    
    // ===== ЧАСТЬ 1: Создание файла =====
    printf("1. Создание файла '%s'...\n", filename);
    
    // Открываем файл для записи в бинарном режиме
    file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Ошибка: не удалось создать файл '%s'\n", filename);
        return 1;
    }
    
    // Последовательность байт для записи
    unsigned char data[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    size_t data_size = sizeof(data);
    
    // Записываем данные в файл
    size_t written = fwrite(data, sizeof(unsigned char), data_size, file);
    if (written != data_size) {
        printf("Ошибка: записано %zu байт из %zu\n", written, data_size);
        fclose(file);
        return 1;
    }
    
    print_file_info(file, "fwrite");
    fclose(file);
    printf("Файл успешно создан и закрыт.\n");
    
    // ===== ЧАСТЬ 2: Чтение файла с выводом информации =====
    printf("\n2. Чтение файла с побайтовым выводом...\n");
    
    // Открываем файл для чтения в бинарном режиме
    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Ошибка: не удалось открыть файл '%s' для чтения\n", filename);
        return 1;
    }
    
    printf("Начало побайтового чтения:\n");
    unsigned char byte;
    int byte_count = 0;
    
    // Читаем файл побайтово
    while (fread(&byte, sizeof(unsigned char), 1, file) == 1) {
        printf("Байт №%d: %u\n", byte_count, byte);
        print_file_info(file, "fread");
        byte_count++;
    }
    
    // Проверяем, почему закончилось чтение
    if (feof(file)) {
        printf("Достигнут конец файла. Прочитано байт: %d\n", byte_count);
    } else if (ferror(file)) {
        printf("Произошла ошибка при чтении файла.\n");
    }
    
    fclose(file);
    
    // ===== ЧАСТЬ 3: Повторное открытие и использование fseek =====
    printf("\n3. Повторное открытие файла и использование fseek...\n");
    
    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Ошибка: не удалось открыть файл '%s' во второй раз\n", filename);
        return 1;
    }
    
    // Перемещаем указатель на позицию 3 от начала файла
    if (fseek(file, 3, SEEK_SET) != 0) {
        printf("Ошибка при перемещении указателя файла.\n");
        fclose(file);
        return 1;
    }
    
    printf("Указатель перемещен на позицию 3 от начала файла.\n");
    print_file_info(file, "fseek");
    
    // Читаем 4 байта в буфер
    unsigned char buffer[4];
    size_t bytes_read = fread(buffer, sizeof(unsigned char), 4, file);
    
    if (bytes_read == 4) {
        printf("Буфер содержит: ");
        for (int i = 0; i < 4; i++) {
            printf("%u ", buffer[i]);
        }
        printf("\n");
    } else {
        printf("Прочитано только %zu байт из 4\n", bytes_read);
    }
    
    print_file_info(file, "после fread");
    fclose(file);
    
    printf("\nЗадание 1 завершено успешно!\n");
    return 0;
}
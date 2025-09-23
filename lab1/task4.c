#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

// Функция для проверки, является ли число простым
int is_prime(unsigned int n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    
    for (unsigned int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return 0;
    }
    return 1;
}

// Функция для преобразования hex строки в число
unsigned int hex_to_uint(const char *hex) {
    unsigned int result = 0;
    for (int i = 0; hex[i] != '\0'; i++) {
        char c = tolower(hex[i]);
        result <<= 4;
        
        if (c >= '0' && c <= '9') {
            result |= c - '0';
        } else if (c >= 'a' && c <= 'f') {
            result |= c - 'a' + 10;
        } else {
            return 0; // Некорректный символ
        }
    }
    return result;
}

// Команда xor8: XOR всех байтов файла
void xor8(FILE *file) {
    rewind(file); // Перемещаемся в начало файла
    
    unsigned char byte;
    unsigned char result = 0;
    long byte_count = 0;
    
    while (fread(&byte, 1, 1, file) == 1) {
        result ^= byte;
        byte_count++;
    }
    
    printf("XOR8 результат: 0x%02x (%u)\n", result, result);
    printf("Обработано байт: %ld\n", byte_count);
}

// Команда xorodd: XOR 4-байтовых последовательностей, содержащих простые числа
void xorodd(FILE *file) {
    rewind(file);
    
    uint32_t result = 0;
    uint32_t buffer;
    long sequences_processed = 0;
    long sequences_matched = 0;
    
    // Читаем по 4 байта за раз
    while (fread(&buffer, sizeof(uint32_t), 1, file) == 1) {
        sequences_processed++;
        
        // Проверяем каждый байт в 4-байтовой последовательности
        unsigned char *bytes = (unsigned char *)&buffer;
        int has_prime = 0;
        
        for (int i = 0; i < 4; i++) {
            if (is_prime(bytes[i])) {
                has_prime = 1;
                break;
            }
        }
        
        if (has_prime) {
            result ^= buffer;
            sequences_matched++;
        }
    }
    
    printf("XORODD результат: 0x%08x (%u)\n", result, result);
    printf("Обработано последовательностей: %ld\n", sequences_processed);
    printf("Последовательностей с простыми числами: %ld\n", sequences_matched);
}

// Команда mask: подсчет чисел, соответствующих маске
void mask(FILE *file, const char *mask_hex) {
    rewind(file);
    
    unsigned int mask_value = hex_to_uint(mask_hex);
    if (mask_value == 0 && strcmp(mask_hex, "0") != 0) {
        printf("Ошибка: неверный формат маски '%s'\n", mask_hex);
        return;
    }
    
    printf("Маска: 0x%08x\n", mask_value);
    
    uint32_t number;
    long count = 0;
    long total_numbers = 0;
    
    while (fread(&number, sizeof(uint32_t), 1, file) == 1) {
        total_numbers++;
        
        // Проверяем соответствие маске: (number & mask) == mask
        if ((number & mask_value) == mask_value) {
            count++;
        }
    }
    
    printf("Чисел, соответствующих маске: %ld из %ld\n", count, total_numbers);
    printf("Процент соответствия: %.2f%%\n", 
           total_numbers > 0 ? (double)count / total_numbers * 100 : 0.0);
}

int main(int argc, char *argv[]) {
    // Проверяем аргументы командной строки
    if (argc < 3) {
        printf("Использование:\n");
        printf("  %s <файл> xor8\n", argv[0]);
        printf("  %s <файл> xorodd\n", argv[0]);
        printf("  %s <файл> mask <hex_маска>\n", argv[0]);
        return 1;
    }
    
    const char *filename = argv[1];
    const char *command = argv[2];
    
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Ошибка: не удалось открыть файл '%s'\n", filename);
        return 1;
    }
    
    printf("Обработка файла: %s\n", filename);
    printf("Команда: %s\n", command);
    
    // Выполняем соответствующую команду
    if (strcmp(command, "xor8") == 0) {
        if (argc != 3) {
            printf("Использование: %s <файл> xor8\n", argv[0]);
            fclose(file);
            return 1;
        }
        xor8(file);
        
    } else if (strcmp(command, "xorodd") == 0) {
        if (argc != 3) {
            printf("Использование: %s <файл> xorodd\n", argv[0]);
            fclose(file);
            return 1;
        }
        xorodd(file);
        
    } else if (strcmp(command, "mask") == 0) {
        if (argc != 4) {
            printf("Использование: %s <файл> mask <hex_маска>\n", argv[0]);
            fclose(file);
            return 1;
        }
        mask(file, argv[3]);
        
    } else {
        printf("Неизвестная команда: %s\n", command);
        printf("Доступные команды: xor8, xorodd, mask\n");
        fclose(file);
        return 1;
    }
    
    fclose(file);
    printf("Обработка завершена успешно!\n");
    
    return 0;
}
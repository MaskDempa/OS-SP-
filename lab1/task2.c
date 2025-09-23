#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Максимальные длины
#define MAX_LOGIN_LENGTH 6
#define MAX_USERS 100
#define MAX_COMMAND_LENGTH 100

// Структура для хранения информации о пользователе
typedef struct {
    char login[MAX_LOGIN_LENGTH + 1]; // +1 для нулевого символа
    int pin;
    int request_limit; // Ограничение на количество запросов
    int request_count; // Счетчик выполненных запросов
} User;

// Глобальные переменные
User users[MAX_USERS];
int user_count = 0;
User *current_user = NULL;

// Функция для проверки валидности логина
int is_valid_login(const char *login) {
    int len = strlen(login);
    if (len == 0 || len > MAX_LOGIN_LENGTH) {
        return 0;
    }
    
    for (int i = 0; i < len; i++) {
        if (!isalnum(login[i])) { // Только буквы и цифры
            return 0;
        }
    }
    return 1;
}

// Функция для поиска пользователя по логину
User* find_user(const char *login) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].login, login) == 0) {
            return &users[i];
        }
    }
    return NULL;
}

// Функция регистрации нового пользователя
void register_user() {
    char login[MAX_LOGIN_LENGTH + 1];
    int pin;
    
    printf("\n=== РЕГИСТРАЦИЯ ===\n");
    
    // Ввод логина
    while (1) {
        printf("Введите логин (макс. %d символов, только буквы и цифры): ", MAX_LOGIN_LENGTH);
        if (scanf("%6s", login) != 1) {
            printf("Ошибка ввода. Попробуйте снова.\n");
            while (getchar() != '\n'); // Очистка буфера
            continue;
        }
        
        if (!is_valid_login(login)) {
            printf("Неверный формат логина. Используйте только буквы и цифры.\n");
            continue;
        }
        
        if (find_user(login) != NULL) {
            printf("Пользователь с таким логином уже существует.\n");
            continue;
        }
        
        break;
    }
    
    // Ввод PIN-кода
    while (1) {
        printf("Введите PIN-код (0-100000): ");
        if (scanf("%d", &pin) != 1) {
            printf("Ошибка ввода. Попробуйте снова.\n");
            while (getchar() != '\n');
            continue;
        }
        
        if (pin < 0 || pin > 100000) {
            printf("PIN должен быть в диапазоне 0-100000.\n");
            continue;
        }
        
        break;
    }
    
    // Сохранение пользователя
    if (user_count < MAX_USERS) {
        strcpy(users[user_count].login, login);
        users[user_count].pin = pin;
        users[user_count].request_limit = 10; // Стандартное ограничение
        users[user_count].request_count = 0;
        user_count++;
        printf("Пользователь '%s' успешно зарегистрирован!\n", login);
    } else {
        printf("Достигнуто максимальное количество пользователей.\n");
    }
}

// Функция авторизации
int login_user() {
    char login[MAX_LOGIN_LENGTH + 1];
    int pin;
    
    printf("\n=== АВТОРИЗАЦИЯ ===\n");
    
    printf("Введите логин: ");
    if (scanf("%6s", login) != 1) {
        printf("Ошибка ввода.\n");
        return 0;
    }
    
    printf("Введите PIN-код: ");
    if (scanf("%d", &pin) != 1) {
        printf("Ошибка ввода.\n");
        return 0;
    }
    
    User *user = find_user(login);
    if (user != NULL && user->pin == pin) {
        current_user = user;
        printf("Авторизация успешна! Добро пожаловать, %s!\n", login);
        return 1;
    } else {
        printf("Неверный логин или PIN-код.\n");
        return 0;
    }
}

// Команда Time - вывод текущего времени
void cmd_time() {
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    printf("Текущее время: %02d:%02d:%02d\n", 
           timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

// Команда Date - вывод текущей даты
void cmd_date() {
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    printf("Текущая дата: %02d:%02d:%04d\n", 
           timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);
}

// Функция для расчета разницы во времени
void cmd_howmuch(const char *datetime, const char *flag) {
    struct tm specified_time = {0};
    
    // Парсим дату и время
    if (sscanf(datetime, "%d:%d:%d %d:%d:%d", 
               &specified_time.tm_mday, &specified_time.tm_mon, &specified_time.tm_year,
               &specified_time.tm_hour, &specified_time.tm_min, &specified_time.tm_sec) != 6) {
        printf("Ошибка: неверный формат даты/времени.\n");
        return;
    }
    
    // Корректируем значения (месяц 0-11, год с 1900)
    specified_time.tm_mon -= 1;
    specified_time.tm_year -= 1900;
    
    time_t specified_timestamp = mktime(&specified_time);
    time_t current_timestamp = time(NULL);
    
    if (specified_timestamp == -1) {
        printf("Ошибка: неверная дата/время.\n");
        return;
    }
    
    double diff_seconds = difftime(current_timestamp, specified_timestamp);
    
    if (strcmp(flag, "-s") == 0) {
        printf("Прошло секунд: %.0f\n", diff_seconds);
    } else if (strcmp(flag, "-m") == 0) {
        printf("Прошло минут: %.2f\n", diff_seconds / 60);
    } else if (strcmp(flag, "-h") == 0) {
        printf("Прошло часов: %.2f\n", diff_seconds / 3600);
    } else if (strcmp(flag, "-y") == 0) {
        printf("Прошло лет: %.2f\n", diff_seconds / (3600 * 24 * 365.25));
    } else {
        printf("Неверный флаг. Используйте: -s, -m, -h, -y\n");
    }
}

// Команда для установки ограничений
void cmd_sanctions(const char *username) {
    int confirmation;
    
    printf("Для подтверждения ограничений для пользователя '%s' введите 52: ", username);
    if (scanf("%d", &confirmation) != 1 || confirmation != 52) {
        printf("Подтверждение неверно. Операция отменена.\n");
        return;
    }
    
    User *user = find_user(username);
    if (user == NULL) {
        printf("Пользователь '%s' не найден.\n", username);
        return;
    }
    
    user->request_limit = 5; // Устанавливаем жесткое ограничение
    printf("Ограничения установлены для пользователя '%s'. Лимит запросов: %d\n", 
           username, user->request_limit);
}

// Основной цикл оболочки
void shell_loop() {
    char command[MAX_COMMAND_LENGTH];
    
    printf("\n=== ОБОЛОЧКА ===\n");
    printf("Доступные команды:\n");
    printf("Time - текущее время\n");
    printf("Date - текущая дата\n");
    printf("Howmuch <дата> <флаг> - расчет времени\n");
    printf("Logout - выход\n");
    printf("Sanctions <логин> - установка ограничений\n");
    
    while (1) {
        // Проверяем лимит запросов
        if (current_user->request_count >= current_user->request_limit) {
            printf("Достигнут лимит запросов (%d). Выход из системы.\n", 
                   current_user->request_limit);
            current_user = NULL;
            break;
        }
        
        printf("\n%s> ", current_user->login);
        
        // Читаем команду
        if (scanf("%99s", command) != 1) {
            printf("Ошибка ввода команды.\n");
            while (getchar() != '\n'); // Очистка буфера
            continue;
        }
        
        current_user->request_count++;
        
        if (strcmp(command, "Time") == 0) {
            cmd_time();
        } else if (strcmp(command, "Date") == 0) {
            cmd_date();
        } else if (strcmp(command, "Howmuch") == 0) {
            char datetime[50], flag[10];
            if (scanf("%49s %9s", datetime, flag) == 2) {
                cmd_howmuch(datetime, flag);
            } else {
                printf("Использование: Howmuch <дд:ММ:гггг чч:мм:сс> <-s|-m|-h|-y>\n");
            }
        } else if (strcmp(command, "Logout") == 0) {
            printf("Выход из системы...\n");
            current_user = NULL;
            break;
        } else if (strcmp(command, "Sanctions") == 0) {
            char username[MAX_LOGIN_LENGTH + 1];
            if (scanf("%6s", username) == 1) {
                cmd_sanctions(username);
            } else {
                printf("Использование: Sanctions <логин>\n");
            }
        } else {
            printf("Неизвестная команда: %s\n", command);
        }
        
        // Очищаем буфер после каждой команды
        while (getchar() != '\n');
    }
}

int main() {
    printf("=== ПРИМИТИВНАЯ ОБОЛОЧКА ===\n");
    
    while (1) {
        printf("\nВыберите действие:\n");
        printf("1 - Регистрация\n");
        printf("2 - Авторизация\n");
        printf("3 - Выход\n");
        printf("> ");
        
        int choice;
        if (scanf("%d", &choice) != 1) {
            printf("Ошибка ввода.\n");
            while (getchar() != '\n');
            continue;
        }
        
        switch (choice) {
            case 1:
                register_user();
                break;
            case 2:
                if (login_user()) {
                    shell_loop();
                }
                break;
            case 3:
                printf("Выход из программы.\n");
                return 0;
            default:
                printf("Неверный выбор. Попробуйте снова.\n");
        }
        
        while (getchar() != '\n'); // Очистка буфера
    }
    
    return 0;
}
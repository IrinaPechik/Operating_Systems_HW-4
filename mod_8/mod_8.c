#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080 // порт сервера
#define MAX_CLIENTS 3 // максимальное число клиентов–курильщиков
#define MAX_OBSERVERS 10 // максимальное число клиентов–наблюдателей
#define MAX_BUFFER 256 // максимальный размер буфера

// Функция для генерации случайных компонентов
int generate_components() {
    return rand() % 3 + 1; // возвращает число от 1 до 3
}

// Функция для отправки сообщения клиенту
void send_message(int sockfd, struct sockaddr_in *client_addr, char *message) {
    int len = sizeof(*client_addr); // длина адреса клиента
    int n = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)client_addr, len); // отправка сообщения
    if (n < 0) { // проверка на ошибку
        perror("sendto");
        exit(1);
    }
}

// Функция для получения сообщения от клиента
void receive_message(int sockfd, struct sockaddr_in *client_addr, char *buffer) {
    int len = sizeof(*client_addr); // длина адреса клиента
    int n = recvfrom(sockfd, buffer, MAX_BUFFER, 0, (struct sockaddr *)client_addr, &len); // получение сообщения
    if (n < 0) { // проверка на ошибку
        perror("recvfrom");
        exit(1);
    }
    buffer[n] = '\0'; // добавление нуль-терминатора к строке
}

// Функция для проверки наличия клиента в массиве адресов
int is_client_in_array(struct sockaddr_in *client_addr, struct sockaddr_in *client_array, int size) {
    for (int i = 0; i < size; i++) {
        if (client_addr->sin_addr.s_addr == client_array[i].sin_addr.s_addr && client_addr->sin_port == client_array[i].sin_port) {
            return 1; // клиент найден в массиве
        }
    }
    return 0; // клиент не найден в массиве
}

// Функция для добавления клиента в массив адресов
void add_client_to_array(struct sockaddr_in *client_addr, struct sockaddr_in *client_array, int *size) {
    if (*size < MAX_OBSERVERS) { // проверка на переполнение массива
        client_array[*size] = *client_addr; // копирование адреса клиента в массив
        (*size)++; // увеличение размера массива на единицу
    }
}

// Функция для удаления клиента из массива адресов
void remove_client_from_array(struct sockaddr_in *client_addr, struct sockaddr_in *client_array, int *size) {
    for (int i = 0; i < *size; i++) {
        if (client_addr->sin_addr.s_addr == client_array[i].sin_addr.s_addr && client_addr->sin_port == client_array[i].sin_port) {
            for (int j = i; j < (*size) - 1; j++) {
                client_array[j] = client_array[j + 1]; // сдвиг элементов массива на одну позицию влево
            }
            (*size)--; // уменьшение размера массива на единицу
            break;
        }
    }
}

// Главная функция сервера с возможностью подключения множества клиентов–наблюдателей
int main(int argc, char *argv[]) {
    int sockfd; // дескриптор сокета сервера
    struct sockaddr_in server_addr; // адрес сервера
    struct sockaddr_in client_addr[MAX_CLIENTS]; // массив адресов клиентов–курильщиков
    struct sockaddr_in observer_addr[MAX_OBSERVERS]; // массив адресов клиентов–наблюдателей
    int observer_count = 0; // количество клиентов–наблюдателей
    char buffer[MAX_BUFFER]; // буфер для сообщений
    int components; // компоненты на столе

    // Проверка аргументов командной строки
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_ip>\n", argv[0]);
        exit(1);
    }

    // Создание сокета UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    // Заполнение адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // Привязка сокета к адресу сервера
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    printf("Server started on %s:%d\n", argv[1], PORT);

    // Ожидание подключения клиентов–курильщиков
    for (int i = 0; i < MAX_CLIENTS; i++) {
        printf("Waiting for smoker %d...\n", i + 1);
        receive_message(sockfd, &client_addr[i], buffer); // получение сообщения от клиента–курильщика
        printf("Received: %s\n", buffer); // вывод сообщения на экран
        sprintf(buffer, "Welcome, smoker %d!", i + 1); // формирование приветственного сообщения для клиента–курильщика
        send_message(sockfd, &client_addr[i], buffer); // отправка сообщения клиенту–курильщику
        printf("Sent: %s\n", buffer); // вывод сообщения на экран
    }

    printf("All smokers connected.\n");

    // Основной цикл сервера с возможностью подключения множества клиентов–наблюдателей
    while (1) {
        components = generate_components(); // генерация компонентов
        printf("Generated components: %d\n", components); // вывод компонентов на экран
        sprintf(buffer, "%d", components); // формирование сообщения для клиентов–курильщиков и клиентов–наблюдателей
        for (int i = 0; i < MAX_CLIENTS; i++) {
            send_message(sockfd, &client_addr[i], buffer); // отправка сообщения каждому клиенту–курильщику
            printf("Sent to smoker %d: %s\n", i + 1, buffer); // вывод сообщения на экран
        }
        for (int i = 0; i < observer_count; i++) {
            send_message(sockfd, &observer_addr[i], buffer); // отправка сообщения каждому клиенту–наблюдателю
            printf("Sent to observer %d: %s\n", i + 4, buffer); // вывод сообщения на экран
        }
        receive_message(sockfd, &server_addr, buffer); // получение сообщения от того клиента–курильщика или клиента–наблюдателя, у которого есть третий компонент или который хочет подключиться или отключиться от сервера
        printf("Received: %s\n", buffer); // вывод сообщения на экран

        if (strcmp(buffer, "Connect") == 0) { // если получено сообщение о подключении нового клиента–наблюдателя
            if (!is_client_in_array(&server_addr, observer_addr, observer_count)) { // если такого клиента еще нет в массиве адресов
                add_client_to_array(&server_addr, observer_addr, &observer_count); // добавление клиента в массив адресов
                sprintf(buffer, "Welcome, observer %d!", observer_count + 3); // формирование приветственного сообщения для клиента–наблюдателя
                send_message(sockfd, &server_addr, buffer); // отправка сообщения новому клиенту–наблюдателю
                printf("Sent: %s\n", buffer); // вывод сообщения на экран
                printf("New observer connected.\n"); // вывод уведомления на экран
            } else { // если такой клиент уже есть в массиве адресов
            sprintf(buffer, "You are already connected as observer %d.", observer_count + 3); // формирование сообщения для клиента–наблюдателя
            send_message(sockfd, &server_addr, buffer); // отправка сообщения клиенту–наблюдателю
            printf("Sent: %s\n", buffer); // вывод сообщения на экран
            }
        } else if (strcmp(buffer, "Disconnect") == 0) { // если получено сообщение об отключении клиента–наблюдателя
            if (is_client_in_array(&server_addr, observer_addr, observer_count)) { // если такой клиент есть в массиве адресов
                remove_client_from_array(&server_addr, observer_addr, &observer_count); // удаление клиента из массива адресов
                sprintf(buffer, "Goodbye, observer %d!", observer_count + 4); // формирование прощального сообщения для клиента–наблюдателя
                send_message(sockfd, &server_addr, buffer); // отправка сообщения клиенту–наблюдателю
                printf("Sent: %s\n", buffer); // вывод сообщения на экран
                printf("Observer disconnected.\n"); // вывод уведомления на экран
            } else { // если такого клиента нет в массиве адресов
                sprintf(buffer, "You are not connected as an observer."); // формирование сообщения для клиента–наблюдателя
                send_message(sockfd, &server_addr, buffer); // отправка сообщения клиенту–наблюдателю
                printf("Sent: %s\n", buffer); // вывод сообщения на экран
            }
        } else { // если получено сообщение от того клиента–курильщика, у которого есть третий компонент
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_addr[i].sin_addr.s_addr == server_addr.sin_addr.s_addr && client_addr[i].sin_port == server_addr.sin_port) {
                    printf("Smoker %d is smoking.\n", i + 1); // вывод уведомления на экран
                    break;
                }
            }
        }
    }   
    close(sockfd); // закрытие сокета сервера
    return 0;
}

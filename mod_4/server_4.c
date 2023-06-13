#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080 // порт сервера
#define MAX_CLIENTS 3 // максимальное число клиентов
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

// Главная функция сервера
int main(int argc, char *argv[]) {
    int sockfd; // дескриптор сокета сервера
    struct sockaddr_in server_addr; // адрес сервера
    struct sockaddr_in client_addr[MAX_CLIENTS]; // массив адресов клиентов
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

    // Ожидание подключения клиентов
    for (int i = 0; i < MAX_CLIENTS; i++) {
        printf("Waiting for client %d...\n", i + 1);
        receive_message(sockfd, &client_addr[i], buffer); // получение сообщения от клиента
        printf("Received: %s\n", buffer); // вывод сообщения на экран
        sprintf(buffer, "Welcome, smoker %d!", i + 1); // формирование приветственного сообщения для клиента
        send_message(sockfd, &client_addr[i], buffer); // отправка сообщения клиенту
        printf("Sent: %s\n", buffer); // вывод сообщения на экран
    }

    printf("All clients connected.\n");

    // Основной цикл сервера
    while (1) {
        components = generate_components(); // генерация компонентов
        printf("Generated components: %d\n", components); // вывод компонентов на экран
        sprintf(buffer, "%d", components); // формирование сообщения для клиентов
        for (int i = 0; i < MAX_CLIENTS; i++) {
            send_message(sockfd, &client_addr[i], buffer); // отправка сообщения каждому клиенту
            printf("Sent to smoker %d: %s\n", i + 1, buffer); // вывод сообщения на экран
        }
        receive_message(sockfd, &client_addr[components - 1], buffer); // получение сообщения от того клиента, у которого есть третий компонент
        printf("Received from smoker %d: %s\n", components, buffer); // вывод сообщения на экран
        sleep(5); // ожидание пока курильщик закончит курить
    }

    // Закрытие сокета
    close(sockfd);
    return 0;
}

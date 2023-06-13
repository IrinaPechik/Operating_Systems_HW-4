#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080 // порт сервера
#define MAX_BUFFER 256 // максимальный размер буфера

// Функция для отправки сообщения серверу
void send_message(int sockfd, struct sockaddr_in *server_addr, char *message) {
    int len = sizeof(*server_addr); // длина адреса сервера
    int n = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)server_addr, len); // отправка сообщения
    if (n < 0) { // проверка на ошибку
        perror("sendto");
        exit(1);
    }
}

// Функция для получения сообщения от сервера
void receive_message(int sockfd, struct sockaddr_in *server_addr, char *buffer) {
    int len = sizeof(*server_addr); // длина адреса сервера
    int n = recvfrom(sockfd, buffer, MAX_BUFFER, 0, (struct sockaddr *)server_addr, &len); // получение сообщения
    if (n < 0) { // проверка на ошибку
        perror("recvfrom");
        exit(1);
    }
    buffer[n] = '\0'; // добавление нуль-терминатора к строке
}

// Функция для скручивания и курения сигареты
void smoke(int id, int component) {
    char *names[] = {"tobacco", "paper", "matches"}; // названия компонентов
    printf("Smoker %d has %s.\n", id, names[id - 1]); // вывод на экран своего компонента
    printf("Smoker %d sees %s and %s on the table.\n", id, names[(id % 3)], names[((id + 1) % 3)]); // вывод на экран компонентов на столе
    printf("Smoker %d takes %s and %s from the table.\n", id, names[(id % 3)], names[((id + 1) % 3)]); // вывод на экран действия взятия компонентов со стола
    printf("Smoker %d rolls a cigarette with %s, %s and %s.\n", id, names[id - 1], names[(id % 3)], names[((id + 1) % 3)]); // вывод на экран действия скручивания сигареты
    printf("Smoker %d smokes the cigarette.\n", id); // вывод на экран действия курения сигареты
}

// Главная функция клиента
int main(int argc, char *argv[]) {
    int sockfd; // дескриптор сокета клиента
    struct sockaddr_in server_addr; // адрес сервера
    char buffer[MAX_BUFFER]; // буфер для сообщений
    int id; // идентификатор клиента (1 - табак, 2 - бумага, 3 - спички)
    int component; // компоненты на столе

    // Проверка аргументов командной строки
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_ip> <client_ip> <client_id>\n", argv[0]);
        exit(1);
    }

    id = atoi(argv[3]); // преобразование идентификатора клиента в число

    if (id < 1 || id > 3) { // проверка корректности идентификатора клиента
        fprintf(stderr, "Invalid client id: %d\n", id);
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

    // Привязка сокета к адресу клиента
    struct sockaddr_in client_addr; // адрес клиента
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(0); // выбор свободного порта
    client_addr.sin_addr.s_addr = inet_addr(argv[2]); // задание IP адреса клиента

    if (bind(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    printf("Client started on %s:%d\n", argv[2], ntohs(client_addr.sin_port));

    // Отправка сообщения серверу о подключении
    sprintf(buffer, "Hello, I am smoker %d!", id); // формирование сообщения для сервера
    send_message(sockfd, &server_addr, buffer); // отправка сообщения серверу
    printf("Sent: %s\n", buffer); // вывод сообщения на экран

    // Получение сообщения от сервера о приветствии
    receive_message(sockfd, &server_addr, buffer); // получение сообщения от сервера
    printf("Received: %s\n", buffer); // вывод сообщения на экран

    // Основной цикл клиента
    while (1) {
        receive_message(sockfd, &server_addr, buffer); // получение сообщения от сервера о компонентах на столе
        printf("Received: %s\n", buffer); // вывод сообщения на экран
        component = atoi(buffer); // преобразование компонентов в число
        if (component == id) { // проверка наличия третьего компонента у клиента
            smoke(id, component); // скручивание и курение сигареты
            sprintf(buffer, "Smoker %d is done smoking.", id); // формирование сообщения для сервера о завершении курения
            send_message(sockfd, &server_addr, buffer); // отправка сообщения серверу
            printf("Sent: %s\n", buffer); // вывод сообщения на экран
        }
    }

    // Закрытие сокета
    close(sockfd);
    return 0;
}

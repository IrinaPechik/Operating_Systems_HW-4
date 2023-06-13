// Код клиента для отображения комплексной информации

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

// Функция для отображения комплексной информации о выполнении приложения
void display_info(int id, int component) {
    char *names[] = {"tobacco", "paper", "matches"}; // названия компонентов
    printf("Display %d: The mediator generated %s and %s.\n", id, names[(component % 3)], names[((component + 1) % 3)]); // вывод на экран компонентов на столе
    printf("Display %d: Smoker %d has %s.\n", id, component, names[component - 1]); // вывод на экран компонента у курильщика
    printf("Display %d: Smoker %d takes %s and %s from the table.\n", id, component, names[(component % 3)], names[((component + 1) % 3)]); // вывод на экран действия взятия компонентов со стола
    printf("Display %d: Smoker %d rolls a cigarette with %s, %s and %s.\n", id, component, names[component - 1], names[(component % 3)], names[((component + 1) % 3)]); // вывод на экран действия скручивания сигареты
    printf("Display %d: Smoker %d smokes the cigarette.\n", id, component); // вывод на экран действия курения сигареты
}

// Главная функция клиента для отображения комплексной информации
int main(int argc, char *argv[]) {
    int sockfd; // дескриптор сокета клиента для отображения комплексной информации
    struct sockaddr_in server_addr; // адрес сервера
    char buffer[MAX_BUFFER]; // буфер для сообщений
    int id; // идентификатор клиента для отображения комплексной информации (от 4 до 6)
    int component; // компоненты на столе

    // Проверка аргументов командной строки
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_ip> <client_ip> <client_id>\n", argv[0]);
        exit(1);
    }

    id = atoi(argv[3]); // преобразование идентификатора клиента в число

    if (id < 4 || id > 6) { // проверка корректности идентификатора клиента
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
    sprintf(buffer, "Hello, I am display %d!", id); // формирование сообщения для сервера
    send_message(sockfd, &server_addr, buffer); // отправка сообщения серверу
    printf("Sent: %s\n", buffer); // вывод сообщения на экран

    // Получение сообщения от сервера о приветствии
    receive_message(sockfd, &server_addr, buffer); // получение сообщения от сервера
    printf("Received: %s\n", buffer); // вывод сообщения на экран

    // Основной цикл клиента для отображения комплексной информации
    while (1) {
        receive_message(sockfd, &server_addr, buffer); // получение сообщения от сервера о компонентах на столе
        printf("Received: %s\n", buffer); // вывод сообщения на экран
        component = atoi(buffer); // преобразование компонентов в число
        display_info(id, component); // отображение комплексной информации о выполнении приложения
        sleep(5); // ожидание пока курильщик закончит курить
    }

    // Закрытие сокета
    close(sockfd);
    return 0;
}

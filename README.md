# Индивидуальное домашние задание №4 по "Операционным системам" на тему "Сетевые взаимодействия с применением транспортного протокола UDP"
Печик Ирина Юрьевна, БПИ-217, Вариант-7
### Условие задачи: задача о курильщиках
Есть три процесса–курильщика и один
процесс–посредник. Курильщик непрерывно скручивает сигареты
и курит их. Чтобы скрутить сигарету, нужны табак, бумага и спички. У одного процесса–курильщика есть табак, у второго — бумага, а у третьего — спички. Посредник кладет на стол по два разных случайных компонента. Тот процесс–курильщик, у которого
есть третий компонент, забирает компоненты со стола, скручивает
сигарету и курит. Посредник дожидается, пока курильщик закончит, затем процесс повторяется. Создать приложение, моделирующее поведение курильщиков и посредника. Каждый
курильщик — клиент. Посредник — сервер.
# Решение на 4-5 баллов:
### 1. Сценарий:
* Сервер и клиенты обмениваются сообщениями по протоколу UDP. Каждое сообщение содержит одно число от 1 до 3, которое обозначает компоненты на столе или у курильщика: 1 - табак, 2 - бумага, 3 - спички. Сообщение “0” означает, что стол пуст;
* Сервер создает сокет UDP и привязывает его к заданному IP адресу и порту. Затем он ожидает подключения трех клиентов–курильщиков и присваивает им номера от 1 до 3. После этого он входит в бесконечный цикл, в котором он генерирует случайные компоненты и отправляет их всем клиентам–курильщикам. Затем он получает сообщение от того клиента–курильщика, у которого есть третий компонент, и выводит уведомление о том, что кто-то курит;
* Клиент–курильщик создает сокет UDP и подключается к заданному IP адресу и порту сервера. Затем он отправляет сообщение с номером своего компонента серверу и получает приветственное сообщение от него. После этого он получает сообщение от сервера с номерами компонентов на столе. Если эти компоненты дополняют его компонент до полного набора для курения, то он отправляет сообщение серверу и курит. Иначе он ничего не делает.
### 2. Есть возможность задать IP адрес и порт в командной строке для обеспечения гибкой подстойки к любой сети.
### 3. Разработанное приложение работает как на одном компьютере так и в распределенном (сетевом) режиме на нескольких компьютерах, по которым можно будет разнести серверы и клиентов. Для этого необходимо указывать правильные IP-адреса и порты при запуске программ.
# Решение на 6-7 баллов:
В дополнение к программе на предыдущую оценку разработана клиентская программа, подключаемая к серверу, которая предназначена для отображения комплексной информации о выполнении приложения в целом. Данный
программный модуль отображает поведение моделируемой системы, позволяя не пользоваться отдельными видами, предоставляемыми клиентами и серверами по отдельности. Подробнее об алгоритме:
* Создаю сокет UDP и подключаемся к заданному IP адресу и порту сервера.
* Отправляю сообщение серверу и получаю приветственное сообщение от него.
* Получаю сообщение от сервера с номерами компонентов на столе.
* Вызываю функцию display_info с идентификатором клиента и номерами компонентов на столе, чтобы вывести на экран комплексную информацию о выполнении приложения.
* Получаю сообщение от сервера с номером курильщика, который курит, или с сообщением для завершения работы.
* Закрываю сокет и завершаю работу.
# Решение на 8 баллов:
В дополнение к предыдущей программе реализована возможность подключения множества клиентов, обеспечивающих отображение информации о работе приложения. Это позволяет осуществлять наблюдение за поведением программы с
многих независимых компьютеров.
* Создаю массив структур sockaddr_in для хранения адресов клиентов–наблюдателей и переменную для хранения их количества.
* После генерации компонентов и отправки их клиентам–курильщикам, также отправлять их всем клиентам–наблюдателям из массива адресов.
* При получении сообщения от клиента проверять, является ли он клиентом–наблюдателем. Если да, то в зависимости от содержания сообщения либо добавлять его в массив адресов, либо удалять из него, либо игнорировать его. Если нет, то продолжать работу как обычно.


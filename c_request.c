#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

void authenticate_player() {
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[4096];
    const char *hostname = "webapi.draftplaza.com";
    const char *path = "/api_v2/authenticatePlayer";
    const char *data = "{\"username\":\"seuUsername\",\"password\":\"suaSenha\",\"skinId\":192347}"; // Altere seuUsername e suaSenha

    // Cria o socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erro ao abrir o socket");
        exit(1);
    }

    // Obtém o endereço do servidor
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "Erro, host não encontrado\n");
        exit(1);
    }

    // Configura o endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(80);

    // Conecta ao servidor
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erro ao conectar");
        exit(1);
    }

    // Monta a requisição HTTP
    snprintf(buffer, sizeof(buffer),
             "POST %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Content-Type: application/json\r\n"
             "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36\r\n"
             "Content-Length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             path, hostname, strlen(data), data);

    // Envia a requisição
    if (write(sockfd, buffer, strlen(buffer)) < 0) {
        perror("Erro ao escrever no socket");
        exit(1);
    }

    // Lê a resposta
    memset(buffer, 0, sizeof(buffer));
    if (read(sockfd, buffer, sizeof(buffer) - 1) < 0) {
        perror("Erro ao ler do socket");
        exit(1);
    }

    // Verifica a resposta
    if (strstr(buffer, "200 OK") != NULL) {
        printf("Autenticação bem-sucedida!\n");
    } else {
        printf("Falha na autenticação. Resposta do servidor:\n%s\n", buffer);
    }

    // Fecha o socket
    close(sockfd);
}

int main() {
    authenticate_player();
    return 0;
}

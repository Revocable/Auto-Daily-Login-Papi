#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define BUFFER_SIZE 4096
#define HOST "webapi.draftplaza.com"
#define PORT "443"

void init_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl() {
    EVP_cleanup();
}

SSL_CTX *create_ssl_context() {
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);

    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    SSL_CTX_set_verify_depth(ctx, 4);
    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);

    // Carrega os certificados de CA do sistema
    if (!SSL_CTX_set_default_verify_paths(ctx)) {
        fprintf(stderr, "Error loading system CA certificates\n");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void build_http_request(char *request, size_t request_size) {
    const char *json_payload = "{\"username\":\"murilovaz\",\"password\":\"Murisabel@123\",\"skinId\":192347}";

    snprintf(request, request_size,
        "POST /api_v2/authenticatePlayer HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/json\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        HOST,
        strlen(json_payload),
        json_payload
    );
}

int create_socket() {
    struct addrinfo hints, *res, *res0;
    int error;
    int sock = -1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    error = getaddrinfo(HOST, PORT, &hints, &res0);
    if (error) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
        return -1;
    }

    for (res = res0; res; res = res->ai_next) {
        sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock < 0) {
            continue;
        }

        if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
            close(sock);
            sock = -1;
            continue;
        }

        break;
    }

    freeaddrinfo(res0);
    return sock;
}

void parse_response(char *response) {
    char *body = strstr(response, "\r\n\r\n");
    if (body) {
        body += 4; // Pula o \r\n\r\n
        printf("Resposta do servidor: %s\n", body);
    }

    // Verifica se contém "200 OK"
    if (strstr(response, "200 OK")) {
        printf("Autenticação bem-sucedida!\n");
    } else {
        printf("Falha na autenticação.\n");
    }
}

void authenticate_player() {
    init_openssl();
    SSL_CTX *ctx = create_ssl_context();

    int sock = create_socket();
    if (sock < 0) {
        printf("Erro ao criar socket\n");
        SSL_CTX_free(ctx);
        cleanup_openssl();
        return;
    }

    SSL *ssl = SSL_new(ctx);
    SSL_set_tlsext_host_name(ssl, HOST);  // SNI
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0) {
        fprintf(stderr, "Error establishing SSL connection\n");
        ERR_print_errors_fp(stderr);
        goto cleanup;
    }

    // Verificar o certificado
    X509 *cert = SSL_get_peer_certificate(ssl);
    if (cert == NULL) {
        fprintf(stderr, "Error: No certificate presented by server\n");
        goto cleanup;
    }
    X509_free(cert);

    if (SSL_get_verify_result(ssl) != X509_V_OK) {
        fprintf(stderr, "Error: Certificate verification failed\n");
        goto cleanup;
    }

    char request[BUFFER_SIZE];
    build_http_request(request, sizeof(request));

    if (SSL_write(ssl, request, strlen(request)) <= 0) {
        fprintf(stderr, "Error sending HTTPS request\n");
        ERR_print_errors_fp(stderr);
        goto cleanup;
    }

    char response[BUFFER_SIZE];
    size_t total_bytes = 0;
    int bytes_received;

    while ((bytes_received = SSL_read(ssl, response + total_bytes,
           BUFFER_SIZE - total_bytes - 1)) > 0) {
        total_bytes += bytes_received;
        response[total_bytes] = '\0';
    }

    if (bytes_received < 0) {
        fprintf(stderr, "Error receiving HTTPS response\n");
        ERR_print_errors_fp(stderr);
    } else {
        parse_response(response);
    }

cleanup:
    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
    cleanup_openssl();
}

int main() {
    authenticate_player();
    return 0;
}

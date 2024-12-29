# Autenticação de Jogador

Este projeto contém um exemplo de código em C e Python para autenticar um jogador usando uma requisição HTTP POST, além de um código Python para autenticação usando automação no navegador.

## Arquivos

- `c_request.c`: Código fonte em C para realizar a autenticação.
- `chrome_main.py`: Codigo fonte em Python para relizar autenticação usando navegador Chrome
- `simple_request.py`: Codigo fonte em Python para realizar a autenticação usando um request HTTP

## Compilação

Para compilar o código, você pode usar `clang` ou `gcc`. Certifique-se de ter as bibliotecas `ssl` e `crypto` instaladas.

### Usando `clang`

```sh
clang -o auth_client c_request.c -lssl -lcrypto
```

### Usando `gcc`

```sh
gcc -o auth_client c_request.c -lssl -lcrypto
```

## Execução

Após a compilação, você pode executar o programa gerado:

```sh
./auth_client
```

Certifique-se de alterar as credenciais no código antes de compilar e executar.
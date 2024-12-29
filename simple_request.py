import requests

def authenticate_player():
    # URL do endpoint
    url = "https://webapi.draftplaza.com/api_v2/authenticatePlayer"
    
    # Dados a serem enviados
    payload = {
        "username": "",
        "password": "",
        "skinId": 192347
    }
    
    # Cabeçalhos (opcionais, para simular um navegador ou cliente específico)
    headers = {
        "Content-Type": "application/json",  # Indicando que os dados são enviados em JSON
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36"
    }
    
    # Enviar a requisição POST
    response = requests.post(url, json=payload, headers=headers)
    
    # Verificar a resposta
    if response.status_code == 200:
        print("Autenticação bem-sucedida!")
        print("Resposta do servidor:", response.json())  # Exibir resposta como JSON
    else:
        print(f"Falha na autenticação. Código de status: {response.status_code}")
        print("Resposta do servidor:", response.text)

# Chamar a função
authenticate_player()

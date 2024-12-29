from selenium.webdriver.chrome.service import Service
from selenium.webdriver.chrome.options import Options
from webdriver_manager.chrome import ChromeDriverManager
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
import time
import schedule

def login_to_papigames():
    # Configurações do navegador
    chrome_options = Options()
    chrome_options.add_argument("--disable-gpu")
    chrome_options.add_argument("--no-sandbox")
    chrome_options.add_argument("--disable-dev-shm-usage")
    chrome_options.add_argument("--incognito")  # Adiciona o modo incógnito para não guardar cookies

    # Inicializa o navegador
    driver = webdriver.Chrome(service=Service(ChromeDriverManager().install()), options=chrome_options)

    try:
        # Acessa a página
        driver.get("https://papigames.com")

        # Espera o botão de login estar presente e clica nele
        login_button = WebDriverWait(driver, 20).until(
            EC.element_to_be_clickable((By.XPATH, "//button[contains(@class, 'loginButton')]"))
        )
        driver.execute_script("arguments[0].click();", login_button)

        # Espera a página de login carregar
        time.sleep(5)

        # Preenche o campo de username
        username_input = WebDriverWait(driver, 20).until(
            EC.presence_of_element_located((By.XPATH, "//input[@name='username']"))
        )
        username_input.send_keys("") # Insira seu nome de usuário

        # Preenche o campo de senha
        password_input = driver.find_element(By.XPATH, "//input[@name='password']")
        password_input.send_keys("") # Insira sua senha

        # Clica no botão de submit
        submit_button = WebDriverWait(driver, 20).until(
            EC.element_to_be_clickable((By.XPATH, "//button[contains(@class, 'styles_loginButton')]"))
        )
        driver.execute_script("arguments[0].click();", submit_button)

        # Espera a página de login processar
        time.sleep(5)

    finally:
        # Fecha o navegador
        driver.quit()

# Executa o script imediatamente
login_to_papigames()

# Agenda a execução do script a cada 24 horas
schedule.every(1).minute.do(login_to_papigames)

# Mantém o script rodando
while True:
    schedule.run_pending()
    time.sleep(1)
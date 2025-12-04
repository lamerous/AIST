# AIST

![Static Badge](https://img.shields.io/badge/lamerous-AIST-blue)
![Develop Platform](https://img.shields.io/badge/platform-Desktop-lightgray)
![Status](https://img.shields.io/badge/status-in%20develop-yellow)

![CPlusPlus](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Python](https://img.shields.io/badge/Python-FFD43B?style=for-the-badge&logo=python&logoColor=blue)
![Qt](https://img.shields.io/badge/Qt-41CD52?style=for-the-badge&logo=qt&logoColor=white)
![FastAPI](https://img.shields.io/badge/fastapi-109989?style=for-the-badge&logo=FASTAPI&logoColor=white)
![PostgreSQL](https://img.shields.io/badge/PostgreSQL-316192?style=for-the-badge&logo=postgresql&logoColor=white)

## 🌐 Доступные языки
- 🇬🇧 [English](README.md)
- 🇷🇺 [Русский](README.ru.md)

## 🚍️ Описание
Цифровая платформа для управления расписанием автобусов, продажей билетов, мониторингом транспортных средств и обслуживанием пассажиров. Предоставляет интуитивно понятный интерфейс для сотрудников и клиентов и автоматизирует рабочие процессы.
<div align="center">
  <img src="docs/screenshots/main.png">
</div>


## 🚀 Запуск сервера
1. Склонируйте GitHub репозиторий
```sh 
git clone https://github.com/lamerous/AIST
cd AIST
```

2. Запустите сервсис Docker (установите его, если ещё этого не сделали)
```sh
sudo systemctl start docker
```

3. Запустите Docker Compose скрипт
```sh
sudo docker-compose up -d
```
P.S. API будет доступен по адресу localhost:8001
    
## ⚙️ Launch From Source
Чтобы скомпилировать десктопный клиент используйте команды:
```sh
qmake
make
```
После этого исполняемый файл будет находиться по пути ./build/bin/AIST.

## 📄 License
Проект находится под лицензией [MIT](LICENSE).

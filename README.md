# AIST

![Static Badge](https://img.shields.io/badge/lamerous-AIST-blue)
![Develop Platform](https://img.shields.io/badge/platform-Desktop-lightgray)
![Status](https://img.shields.io/badge/status-in%20develop-yellow)

![CPlusPlus](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Python](https://img.shields.io/badge/Python-FFD43B?style=for-the-badge&logo=python&logoColor=blue)
![Qt](https://img.shields.io/badge/Qt-41CD52?style=for-the-badge&logo=qt&logoColor=white)
![FastAPI](https://img.shields.io/badge/fastapi-109989?style=for-the-badge&logo=FASTAPI&logoColor=white)
![PostgreSQL](https://img.shields.io/badge/PostgreSQL-316192?style=for-the-badge&logo=postgresql&logoColor=white)

## 🌐 Available Languages
- 🇬🇧 [English](README.md)
- 🇷🇺 [Русский](README.ru.md)

## 🚍️ Description
A digital platform for managing bus schedules, ticket sales, vehicle monitoring, and passenger services. Provides an intuitive interface for staff and customers and automates workflows.
<div align="center">
  <img src="docs/screenshots/main.png">
</div>


## 🚀 Quick Server Deploy
1. Clone GitHub repository:
```sh 
git clone https://github.com/lamerous/AIST
cd AIST
```

2. Start docker service (install docker if you haven't already)
```sh
sudo systemctl start docker
```

3. Run Docker Compose script to launch services
```sh
sudo docker-compose up -d
```
P.S. The API will be able on localhost:8001
    
## ⚙️ Launch From Source
To compile desktop client you need to run following commands:
```sh
qmake
make
```
After this, the executable file will be located at ./build/bin/AIST.

## 📄 License
This project is licensed under the [MIT license](LICENSE).

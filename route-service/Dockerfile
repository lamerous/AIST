FROM python:3.13-slim

WORKDIR /app

COPY . .

RUN apt-get update \
    && apt-get install -y \
    gcc \
    python3-dev \
    libpq-dev \
    build-essential \
    libuv1-dev \
    && apt-get clean

RUN pip install --no-cache-dir -U pip setuptools wheel && \
	pip install --no-cache-dir -r requirements.txt

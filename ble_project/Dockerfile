FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libbluetooth-dev \
    libglib2.0-dev \
    libdbus-1-dev \
    pkg-config \
    bluez \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

COPY . .

RUN make build

CMD ["/bin/bash"]

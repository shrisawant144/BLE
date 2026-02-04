.PHONY: all build clean setup test docs help central peripheral install

all: build

build:
	@mkdir -p build && cd build && cmake .. && make

central:
	@mkdir -p build && cd build && cmake -DBUILD_PERIPHERAL=OFF .. && make

peripheral:
	@mkdir -p build && cd build && cmake -DBUILD_CENTRAL=OFF .. && make

clean:
	@./scripts/clean.sh

setup:
	@./setup_bluetooth.sh

install:
	@./scripts/install_deps.sh

test:
	@./scripts/run_tests.sh

docs:
	@mkdir -p build && cd build && cmake -DBUILD_DOCS=ON .. && make

help:
	@echo "BLE Learning Project - Available Commands:"
	@echo "  make build      - Build entire project"
	@echo "  make central    - Build only central examples"
	@echo "  make peripheral - Build only peripheral examples"
	@echo "  make clean      - Remove build artifacts"
	@echo "  make setup      - Setup Bluetooth dependencies"
	@echo "  make install    - Install system dependencies"
	@echo "  make test       - Run tests"
	@echo "  make docs       - Generate documentation"
	@echo "  make help       - Show this help"

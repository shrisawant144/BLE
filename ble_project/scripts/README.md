# Scripts

Utility scripts for the BLE project.

## Available Scripts

### install_deps.sh
Installs system dependencies for Ubuntu/Debian, RHEL/CentOS, and Arch Linux.

```bash
./scripts/install_deps.sh
```

### clean.sh
Removes all build artifacts and temporary files.

```bash
./scripts/clean.sh
```

### run_tests.sh
Runs project tests and validates build artifacts.

```bash
./scripts/run_tests.sh
```

## Usage

All scripts are executable and can be run directly:

```bash
cd /path/to/ble_project
./scripts/install_deps.sh
```

Or via Makefile:

```bash
make install  # Runs install_deps.sh
make clean    # Runs clean.sh
make test     # Runs run_tests.sh
```

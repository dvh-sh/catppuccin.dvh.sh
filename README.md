# Catppuccin API

> High-performance, lightweight C++ REST API for serving Catppuccin theme data.

![C++](https://img.shields.io/badge/C++17-00599C?style=flat&logo=c%2B%2B&logoColor=white)
![Docker](https://img.shields.io/badge/Docker-2496ED?style=flat&logo=docker&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-blue.svg)

A backend service built for speed and low resource usage, designed to serve data related to the Catppuccin theming ecosystem. It is optimized for containerized deployments.

---

## Features

*   **High Performance**: Low-latency responses powered by `cpp-httplib` and aggressive C++ optimizations.
*   **Lightweight**: Minimal memory footprint and a small, multi-stage Docker image (<10MB).
*   **Production Ready**: Includes rate limiting, health checks, metrics, and graceful shutdown handling.
*   **Configurable**: All key parameters are managed via environment variables for deployment flexibility.
*   **Simple & Focused**: Provides a clean, read-only API for accessing ports, palettes, and other community data.

## Quick Start (Docker)

The recommended way to run this API is with Docker.

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/dvh-sh/catppuccin.dvh.sh.git
    cd catppuccin-api
    ```

2.  **Build and run using Docker Compose:**
    ```bash
    docker-compose up --build -d
    ```

The API will be available at `http://localhost:8080`.

## API Endpoints

All endpoints return JSON data.

| Method | Endpoint                    | Description                                  |
| :---   | :---                        | :---                                         |
| `GET`  | `/health`                   | Health check for monitoring.                 |
| `GET`  | `/metrics`                  | Provides server uptime and request metrics.  |
| `GET`  | `/palette`                  | Returns the complete Catppuccin color palette. |
| `GET`  | `/ports`                    | Lists all ports, paginated.                  |
| `GET`  | `/ports/:identifier`        | Retrieves a specific port by its key.        |
| `GET`  | `/collaborators`            | Lists all collaborators, paginated.          |
| `GET`  | `/collaborators/:username`  | Retrieves a specific collaborator.           |
| `GET`  | `/categories`               | Lists all categories, paginated.             |
| `GET`  | `/categories/:key`          | Retrieves a specific category.               |
| `GET`  | `/showcases`                | Lists all showcases, paginated.              |
| `GET`  | `/rate-limit-status`        | Shows the current rate limit for your IP.    |

**Pagination Query Parameters:**

*   `?page=<number>`: The page number to retrieve (default: `1`).
*   `?per_page=<number>`: The number of items per page (default: `20`).

---

## Configuration

The application is configured using environment variables.

| Variable        | Description                                  | Default      |
| :---            | :---                                         | :---         |
| `HOST`          | The network address to bind the server to.   | `0.0.0.0`    |
| `PORT`          | The port for the server to listen on.        | `8080`       |
| `THREADS`       | The number of worker threads for the server. | `4`          |
| `RATE_LIMIT`    | The number of requests allowed per window.   | `100`        |
| `RATE_WINDOW`   | The duration of the rate limit window (sec). | `3600`       |
| `CACHE_TTL`     | Cache time-to-live in seconds.               | `300`        |
| `VERBOSE`       | Enable verbose logging (`true`/`false`).     | `false`      |

---

## Building from Source

### Requirements

*   A C++17 compatible compiler (GCC 7+, Clang 6+)
*   `make`
*   `curl` (for fetching data)

### Build Commands

The Makefile provides targets for both production and debugging.

*   **Create a release build (optimized):**
    ```bash
    make release
    ```

*   **Create a debug build:**
    ```bash
    make debug
    ```

*   **Run the application:**
    ```bash
    ./catppuccin-api
    ```

*   **Clean build artifacts:**
    ```bash
    make clean
    ```

## Acknowledgements
- (nlohmann/json)[https://github.com/nlohmann/json]
- (yhirose/cpp-httplib)[https://github.com/yhirose/cpp-httplib]

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.



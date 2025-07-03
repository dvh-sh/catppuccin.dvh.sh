# Copilot Instructions for Catppuccin API

## Project Overview

This is a high-performance C++17 REST API serving Catppuccin theme data (ports, userstyles, categories, collaborators). The API is optimized for speed, low memory usage, and containerized deployment.

## Architecture & Technology Stack

- **Language**: C++17 with modern practices
- **Libraries**: nlohmann/json for JSON handling, cpp-httplib for HTTP server
- **Build System**: Makefile with aggressive compiler optimizations (-O3, -march=native, -flto)
- **Deployment**: Multi-stage Docker builds on Alpine Linux, deployed via Coolify
- **CI/CD**: GitHub Actions for automated builds and deployments

## Code Style & Standards

- Use JSDoc-style comments for functions and classes, but keep them concise and meaningful
- Prefer clean, readable code over excessive documentation
- Use `const` references where appropriate to avoid unnecessary copying
- Follow RAII principles and modern C++ best practices
- Keep dependencies minimal - avoid adding new external libraries unless absolutely necessary
- Use `snake_case` for variables and functions, `PascalCase` for classes/structs

## Performance Requirements

- Always prioritize performance and memory efficiency
- Use move semantics when appropriate
- Consider caching implications for data operations
- Minimize allocations in hot paths
- Prefer stack allocation over heap when possible

## API Design Patterns

- All endpoints return JSON with consistent error handling via `ApiResponse` struct
- Implement pagination for list endpoints using `page` and `per_page` parameters
- Include rate limiting for all public endpoints
- Follow RESTful conventions: GET for retrieval, proper HTTP status codes
- Support CORS for web browser access

## File Organization

- Source files in `src/` directory with `.cc` extension
- Headers in `src/` with `.hpp` extension
- External libraries in `include/` directory
- Static web assets in `public/` directory
- Keep related functionality grouped in logical modules (models, data_fetcher, rate_limiter, etc.)

## Docker & Deployment

- Use multi-stage builds to minimize final image size
- Install only essential runtime dependencies in final stage
- Run as non-root user for security
- Include health checks for container orchestration
- Optimize for both x86_64 and ARM64 architectures

## Data Handling

- Cache external API data to minimize GitHub API calls
- Validate data structure but avoid heavyweight schema validation
- Handle both JSON and YAML data sources (userstyles are YAML)
- Implement graceful fallbacks for data fetching failures
- Use pagination to handle large datasets efficiently

## Error Handling

- Use structured error responses with meaningful messages
- Log errors for debugging but avoid cluttering production output
- Implement graceful degradation when external services are unavailable
- Return appropriate HTTP status codes (404, 429, 500, etc.)

## Build & Development

- Support both release (optimized) and debug builds via Makefile targets
- Handle compiler differences between GCC and Clang automatically
- Use static linking where possible for portability
- Strip debug symbols from release builds for smaller binaries
- Test on both local development and containerized environments

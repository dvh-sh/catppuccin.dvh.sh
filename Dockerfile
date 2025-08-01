FROM alpine:3.18 AS builder

# Install build dependencies
RUN apk add --no-cache \
    g++ \
    make \
    curl \
    linux-headers \
    musl-dev

# Set working directory
WORKDIR /app

# Copy source files
COPY src/ ./src/
COPY include/ ./include/
COPY Makefile ./

# Build optimized binary
RUN make clean && make release

# Runtime stage - ultra minimal
FROM alpine:3.18

# Install runtime dependencies including yq
RUN apk add --no-cache \
    curl \
    ca-certificates \
    && rm -rf /var/cache/apk/* \
    && curl -L "https://github.com/mikefarah/yq/releases/latest/download/yq_linux_$(uname -m | sed 's/x86_64/amd64/; s/aarch64/arm64/')" \
    -o /usr/local/bin/yq \
    && chmod +x /usr/local/bin/yq

# Create non-root user for security
RUN addgroup -g 1000 catppuccin && \
    adduser -D -s /bin/sh -u 1000 -G catppuccin catppuccin

# Set working directory
WORKDIR /app

# Copy binary and public files from builder
COPY --from=builder /app/catppuccin-api .
COPY public/ ./public/

# Set ownership
RUN chown -R catppuccin:catppuccin /app

# Switch to non-root user
USER catppuccin

# Expose port
EXPOSE 3000

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:3000/health || exit 1

# Run the application
CMD ["./catppuccin-api"]

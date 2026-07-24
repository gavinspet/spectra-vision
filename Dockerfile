FROM ubuntu:22.04

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy project
COPY . .

# Build C++ backend
RUN cd backend && \
    mkdir -p build && \
    cd build && \
    cmake -G "Ninja" -DCMAKE_CXX_STANDARD=17 .. && \
    cmake --build . --parallel 4

# Expose port
EXPOSE 8080

# Run server
CMD ["./backend/build/bin/SpectraVision"]

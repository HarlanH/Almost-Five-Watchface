FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# System dependencies: Node.js 18, Python venv, Pebble emulator libs
RUN apt-get update \
 && apt-get install -y curl gnupg ca-certificates python3-pip python3.12-venv \
    libsdl1.2debian libfdt1 libpixman-1-0 libsdl2-2.0-0 \
 && curl -fsSL https://deb.nodesource.com/setup_18.x | bash - \
 && apt-get install -y nodejs \
 && rm -rf /var/lib/apt/lists/*

# Install uv (Python package manager) and pebble-tool
RUN curl -LsSf https://astral.sh/uv/install.sh | sh
ENV PATH="/root/.local/bin:${PATH}"
RUN uv tool install pebble-tool --python 3.12

# Install latest Pebble SDK
RUN pebble sdk install latest

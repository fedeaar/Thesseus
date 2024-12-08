FROM ubuntu:22.04

# Install global dependencies
RUN apt-get update && \
  apt-get install -y \
    git \
    cmake \
    gcc \
    clang \
    build-essential \
    libxmu-dev \
    libxi-dev \
    libgl-dev \
    libglu1-mesa-dev

RUN apt install -y \
  wget

RUN wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | \
  tee /etc/apt/trusted.gpg.d/lunarg.asc && \
  wget -qO /etc/apt/sources.list.d/lunarg-vulkan-jammy.list https://packages.lunarg.com/vulkan/lunarg-vulkan-jammy.list && \
  apt update && \
  apt install -y vulkan-sdk

FROM ubuntu:22.04

WORKDIR /Thesseus

# Install global dependencies
RUN apt-get update && apt-get install -y \
  git \
  cmake \
  gcc \
  clang \
  build-essential \
  libxmu-dev \
  libxi-dev \
  libgl-dev \
  libglu1-mesa-dev

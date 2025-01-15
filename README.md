# Thesseus

## initial setup

```bash
# install global dependencies
# likely will require sudoing
apt-get update
apt-get install \
    git \
    cmake \
    gcc \
    clang \
    build-essential \
    libxmu-dev \
    libxi-dev
apt update
apt install wget
wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | tee /etc/apt/trusted.gpg.d/lunarg.asc
wget -qO /etc/apt/sources.list.d/lunarg-vulkan-jammy.list https://packages.lunarg.com/vulkan/lunarg-vulkan-jammy.list
apt install vulkan-sdk

# install local dependencies
git submodule update --init --recursive --progress

# if any is missing
git submodule update --force
```

## build proyect

```bash
# inside the container
cd build

# build game
./build.sh -b 

# build debug version
./build.sh -d

# build test suite
./build.sh -t

# to run, add -r flag. for example:
./build.sh -t -r
```

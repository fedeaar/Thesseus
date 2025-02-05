# Thesseus

## initial setup

```bash
# install tools
# likely will require sudoing
apt-get update
apt-get install \
    git \
    cmake \
    gcc \
    clang \
    build-essential

# install global dependencies
apt-get libxmu-dev libxi-dev

# install vulkan sdk. See https://vulkan.lunarg.com/doc/sdk
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
# build game
build/build.sh -b 

# build debug version
build/build.sh -d

# build test suite
build/build.sh -t

# to run, add -r flag. for example:
build/build.sh -t -r
```

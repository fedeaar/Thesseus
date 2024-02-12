# Thesseus

## initial setup

```bash
# install basic tools
sudo apt-get install build-essential cmake gcc clang

# install basic deps
sudo apt-get install libxmu-dev libxi-dev libgl-dev

# install project deps
git submodule update --init --recursive --progress

# if any is missing
git submodule update --force
```

## build proyect
```bash
cd build

# build game
./build.sh -b 

# build debug version
./build.sh -d

# build test suite
./build.sh -t

# to run, add -r flag, for example
./build.sh -b -r
```

# Thesseus

## initial setup

```bash
# build image with global dependencies
docker compose -f ./docker/compose.yml up --build

# install local dependencies
git submodule update --init --recursive --progress

# if any is missing
git submodule update --force
```

## running the container

```bash
./start-container.sh
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

# if running -b or -d versions, this must be done 
# outside the container (after compiling)

# to build a different scene, add -s flag and specify 
# path in scene/collection folder. for example:
./build.sh -b -s "two_cubes.scene.cpp"
```

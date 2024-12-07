docker rm dev-thesseus
docker run \
  -it \
  -v $PWD:$PWD \
  -w $PWD \
  --name dev-thesseus \
  dev-thesseus 

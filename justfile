run:
  podman run -it --rm \
  --device=/dev/ttyUSB0 \
  -v $PWD:/config:z \
  docker.io/esphome/esphome \
   run m5atom.yaml

build:
  podman run -it --rm \
  -v $PWD:/config:z \
  docker.io/esphome/esphome \
   run m5atom.yaml


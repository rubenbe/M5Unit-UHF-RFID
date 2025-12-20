run:
  podman run -it --rm \
  --security-opt label=disable \
  --userns=host \
  --device=/dev/ttyUSB0 \
  -v $PWD:/config:z \
  docker.io/esphome/esphome \
   run m5atom.yaml

build:
  podman run -it --rm \
  --security-opt label=disable \
  --userns=host \
  -v $PWD:/config:z \
  docker.io/esphome/esphome \
   run m5atom.yaml

test:
  g++ src/my_helpers.cpp -DTEST && ./a.out

open-docker:
  podman run -it --rm \
  -u ${UID} \
  --security-opt label=disable \
  --userns=host \
  --device=/dev/ttyUSB0 \
  -v $PWD:/config:z \
  --entrypoint bash \
  docker.io/esphome/esphome

backtrace:
  podman run -it --rm \
  --security-opt label=disable \
  --userns=host \
  --device=/dev/ttyUSB0 \
  -v $PWD:/config:z \
  docker.io/esphome/esphome \
  logs m5atom.yaml

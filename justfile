# ESPHome M5 RFID build commands

# Default recipe - show available commands
default:
    @just --list

# Configuration
config := "m5atom.yaml"
image := "docker.io/esphome/esphome"
device := "/dev/ttyUSB0"

# Common podman options
podman_opts := "-it --rm --security-opt label=disable --userns=host -v $PWD:/config:z"

# Compile the firmware (no upload)
compile:
    podman run {{podman_opts}} {{image}} compile {{config}}

# Validate the ESPHome config
validate:
    podman run {{podman_opts}} {{image}} config {{config}}

# Build and upload firmware via USB
upload:
    podman run {{podman_opts}} --device={{device}} {{image}} upload {{config}}

# Build and upload, then show logs
run:
    podman run {{podman_opts}} --device={{device}} {{image}} run {{config}}

# Show device logs
logs:
    podman run {{podman_opts}} --device={{device}} {{image}} logs {{config}}

# Upload via OTA (over-the-air)
ota:
    podman run {{podman_opts}} {{image}} upload --device OTA {{config}}

# Interactive shell in container
shell:
    podman run {{podman_opts}} --device={{device}} --entrypoint bash {{image}}

# Run ESPHome dashboard on port 6052
dashboard:
    podman run {{podman_opts}} -p 6052:6052 {{image}} dashboard /config

# Clean build artifacts
clean:
    rm -rf .esphome

# Clean and rebuild
rebuild: clean compile

# Run C++ helper tests
test:
    g++ src/my_helpers.cpp -DTEST && ./a.out

# Show firmware size
size: compile
    @ls -lh .esphome/build/rfid01/.pioenvs/rfid01/firmware.bin 2>/dev/null || echo "Build first with: just compile"

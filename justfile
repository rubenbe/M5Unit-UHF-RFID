# ESPHome M5 RFID build commands

# Default recipe - show available commands
default:
    @just --list

# Configuration: just target=m5atom compile
target := ""
config := target + ".yaml"

_check-target:
    @test -n "{{target}}" || { echo "Specify target: just target=m5atom or just target=m5core"; exit 1; }
image := "docker.io/esphome/esphome"
device := "/dev/ttyUSB0"

# Common podman options
podman_opts := "-it --rm --security-opt label=disable --userns=host -v $PWD:/config:z"

# Compile the firmware (no upload)
compile: _check-target
    podman run {{podman_opts}} {{image}} compile {{config}}

# Validate the ESPHome config
validate: _check-target
    podman run {{podman_opts}} {{image}} config {{config}}

# Build and upload firmware via USB
upload: _check-target
    podman run {{podman_opts}} --device={{device}} {{image}} upload {{config}}

# Build and upload, then show logs
run: _check-target
    podman run {{podman_opts}} --device={{device}} {{image}} run {{config}}

# Show device logs
logs: _check-target
    podman run {{podman_opts}} --device={{device}} {{image}} logs {{config}}

# Upload via OTA (over-the-air)
ota: _check-target
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

# Toolchain path (inside container, mounted from local .esphome)
addr2line := "/config/.esphome/platformio/packages/toolchain-xtensa-esp-elf/bin/xtensa-esp32-elf-addr2line"
elf := "/config/.esphome/build/rfid01/.pioenvs/rfid01/firmware.elf"

# Decode a backtrace (paste the backtrace as argument)
# Usage: just decode "Backtrace: 0x400d1234:0x3ffb1234 0x400d5678:0x3ffb5678"
decode backtrace:
    #!/usr/bin/env bash
    addrs=$(echo "{{backtrace}}" | grep -oE '0x[0-9a-fA-F]+' | head -20 | tr '\n' ' ')
    podman run --rm --security-opt label=disable --userns=host -v $PWD:/config:z \
        --entrypoint {{addr2line}} {{image}} \
        -pfiaC -e {{elf}} $addrs

# Interactive backtrace decoder - enter addresses one per line
decode-interactive:
    @echo "Paste addresses (one per line, Ctrl+D when done):"
    @podman run -i --rm --security-opt label=disable --userns=host -v $PWD:/config:z \
        --entrypoint {{addr2line}} {{image}} \
        -pfiaC -e {{elf}}

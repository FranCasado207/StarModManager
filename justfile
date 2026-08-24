default:
    @just --list

configure:
    mkdir -p build
    cd build && cmake -G Ninja ..

build: configure
    cd build && ninja

run: build
    ./build/StarModManager

# limpa o build
clean:
    rm -rf build
    rm -rf ~/.config/Yakureito/StarModManager

.PHONY: clean build deps full-clean

deps: pico-sdk
	sudo apt install -y cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
	cd pico-sdk
	git pull
	cd ../

pico-sdk:
	git clone https://github.com/raspberrypi/pico-sdk --recurse-submodules

build: deps
	cmake -B build -DCMAKE_BUILD_TYPE=Debug -DPICO_SDK_PATH=pico-sdk
	cmake --build build --config Debug

clean:
	sudo rm -r build

full-clean: clean
	sudo rm -r pico-sdk
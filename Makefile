.PHONY: clean build deps libs

deps:
	sudo apt install -y cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib

libs:
	sudo rm -r pico-sdk
	git clone https://github.com/raspberrypi/pico-sdk --recurse-submodules

build: 
	cmake -B build -DCMAKE_BUILD_TYPE=Debug -DPICO_SDK_PATH=pico-sdk
	cmake --build build --config Debug

clean:
	sudo rm -r build
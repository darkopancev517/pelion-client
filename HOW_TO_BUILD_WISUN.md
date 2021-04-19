# TARGET: Vertexcom_WISUN_VC7300

# MCU: VC7300 A3B5 (with pseudo SRAM)

## Deploy WISUN sources files
```
python pal-platform/pal-platform.py deploy --target=Vertexcom_WISUN_VC7300
```

## Build WISUN menuconfig and Check 'Build with VCRTOS (CMAKE)' option and run program on PSRAM option
```
cd pal-platform/SDK/Vertexcom_WISUN_VC7300/wisun && make config

Note: make sure to check 'Build with VCRTOS (CMAKE)' options and run program on PSRAM options

in wisun .config file make sure the following options is set properly:

CONFIG_BUILD_WITH_VCRTOS=y
CONFIG_MCU_vc7300a_pseudo_sram=y
CONFIG_MCU_vc7300a_X_PSRAM=y

cd ../../../..

Note: back to pelion client root directory
```

## Generate WISUN build files and make
```
python pal-platform/pal-platform.py generate --target=Vertexcom_WISUN_VC7300 --mconfig=./pal-platform/SDK/Vertexcom_WISUN_VC7300/wisun/.config

cd __Vertexcom_WISUN_VC7300

mkdir build && cd build

cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=./../pal-platform/SDK/Vertexcom_WISUN_VC7300/ARMGCC.cmake -DEXTERNAL_DEFINE_FILE=./../define_Vertexcom_WISUN_VC7300_update.txt -DUPDATE_LINKING=1

make

Note: The correct WISUN image for VC7300 A3B5 will be: Debug/vertexcom_wisun_PSRAM_NODE.bin
```

## TODO: currently this build only support WISUN NODE devices

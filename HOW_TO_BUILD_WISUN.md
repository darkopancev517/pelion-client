# TARGET: Vertexcom_WISUN_VC7300

## Deploy WISUN sources files
```
python pal-platform/pal-platform.py deploy --target=Vertexcom_WISUN_VC7300
```

## Build WISUN menuconfig and Check 'Build with VCRTOS (CMAKE)' options
```
cd pal-platform/SDK/Vertexcom_WISUN_VC7300/wisun && make config

Note: make sure to check 'Build with VCRTOS (CMAKE)' options

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
```

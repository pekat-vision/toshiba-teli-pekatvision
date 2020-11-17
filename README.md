# Toshiba Teli & Pekat Vision example

The example shows how to connect to Toshiba Teli camera, acquire one image and analyze it in Pekat Vision. Both Python and C++ examples require Toshiba Teli
SDK to be installed on your system. You will also need to modify the code to reference your instance of Pekat server - just search for `host` in
Python and `pv_create_remote_analyzer` in C++.

## Python

[Pekat Vision SDK for Python](https://github.com/pekat-vision/pekat-vision-sdk-python) is required either installed or you can include it in
`PYTHONPATH` enviroment variable. You also need to put Toshiba Teli libraries path into LD_LIBRARY_PATH variable to correctly find dependencies:

```sh
TELICAMSDK=/opt/TeliCamSDK
export LD_LIBRARY_PATH="$TELICAMSDK/lib:$TELICAMSDK/genicam/bin/Linux64_x64:$LD_LIBRARY_PATH"
python3 pekat_teli_sample.py
```
On Windows you do not need to set paths to Toshiba Teli SDK libraries, just run the sample.

Note that attached `teli.py` module provides access to part of TeliCamSDK. It does not provide access to all the features or the SDK but only subset
needed to set camera parameters and acquire image using simple API.

## C++

[Pekat Vision SDK for ANSI C](https://github.com/pekat-vision/pekat-vision-sdk-ansi-c) is required for C++ example.

To compile and run the example on Linux you also need libCURL at least 7.62.0 with development support installed (`curl-config` must be in your
path).

```sh
# Path to Pekat SDK
PEKATSDK=pekat-vision-sdk-ansi-c
# Path to Toshiba Teli SDK
TELICAMSDK=/opt/TeliCamSDK
gcc "-I$TELICAMSDK/include" "-L$TELICAMSDK/lib" -lTeliCamApi -lTeliCamUtl `curl-config --cflags` `curl-config --libs` "-I$PEKATSDK" "$PEKATSDK/pekatvision/sdk_linux.c" pekat_teli_sample.cpp -o pekat_teli_sample
```

To run the sample, you need to set LD_LIBRARY_PATH as for Python:

```sh
export LD_LIBRARY_PATH="$TELICAMSDK/lib:$TELICAMSDK/genicam/bin/Linux64_x64:$LD_LIBRARY_PATH"
./pekat_teli_sample
```

On Windows, use attached Visual Studio project. You will need `TELICAMSDK` enviroment variable set (done by Toshiba Teli SDK installer) and also
Pekat Vision SDK in directory named `PekatVisionSDK` one level above the sample project (i.e. `..\PekatVisionSDK`). Or you can change the reference
to project and include directory. Then you can just compile and run the example.

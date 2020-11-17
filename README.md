## TeliCam SDK Python Wrapper + PEKAT VISION

### Usage
```shell
export TELICAMSDK=/opt/TeliCamSDK
export LD_LIBRARY_PATH=$TELICAMSDK/lib:$TELICAMSDK/genicam/bin/Linux64_x64:$LD_LIBRARY_PATH
python3 ./pekat_teli_sample.py
```

### Prerequisites
- Python 3.6+
- opencv-python
- https://github.com/pekat-vision/pekat-vision-sdk-python

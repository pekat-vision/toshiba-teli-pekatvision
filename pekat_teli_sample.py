#!/usr/bin/python

from teli import *
from PekatVisionSDK import Instance as Pekat

import cv2

initialize()

cameras = getNumOfCameras()
print("Number of cameras: %d" % cameras)

if (cameras):
    # At least one camera
    for i in range(cameras):
        info = getCameraInfo(i)
        print("Camera %d" % i)
        print("  Camera type: %s" % info.camType)
        print("  Manufacturer: %s" % info.manufacturer)
        print("  Model: %s" % info.modelName)
        print("  Serial no: %s" % info.serialNumber)
        print("  User name: %s" % info.userDefinedName)

    # Connect to Pekat
    pekat = Pekat(host = "127.0.0.1", port = 8100, already_running = True)

    with Camera(0) as cam:
        # How to get and set properties
        # String
        # print(cam.getStringValue("DeviceUserID"))
        # cam.setStringValue("DeviceUserID", "My camera")
        # Enum
        # print(cam.getEnumStringValue("TestPattern"))
        # cam.setEnumStringValue("TestPattern", "ColorBar")
        # Int
        # print(cam.getIntValue("AcquisitionFrameCount"))
        # cam.setIntValue("AcquisitionFrameCount", 2)
        # Float
        # print(cam.getFloatValue("ExposureTime"))
        # cam.setFloatValue("ExposureTime", 500)

        # Start streaming
        cam.startStream()
        # Wait for image 2000ms
        w = cam.waitForImage(2000)
        if (w):
            # Image received
            img = cam.getCurrentImage()
            # Analyze in Pekat
            img_res, _ = pekat.analyze(img, response_type = 'annotated_image')
            # Save original and result to disk
            cv2.imwrite('pict-orig.png', img)
            cv2.imwrite('pict-annotated.png', img_res)
        else:
            print("Image not received in time")
        # Stop streaming
        cam.stopStream()

terminate()

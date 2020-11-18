#ifdef _WIN32
    // Don't warn about fopen_s
    #define _CRT_SECURE_NO_WARNINGS
    #include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <TeliCamApi.h>
#include <TeliCamUtl.h>
#include <pekatvision/sdk.h>

using namespace Teli;

int main() {
    CAM_API_STATUS status;
    CAM_HANDLE hCam = (CAM_HANDLE)NULL;
    SIGNAL_HANDLE hSignal = (SIGNAL_HANDLE)NULL;
    CAM_STRM_HANDLE hStream = (CAM_STRM_HANDLE)NULL;
    void *buffer = NULL;
    void *image = NULL;
    pv_analyzer *pekat = NULL;
    uint32_t cameras;

    status = Sys_Initialize();
    if (status) {
        printf("Sys_Initialize error: 0x%08x\n", status);
        return 1;
    }

    do {
        status = Sys_GetNumOfCameras(&cameras);
        if (status) {
            printf("Sys_GetNumOfCameras error: 0x%08x\n", status);
            break;
        }
        printf("Number of cameras: %d\n", cameras);

        for (int i = 0; i < cameras; i++) {
            CAM_INFO info;

            status = Cam_GetInformation((CAM_HANDLE)NULL, i, &info);
            if (status) {
                printf("Cam_GetInformation error: 0x%08x\n", status);
                break;
            }

            printf("Camera %d\n", i);
            printf("  Camera type: %s\n", info.eCamType == CAM_TYPE_U3V ? "USB3" : info.eCamType == CAM_TYPE_GEV ? "GigE" : "unknown");
            printf("  Manufacturer: %s\n", info.szManufacturer);
            printf("  Model: %s\n", info.szModelName);
            printf("  Serial no: %s\n", info.szSerialNumber);
            printf("  User name: %s\n", info.szUserDefinedName);
        }

        if (cameras) {
            uint32_t maxBufSize;

            // Create Pekat analyzer
            pekat = pv_create_remote_analyzer("127.0.0.1", 8100, NULL);
            if (!pekat) {
                printf("Failed to create Pekat analyzer\n");
                break;
            }
            // Open camera
            status = Cam_Open(0, &hCam);
            if (status) {
                printf("Cam_Open error: 0x%08x\n", status);
                break;
            }

            // Here you could set various properties using SDK

            // Create signal
            status = Sys_CreateSignal(&hSignal);
            if (status) {
                printf("Sys_CreateSignal error: 0x%08x\n", status);
                break;
            }
            // Open stream
            status = Strm_OpenSimple(hCam, &hStream, &maxBufSize, hSignal);
            if (status) {
                printf("Strm_OpenSimple error: 0x%08x\n", status);
                break;
            }
            // Prepare buffer
            buffer = malloc(maxBufSize);
            if (!buffer) {
                printf("Cannot allocate %d bytes of memory\n", maxBufSize);
                break;
            }
            // Start streaming
            status = Strm_Start(hStream);
            if (status) {
                printf("Strm_Start error: 0x%08x\n", status);
                break;
            }
            // Wait for image
            status = Sys_WaitForSignal(hSignal, 2000);
            if (status == CAM_API_STS_SUCCESS) {
                // Image ready
                CAM_IMAGE_INFO imageInfo;
                uint32_t bufSize = maxBufSize;

                // Get image
                status = Strm_ReadCurrentImage(hStream, buffer, &bufSize, &imageInfo);
                if (status) {
                    printf("Strm_ReadCurrentImage error: 0x%08x\n", status);
                    break;
                }
                // Allocate buffer for RGB image
                image = malloc(imageInfo.uiSizeX * imageInfo.uiSizeY * 3);
                if (!image) {
                    printf("Cannot allocate memory for %dx%d image\n", imageInfo.uiSizeX, imageInfo.uiSizeY);
                    break;
                }
                // Convert to RGB
                status = ConvImage(DST_FMT_BGR24, imageInfo.uiPixelFormat, true, image, buffer, imageInfo.uiSizeX, imageInfo.uiSizeY);
                if (status) {
                    printf("ConvImage error: 0x%08x\n", status);
                    break;
                }
                // Save as bitmap
                status = SaveBmpRGB(image, imageInfo.uiSizeX, imageInfo.uiSizeY, "pict-orig.bmp");
                if (status) {
                    printf("SaveBmpRGB error: 0x%08x\n", status);
                    break;
                }
                // Analyze by Pekat
                int res = pv_analyze_raw_image(pekat, (char*)image, imageInfo.uiSizeX, imageInfo.uiSizeY, PVRT_ANNOTATED_IMAGE, NULL);
                if (res) {
                    printf("Analysis in Pekat failed: %d\n", res);
                    break;
                } else {
                    char *res_image = pv_get_result_data(pekat);
                    int res_size = pv_get_result_data_size(pekat);
                    FILE *f = fopen("pict-annotated.png", "wb");
                    if (f) {
                        fwrite(res_image, res_size, 1, f);
                        fclose(f);
                    } else {
                        printf("Cannot save Pekat result image to disk\n");
                    }
                }
            } else if (status == CAM_API_STS_TIMEOUT) {
                // Not received
                printf("Image not received in time\n");
            } else {
                printf("Sys_WaitForSignal error: 0x%08x\n", status);
                break;
            }
        }
    } while (0);

    // Cleanup
    if (hStream) {
        Strm_Stop(hStream);
        Strm_Close(hStream);
    }
    if (hSignal) {
        Sys_CloseSignal(hSignal);
    }
    if (hCam) {
        Cam_Close(hCam);
    }
    if (image) {
        free(image);
    }
    if (buffer) {
        free(buffer);
    }
    if (pekat) {
        pv_free_analyzer(pekat);
    }
    Sys_Terminate();
    return status ? 1 : 0;
}

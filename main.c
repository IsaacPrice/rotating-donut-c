#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#define DEFAULT_WIDTH 60
#define DEFAULT_HEIGHT 30

char distanceMappings[] = ".:-=+*#%@";

int main(int argc, char *argv[]) {
    int width = DEFAULT_WIDTH;
    int height = DEFAULT_HEIGHT;
    double thetaSpeed = 0.04;
    double phiSpeed = 0.02;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--width") == 0 && i + 1 < argc) {
            width = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--height") == 0 && i + 1 < argc) {
            height = atoi(argv[++i]);
        } else if (strcmp((argv[i]), "--thetaSpeed") == 0 && i + 1 < argc) {
            thetaSpeed = atof(argv[++i]);
        } else if (strcmp(argv[i], "--phiSpeed") == 0 && i + 1 < argc) {
            phiSpeed = atof(argv[++i]);
        } else {
            printf("Invalid argument: %s\n", argv[i]);
            return 1;
        }
    }

    double R = 2.0; 
    double r = 1.0; 
    double K2 = 5.0;
    double K1 = width * K2 * 3 / (8 * (R + r));
    
    double theta = 0.0;
    double phi = 0.0;

    int bufferSize = (width + 1) * height;
    char screenBuffer[bufferSize];
    double zBuffer[width * height];

    // Loop over angles
    while (1 == 1) {
        memset(screenBuffer, ' ', bufferSize - 1);
        screenBuffer[bufferSize - 1] = '\0';
        memset(zBuffer, 0, sizeof(zBuffer));

        system("clear");
        for (double u = 0; u < 2 * M_PI; u += 0.07) {         // Small circle angle
            for (double v = 0; v < 2 * M_PI; v += 0.02) {     // Big circle angle
                
                // 3D Coordinates before rotation
                double x = (R + r * cos(u)) * cos(v);
                double y = (R + r * cos(u)) * sin(v);
                double z = r * sin(u);

                // Rotate around Y-axis
                double xPrime = x * cos(theta) + z * sin(theta);
                double zPrime = -x * sin(theta) + z * cos(theta);

                // Rotate around X-axis
                double yPrime = (y * cos(phi) - zPrime * sin(phi)) * .5;
                double zDoublePrime = y * sin(phi) + zPrime * cos(phi);

                double ooz = 1.0 / (K2 + zDoublePrime);
                int xScreen = (int)(width / 2 + K1 * xPrime * ooz);
                int yScreen = (int)(height / 2 - K1 * yPrime * ooz);

                // Calculate normal vector for the donut surface (simple approximation)
                double nx = cos(u) * cos(v);
                double ny = cos(u) * sin(v);
                double nz = sin(u);

                // Normalize the normal vector
                double normalLength = sqrt(nx * nx + ny * ny + nz * nz);
                nx /= normalLength;
                ny /= normalLength;
                nz /= normalLength;

                // Light direction (simple, facing -Z direction)
                double lx = 0.0, ly = 0.0, lz = -1.0; // Light from viewer (along negative Z-axis)

                // Dot product of normal and light direction to simulate basic lighting
                double dotProduct = nx * lx + ny * ly + nz * lz;

                // Normalize the dot product to range 0–1 (clamp if negative)
                double luminance = fmax(0.0, dotProduct); 

                int distanceIndex = (int)(luminance * (sizeof(distanceMappings) - 2)); // Adjust brightness

                if (xScreen >= 0 && xScreen < width && yScreen >= 0 && yScreen < height) {
                    int idx = yScreen * width + xScreen; // No newline padding for zBuffer
                    int bufIdx = yScreen * (width + 1) + xScreen; // With padding for screenBuffer

                    if (ooz > zBuffer[idx]) { // Depth check
                        zBuffer[idx] = ooz;
                        screenBuffer[bufIdx] = distanceMappings[distanceIndex];
                    }
                }
            }
        }

        for (int i = 0; i < height; i++) {
            screenBuffer[i * (width + 1) + width] = '\n';
        }
        
        printf("%s", screenBuffer);
        fflush(stdout);
        usleep(83333); // 41.666 ms (24 fps)
        theta += thetaSpeed;
        phi += phiSpeed;
    }

    return 0;
}

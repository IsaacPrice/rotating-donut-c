#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define DEFAULT_WIDTH 60
#define DEFAULT_HEIGHT 30
#define DEFAULT_THETA_SPEED .05
#define DEFAULT_PHI_SPEED .01667
#define DEFAULT_FRAME_RATE 24

char distanceMappings[] = ".-+=%@#";

int main(int argc, char *argv[]) {
    int width = DEFAULT_WIDTH;
    int height = DEFAULT_HEIGHT;
    double thetaSpeed = DEFAULT_THETA_SPEED;
    double phiSpeed = DEFAULT_PHI_SPEED;
    int frameRate = DEFAULT_FRAME_RATE;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--width") == 0 && i + 1 < argc) {
            width = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--height") == 0 && i + 1 < argc) {
            height = atoi(argv[++i]);
        } else if (strcmp((argv[i]), "--thetaSpeed") == 0 && i + 1 < argc) {
            thetaSpeed = atof(argv[++i]);
        } else if (strcmp(argv[i], "--phiSpeed") == 0 && i + 1 < argc) {
            phiSpeed = atof(argv[++i]);
        } else if (strcmp(argv[i], "--frameRate") == 0 && i + 1 < argc) {   
            frameRate = atoi(argv[++i]);
        } else {
            printf("Invalid argument: %s\n", argv[i]);
            return 1;
        }
    }

    thetaSpeed /= frameRate;
    phiSpeed /= frameRate;
    double frameTime = 1.0 / frameRate;

    double R = 2.0; 
    double r = 1.0; 
    double K2 = 5.0;
    double K1 = width * K2 * 3 / (8 * (R + r));
    
    double theta = 0.0;
    double phi = 0.0;

    int bufferSize = (width + 1) * height + 1;
    char screenBuffer[bufferSize];
    double zBuffer[width * height];

    clock_t startTime;

    while (1) {
        startTime = clock();

        memset(screenBuffer, ' ', bufferSize - 1);
        screenBuffer[bufferSize - 1] = '\0';
        memset(zBuffer, 0, sizeof(zBuffer));

        for (double u = 0; u < 2 * M_PI; u += 0.07) {         
            for (double v = 0; v < 2 * M_PI; v += 0.02) {    
                
                double x = (R + r * cos(u)) * cos(v);
                double y = (R + r * cos(u)) * sin(v);
                double z = r * sin(u);

                double xPrime = x * cos(theta) + z * sin(theta);
                double zPrime = -x * sin(theta) + z * cos(theta);

                double yPrime = (y * cos(phi) - zPrime * sin(phi)) * .5;
                double zDoublePrime = y * sin(phi) + zPrime * cos(phi);

                double ooz = 1.0 / (K2 + zDoublePrime);
                int xScreen = (int)(((float)width / 2.0) + K1 * xPrime * ooz);
                int yScreen = (int)(((float)height / 2.0) - K1 * yPrime * ooz);

                double nx = cos(u) * cos(v);
                double ny = cos(u) * sin(v);
                double nz = sin(u);

                double normalLength = sqrt(nx * nx + ny * ny + nz * nz);
                nx /= normalLength;
                ny /= normalLength;
                nz /= normalLength;

                double lx = 0.0, ly = 0.0, lz = -1.0;
                double dotProduct = nx * lx + ny * ly + nz * lz;
                double luminance = fmax(0.0, dotProduct); 
                int distanceIndex = (int)(luminance * (sizeof(distanceMappings) - 2));

                if (xScreen >= 0 && xScreen < width && yScreen >= 0 && yScreen < height) {
                    int idx = yScreen * width + xScreen; 
                    int bufIdx = yScreen * (width + 1) + xScreen;

                    if (ooz > zBuffer[idx]) { 
                        zBuffer[idx] = ooz;
                        screenBuffer[bufIdx] = distanceMappings[distanceIndex];
                    }
                }
            }
        }

        for (int i = 0; i < height; i++) {
            screenBuffer[i * (width + 1) + width] = '\n';
        }

        clock_t endTime = clock();
        double deltaTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
        double remainingTime = frameTime - deltaTime;

        if (remainingTime > 0) {
            usleep((useconds_t) (remainingTime * 1000000));
        }

        startTime = endTime;
        
        printf("\033[2J\033[H");
        fflush(stdout);
        write(1, screenBuffer, sizeof(screenBuffer) - 1);
        
        theta += thetaSpeed;
        phi += phiSpeed;
    }

    return 0;
}

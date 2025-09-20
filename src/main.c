#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define cellsX 10
#define cellsY 10
#define pixelsPerCell 100

#define WIDTH (cellsX * pixelsPerCell)
#define HEIGHT (cellsY * pixelsPerCell)

typedef struct {
    float x, y; 
} Vec2;

// dot product
float dot(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

// linear interpolation
float linterp(float a, float b, float d) {
    return a + d * (b - a);
}

// smoothing for edges of the cells
float fade (float t){
    return t * t * t * (t * (t * 6 - 15) + 10);
}


int main() {
    srand(time(NULL));

    // shared angles (theta) all 4 corners must match for all in the whole grid
    Vec2 theta[cellsY+1][cellsX+1];
    for(int i = 0; i <= cellsY; i++) {
        for(int j = 0; j <= cellsX; j++) {
            float angle = ((float)rand() / RAND_MAX) * 2.0f * 3.14159265358979f;
            theta[i][j].x = cos(angle);
            theta[i][j].y = sin(angle);
        }
    }

    // define the corners vectors theta
    Vec2 BL = {-1,0};
    Vec2 BR = {0,-1};
    Vec2 TL = {1,0};
    Vec2 TR = {0,1};

    static int perlin[HEIGHT][WIDTH];


    for(int gridPosY = 0; gridPosY < cellsY; gridPosY++) {
        for(int gridPosX = 0; gridPosX < cellsX; gridPosX++) {

    
            // loop for the pixels in the grid pos
            for(int i = 0; i < pixelsPerCell; i++){
                for(int j = 0; j < pixelsPerCell; j++){

                    // convert the pos on 0-9 to 0-1
                    float x = (float)j / (pixelsPerCell);
                    float y = (float)i / (pixelsPerCell);

                    // find distance from point to corner
                    Vec2 distBL = {x - 0, y - 0};
                    Vec2 distBR = {x - 1, y - 0};
                    Vec2 distTL = {x - 0, y - 1};
                    Vec2 distTR = {x - 1, y - 1};

                    // theta for the corners of this cell
                    Vec2 gBL = theta[gridPosY][gridPosX];
                    Vec2 gBR = theta[gridPosY][gridPosX + 1];
                    Vec2 gTL = theta[gridPosY + 1][gridPosX];
                    Vec2 gTR = theta[gridPosY + 1][gridPosX + 1];

                    // dot product calc
                    float dbBL = dot(gBL, distBL);
                    float dbBR = dot(gBR, distBR);
                    float dbTL = dot(gTL, distTL);
                    float dbTR = dot(gTR, distTR);

                    float smoothX = fade(x);
                    float smoothY = fade(y);

                    // linear interpolation calc
                    float ixB = linterp(dbBL, dbBR, smoothX);
                    float ixT = linterp(dbTL, dbTR, smoothX);
                    float value = linterp(ixB, ixT, smoothY);

                    // to display
                    int display = (int)((value + 1)* 127.5f);
                    display = (display - 128) * 2 + 128;
                    if(display < 0) display = 0;
                    if(display > 255) display = 255;

                    int px = gridPosX * pixelsPerCell + j;
                    int py = gridPosY * pixelsPerCell + i;

                    perlin[py][px] = display;
                }
            }
        }
    }

    // write to file
    FILE *f = fopen("Perlin.ppm", "w");
    if (!f) {
        perror("failed to write file");
        return 1;
    }

    fprintf(f, "P3\n%d %d\n255\n", WIDTH, HEIGHT);

    for(int i = 0; i<HEIGHT; i++) {
        for(int j = 0; j<WIDTH; j++) {
            float t = perlin[i][j] / 255.0f;  // t is now 0.0 .. 1.0

            int r = (int)(255 * (0.5f + 0.5f * sin(6.28f * t + 0.0f)));  // red
            int g = (int)(255 * (0.5f + 0.5f * sin(6.28f * t + 2.0f)));  // green
            int b = (int)(255 * (0.5f + 0.5f * sin(6.28f * t + 4.0f)));  // blue

            fprintf(f, "%d %d %d ", r, g, b);
        }
        fprintf(f, "\n");
    }

    fclose(f);

    printf("file wrote to Perlin.ppm\n");

    // auto open the file on linux
    system("xdg-open Perlin.ppm");
    

    return 0;
}

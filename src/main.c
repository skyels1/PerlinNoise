#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define row 30
#define col 30

typedef struct {
    float x, y; 
} Vec2;

float dot(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

float linterp (float a, float b, float d) {
    return a + d * (b - a);
}


int main() {

    // define the corners vectors theta
    Vec2 BL = {-1,0};
    Vec2 BR = {0,-1};
    Vec2 TL = {1,0};
    Vec2 TR = {0,1};

    int boxSize = 30;
    static int perlin[row][col];

    for(int i = 0; i < boxSize; i++){
        for(int j = 0; j < boxSize; j++){

            // convert the pos on 0-9 to 0-1
            float x = (float)j / (boxSize - 1);
            float y = (float)i / (boxSize - 1);

            Vec2 distBL = {x - 0, y - 0};
            Vec2 distBR = {x - 1, y - 0};
            Vec2 distTL = {x - 0, y - 1};
            Vec2 distTR = {x - 1, y - 1};

            float dbBL = dot(BL, distBL);
            float dbBR = dot(BR, distBR);
            float dbTL = dot(TL, distTL);
            float dbTR = dot(TR, distTR);

            float ixB = linterp(dbBL, dbBR, x);
            float ixT = linterp(dbTL, dbTR, x);
            float value = linterp(ixB, ixT, y);

            int display = (int)((value + 1)* 127.5f);
            if(display < 0) display = 0;
            if(display > 255) display = 255;

            perlin[i][j] = display;
        }
    }

    FILE *f = fopen("Perlin.ppm", "w");
    if (!f) {
        perror("failed to write file");
        return 1;
    }

    fprintf(f, "P3\n%d %d\n255\n", col, row);

    for(int i = 0; i<row; i++) {
        for(int j = 0; j<col; j++) {
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

    system("xdg-open Perlin.ppm");
    

    return 0;
}

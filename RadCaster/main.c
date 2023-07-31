//
//  main.c
//  RadCaster
//
//  Created by Mahmoud Radwan on 28/07/2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <GLUT/GLUT.h>
#include <math.h>
#include <SOIL/SOIL.h>
#define PI 3.1415926535
#define PI2 PI/2
#define PI3 3*PI/2
#define DR 0.0174533
GLuint textureID; 
// these are the coordinates of our player
float px,py,pdx,pdy,pa;

// this function draws our squared player
void drawPlayer(){
    //this draws the colors, the parameters are in RGB, in range of 0.0 to 1.0
    glColor3f(1, 1, 0);
    // this defines the sizw of the point
    glPointSize(8);
    //this indicates the start of primitive drawing mode
    glBegin(GL_POINTS);
    //this draws it in the coordinates
    glVertex2i(px,py);
    glEnd();
    
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(px, py);
    glVertex2i(px+pdx*5, py+pdy*5);
    glEnd();
}

int mapX=8, mapY=8,mapS=64;
int map[]= {
  1,1,1,1,1,1,1,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,1,0,0,0,0,1,
  1,0,0,0,0,1,0,1,
  1,0,0,0,0,1,0,1,
  1,0,0,0,0,1,0,1,
  1,1,1,1,1,1,1,1,
};

void drawMap2D() {
    int x, y, x0, y0;
    glEnable(GL_TEXTURE_2D);

    for (y = 0; y < mapY; y++) {
        for (x = 0; x < mapX; x++) {
            if (map[y * mapX + x] == 1) {
                // Calculate the screen coordinates of the current rectangle
                x0 = x * mapS;
                y0 = y * mapS;

                // Load the local image "texture.jpg" as a texture
                if (textureID == 0) {
                    textureID = SOIL_load_OGL_texture(
                        "texture.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID,
                        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
                    );

                    if (textureID == 0) {
                        printf("Error loading texture.\n");
                        return;
                    }
                }

                glBindTexture(GL_TEXTURE_2D, textureID);

                // Enable texturing and bind the loaded texture
                glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f);
                glVertex2i(x0 + 1, y0 + 1);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2i(x0 + 1, y0 + mapS - 1);
                glTexCoord2f(1.0f, 1.0f);
                glVertex2i(x0 + mapS - 1, y0 + mapS - 1);
                glTexCoord2f(0.0f, 1.0f);
                glVertex2i(x0 + mapS - 1, y0 + 1);
                glEnd();
            }
        }
    }

    glDisable(GL_TEXTURE_2D);
}

float dist(float ax, float ay, float bx, float by, float ang) {
    return (sqrt((bx-ax) * (bx-ax) + (by-ay) * (by-ay)));
}
void draw2dRays(){
    int r,mx,my,mp, d0f; float rx,ry, ra, x0, y0, disT;
    ra = pa-DR*30; if(ra<0) {ra +=2*PI;} if(ra>2*PI){ra -= 2*PI;}
    
    for(r=0;r<60;r++){
        float disH= 10000000, hx = px, hy = py;
        d0f=0;
        float aTan = -1/tan(ra);
        if (ra>PI) { ry=(((int)py>>6)<<6)-0.0001; rx=(py-ry) * aTan+px; y0=-64; x0=-y0*aTan;}
        if (ra<PI) { ry=(((int)py>>6)<<6)+64;rx=(py-ry) * aTan+px; y0=64; x0=-y0*aTan;}
        if (ra==0 ||  ra==PI) {rx=px; ry=py; d0f=8;}
        while (d0f < 8) {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp= my * mapX + mx;
            if (mp > 0 && mp<mapX*mapY && map[mp]==1)  {hx = rx; hy = ry; disH = dist(px, py, hx, hy, ra); d0f=8;}
            else { rx+=x0; ry+=y0; d0f+=1;}
        }

        d0f=0;
        float disV= 10000000, vx = px, vy = py;
        float nTan = -tan(ra);
        if (ra>PI2 && ra<PI3) { rx=(((int)px>>6)<<6)-0.0001; ry=(px-rx) * nTan+py; x0=-64; y0=-x0*nTan;}
        if (ra<PI2 || ra>PI3) { rx=(((int)px>>6)<<6)+64;     ry=(px-rx) * nTan+py; x0= 64; y0=-x0*nTan;}
        if (ra==0 ||  ra==PI) {rx=px; ry=py; d0f=8;}
        while (d0f < 8) {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp= my * mapX + mx;
            if (mp > 0 && mp<mapX*mapY && map[mp]==1)  {vx = rx; vy = ry; disV = dist(px, py, vx, vy, ra);d0f=8;}
            else { rx+=x0; ry+=y0; d0f+=1;}
        }
        
        if (disV<disH) { rx=vx; ry=vy; disT = disV; glColor3f(0.9, 0, 0); }
        if (disH<disV) { rx=hx; ry=hy; disT = disH; glColor3f(0.7, 0, 0); }
        
        glLineWidth(3);
        glBegin(GL_LINES);
        glVertex2i(px, py);
        glVertex2i(rx, ry);
        glEnd();
        
        float ca=pa-ra; if (ca<0) {ca+=2*PI;} if (ca>2*PI) {ca-=2*PI;} disT=disT*cos(ca);
        float LineH = (mapS*320) / disT; if(LineH > 320) {LineH = 320;}
        float LineO = 160-LineH/2;
        glLineWidth(8); glBegin(GL_LINES);glVertex2i(r*8+530, LineO); glVertex2i(r*8+530, LineH+LineO);glEnd();
        ra += DR; if(ra<0) {ra +=2*PI;} if(ra>2*PI){ra -= 2*PI;}
    }
}


void display(){
    //this clears the color buffers and the depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap2D();
    drawPlayer();
    draw2dRays();
    glutSwapBuffers();
    
}

void buttons(unsigned char key, int x, int y){
    
    if(key=='a') {pa-=0.1; if(pa<0){pa+=2*PI;} pdx=cos(pa) * 5; pdy=sin(pa)*5; }
    if(key=='d') {pa+=0.1; if(pa>2*PI){pa-=2*PI;} pdx=cos(pa) * 5; pdy=sin(pa)*5;}
    if(key=='w') {px+=pdx; py+=pdy;}
    if(key=='s') {px-=pdx; py-=pdy;}
    printf("%9.0af", px);
    //this makes it redisplay everytime to update the screen
    glutPostRedisplay();
}

void init(){
    glClearColor(0.3,0.3,0.3, 0);
    gluOrtho2D(0,1024,512,0);
    px=300;
    py= 300;
    pdx=cos(pa) * 5; pdy=sin(pa)*5;
}

int main(int argc, char* argv[]){
    glutInit (&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (1024, 512);
    glutCreateWindow ("RadCaster");
    init();
    glutDisplayFunc (display);
    glutKeyboardFunc(buttons);
    glutMainLoop ();
}

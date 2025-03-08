
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>
#include <math.h>


GLsizei wh = 750, ww = 1200; // 視窗大小
float squX = 30, squY = 30;  // 戰車位置
float dir = 1;               // 方向
int isShoot = 0;             // 子彈數
int sphNo = 0;               // 球數
float bSize = 40;            // 子彈大小
bool win = false;            // 是否贏
bool loose = false;          // 是否輸
int mode = 1;                // 關卡
int score = 0, tmpScore = 0; // 分數
int obsX = 0;                // 障礙物位置
int A = 10;                  // 障礙物移動變數
int bulletLast = 20;
int pressTime = 0;

// 球的結構
struct sphere {
    bool live;  // 是否存在
    int type;   // 大中小球
    float bx;   // 位置
    float by;
    float size; // 大小
    float xStep; // 位移量
    float yStep;
};

// 子彈的結構
struct bullet {
    bool live; // 是否存在
    float bx;  // 位置
    float by;
    int dir;   // 方向
};

sphere *sArray = new sphere[25];
bullet *bArray = new bullet[1000];

// 畫球
void renderSphere(float x, float y, float z, float radius, int subdivisions, GLUquadricObj *quadric) {
    glPushMatrix();
    glTranslatef(x, y, z);
    gluSphere(quadric, radius, subdivisions, subdivisions);
    glPopMatrix();
}

// 顯示文字
void drawText(const char *message, int x, int y) {
    glScalef(1.0, 1.0, 1.0);
    glRasterPos2f((GLfloat)x, (GLfloat)y);
    while (*message) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *message++);
    }
}

// 畫障礙物
void obstacle() {
    glColor3f(1, 0, 0);
    switch (mode) {
        case 1:
        case 5:
        case 6:
            glRectf(0, 80, 80, 100);
            break;
        case 2:
            glRectf(0, 80, 80, 100);
            glRectf(720, 80, 800, 100);
            break;
        case 3:
            glRectf(320, 80, 460, 100);
            break;
        case 4:
            glRectf(obsX, 200, obsX + 150, 240);
            break;
    }
}

// 繪製遊戲畫面
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    obstacle();

    // 畫坦克
    glColor3f(loose ? 0.5f : 0.0f, loose ? 0.5f : 1.0f, 0.0f);
    glBegin(GL_POLYGON);

    if (dir == 1) { // 右
        glVertex2f(squX + 30, squY + 10);
        glVertex2f(squX + 30, squY + 30);
        glVertex2f(squX - 30, squY + 30);
        glVertex2f(squX - 30, squY - 30);
        glVertex2f(squX + 30, squY - 30);
        glVertex2f(squX + 30, squY - 10);
        glVertex2f(squX + 50, squY - 10);
        glVertex2f(squX + 50, squY + 10);
    } else if (dir == 2) { // 左
        glVertex2f(squX - 30, squY + 10);
        glVertex2f(squX - 30, squY + 30);
        glVertex2f(squX + 30, squY + 30);
        glVertex2f(squX + 30, squY - 30);
        glVertex2f(squX - 30, squY - 30);
        glVertex2f(squX - 30, squY - 10);
        glVertex2f(squX - 50, squY - 10);
        glVertex2f(squX - 50, squY + 10);
    } else if (dir == 3) { // 上
        glVertex2f(squX - 10, squY + 30);
        glVertex2f(squX - 10, squY + 50);
        glVertex2f(squX + 10, squY + 50);
        glVertex2f(squX + 10, squY + 30);
        glVertex2f(squX + 30, squY + 30);
        glVertex2f(squX + 30, squY - 30);
        glVertex2f(squX - 30, squY - 30);
        glVertex2f(squX - 30, squY + 30);
    }

    glEnd();

    // 畫子彈
    glColor3f(1, 0, 0);
    for (int i = 0; i < isShoot; i++) {
        if (bArray[i].live) {
            renderSphere(bArray[i].bx, bArray[i].by, bSize, bSize, 100, gluNewQuadric());
        }
    }

    // 畫球
    for (int i = 0; i <= sphNo; i++) {
        if (sArray[i].live) {
            if (sArray[i].type == 1) glColor3f(0, 0, 1);
            if (sArray[i].type == 2) glColor3f(1, 0, 1);
            if (sArray[i].type == 3) glColor3f(1, 1, 0);
            renderSphere(sArray[i].bx, sArray[i].by, 1000, 1000, 200, gluNewQuadric());
        }
    }

    // 顯示分數
    glColor3f(1, 1, 1);
    drawText("Score : ", 0, 480);
    char s[64];
    printf(s, "%d", score);
    drawText(s, 80, 480);

    // 遊戲結束或勝利訊息
    if (loose) {
        drawText("Game Over", 300, 250);
        drawText("Press S to restart", 300, 200);
    }
    if (win) {
        drawText("You Win", 300, 250);
        drawText("Press Enter to next", 300, 200);
    }

    glFlush();
}

// 初始化
void myinit() {
    glViewport(0, 0, ww, wh);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (GLdouble)ww, 0.0, (GLdouble)wh, -1.0, 1.0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

// 主程式
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(ww, wh);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Bubble Shooter Game");
    myinit();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}

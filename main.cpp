#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>
#include <math.h>

GLsizei wh = 750, ww = 1200; // windows長寬
float squX = 30, squY = 30; // 戰車位置
float dir = 1; // 方向
int isShoot = 0; // 子彈數
int sphNo = 0; // 球數
float bSize = 40; // 子彈大小
bool win = false; // 是否贏
bool loose = false; // 是否輸
int mode = 1; // 關卡
int score = 0, tmpScore = 0; // 分數
int obsX = 0; // 障礙物 X 位置
int A = 10;

int bulletLast = 20;
int pressTime = 0;

// 球的結構
struct sphere {
    bool live;  // 是否存在
    int type;   // 大、中、小球
    float bx;   // 位置
    float by;
    float size; // 大小
    float xStep; // 位移量
    float yStep;
};

// 子彈的結構
struct bullet {
    bool live;  // 是否存在
    float bx;   // 位置
    float by;
    int dir;    // 方向
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

// 文字顯示
void drawText(const char *message, int x, int y) {
    glScalef(1.0, 1.0, 1.0);
    glRasterPos2f((GLfloat)x, (GLfloat)y);
    while (*message) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *message++);
    }
}

// 障礙物
void obstacle() {
    glColor3f(1, 0, 0);
    switch(mode) {
        case 1:
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
        case 5:
            glRectf(0, 80, 80, 100);
            break;
        case 6:
            glRectf(0, 80, 80, 100);
            break;
    }
}

// 顯示頁面，畫出所有圖形
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // draw obstacle
    obstacle();

    // draw tank
    if(!loose)
        glColor3f(0, 1, 0);
    else
        glColor3f(0.5, 0.5, 0.5);

    glBegin(GL_POLYGON);
    if(dir == 1) { // right
        glVertex2f(squX + 30, squY + 10);
        glVertex2f(squX + 30, squY + 30);
        glVertex2f(squX - 30, squY + 30);
        glVertex2f(squX - 30, squY - 30);
        glVertex2f(squX + 30, squY - 30);
        glVertex2f(squX + 30, squY - 10);
        glVertex2f(squX + 50, squY - 10);
        glVertex2f(squX + 50, squY + 10);
    }
    if(dir == 2) { // left
        glVertex2f(squX - 30, squY + 10);
        glVertex2f(squX - 30, squY + 30);
        glVertex2f(squX + 30, squY + 30);
        glVertex2f(squX + 30, squY - 30);
        glVertex2f(squX - 30, squY - 30);
        glVertex2f(squX - 30, squY - 10);
        glVertex2f(squX - 50, squY - 10);
        glVertex2f(squX - 50, squY + 10);
    }
    if(dir == 3) { // up
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

    // draw bullets
    glColor3f(1, 0, 0);
    for(int i = 0; i < isShoot; i++) {
        if(bArray[i].live)
            renderSphere(bArray[i].bx, bArray[i].by, bSize, bSize, 100, gluNewQuadric());
    }

    // draw balls
    for(int i = 0; i <= sphNo; i++) {
        if(sArray[i].live) {
            if(sArray[i].type == 1) {
                glColor3f(0, 0, 1);
                renderSphere(sArray[i].bx, sArray[i].by, 1000, 1000, 200, gluNewQuadric());
            }
            if(sArray[i].type == 2) {
                glColor3f(1, 0, 1);
                renderSphere(sArray[i].bx, sArray[i].by, 800, 800, 200, gluNewQuadric());
            }
            if(sArray[i].type == 3) {
                glColor3f(1, 1, 0);
                renderSphere(sArray[i].bx, sArray[i].by, 600, 600, 200, gluNewQuadric());
            }
        }
    }

    // display score
    glColor3f(1, 1, 1);
    drawText("Score : ", 0, 480);
    char s[64];
    char m[64];
    printf(s, "%d", score);
    printf(m, "%d", mode);
    drawText(s, 80, 480);

    if(mode == 6) {
        drawText("Bullet Last:", 0, 450);
        char b[20];
        printf(b, "%d", bulletLast - pressTime);
        drawText(b, 120, 450);
    }

    // judge win or lose
    if(loose) {
        glColor3f(1, 1, 1);
        drawText("Game Over", 300, 250);
        drawText("Press S to restart", 300, 200);
    }
    if(win) {
        glColor3f(1, 1, 1);
        drawText("You Win", 300, 250);
        drawText("Press Enter to next", 300, 200);
    }
    if(mode == 7) {
        glColor3f(1, 1, 1);
        drawText("The End", 300, 250);
        drawText("Thank you for playing", 300, 200);
    } else {
        glColor3f(1, 1, 1);
        drawText("Stage: ", 700, 480);
        drawText(m, 780, 480);
    }

    glFlush();
}

// 初始化頁面，清空所有圖形
void myinit(void) {
    glViewport(0, 0, ww, wh);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (GLdouble)ww, 0.0, (GLdouble)wh, -1.0, 1.0);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();

    // set the first ball
    score = tmpScore;
    switch(mode) {
        case 1:
            sArray[sphNo].bx = 400;
            sArray[sphNo].by = 400;
            sArray[sphNo].live = true;
            sArray[sphNo].size = 45;
            sArray[sphNo].xStep = -5;
            sArray[sphNo].yStep = -5;
            sArray[sphNo].type = 1;
            break;
        case 2:
            sArray[sphNo].bx = 300;
            sArray[sphNo].by = 300;
            sArray[sphNo].live = true;
            sArray[sphNo].size = 45;
            sArray[sphNo].xStep = -5;
            sArray[sphNo].yStep = -5;
            sArray[sphNo].type = 1;
            sphNo++;
            sArray[sphNo].bx = 500;
            sArray[sphNo].by = 500;
            sArray[sphNo].live = true;
            sArray[sphNo].size = 45;
            sArray[sphNo].xStep = 5;
            sArray[sphNo].yStep = -5;
            sArray[sphNo].type = 1;
            break;
        case 3:
            squX = 400;
            sArray[sphNo].bx = 300;
            sArray[sphNo].by = 400;
            sArray[sphNo].live = true;
            sArray[sphNo].size = 45;
            sArray[sphNo].xStep = -5;
            sArray[sphNo].yStep = -5;
            sArray[sphNo].type = 1;
            sphNo++;

            sArray[sphNo].bx = 500;
            sArray[sphNo].by = 200;
            sArray[sphNo].live = true;
            sArray[sphNo].size = 45;
            sArray[sphNo].xStep = 5;
            sArray[sphNo].yStep = -5;
            sArray[sphNo].type = 1;
            sphNo++;

            sArray[sphNo].bx = 200;
            sArray[sphNo].by = 300;
            sArray[sphNo].live = true;
            sArray[sphNo].size = 35;
            sArray[sphNo].xStep = 5;
            sArray[sphNo].yStep = -8;
            sArray[sphNo].type = 2;
            sphNo++;

            sArray[sphNo].bx = 600;
            sArray[sphNo].by = 500;
            sArray[sphNo].live = true;
            sArray[sphNo].size = 35;
            sArray[sphNo].xStep = -5;
            sArray[sphNo].yStep = -8;
            sArray[sphNo].type = 2;
            break;
        case 4:
            sArray[sphNo].bx = 400;
            sArray[sphNo].by = 400;
            sArray[sphNo].live = true;
            sArray[sphNo].size = 45;
            sArray[sphNo].xStep = -5;
            sArray[sphNo].yStep = -5;
            sArray[sphNo].type = 1;
            break;
        case 5:
            sArray[sphNo].bx = 400;
            sArray[sphNo].by = 400;
            sArray[sphNo].live = true;
            sArray[sphNo].size = 45;
            sArray[sphNo].xStep = -5;
            sArray[sphNo].yStep = -5;
            sArray[sphNo].type = 1;
            break;
        case 6: // stage 6
            sArray[sphNo].bx = 400;
            sArray[sphNo].by = 400;
            sArray[sphNo].live = true;
            sArray[sphNo].size = 45;
            sArray[sphNo].xStep = -5;
            sArray[sphNo].yStep = -5;
            sArray[sphNo].type = 1;
            break;
    }
}

// 按鍵動作
void processKeys(unsigned char key, int x, int y) {
    if(key == 32 && !loose) { // 空白鍵
        if(mode == 6 && pressTime < 20) {
            bArray[isShoot].live = true;
            bArray[isShoot].dir = dir;
            if(bArray[isShoot].dir == 1) {
                bArray[isShoot].bx = squX + 60;
                bArray[isShoot].by = squY;
            }
            if(bArray[isShoot].dir == 2) {
                bArray[isShoot].bx = squX - 60;
                bArray[isShoot].by = squY;
            }
            if(bArray[isShoot].dir == 3) {
                bArray[isShoot].bx = squX;
                bArray[isShoot].by = squY + 60;
            }
            // add bullet
            isShoot++;
            pressTime++;
        }
        else if(mode == 6 && pressTime == 20) {
            for(int i = 0; i < isShoot; i++)
                bArray[i].live = false;
            for(int i = 0; i < sphNo; i++)
                sArray[i].live = false;
            isShoot = 0;
            sphNo = 0;
            squX = 30;
            squY = 30;
            dir = 1;
            win = false;
            loose = true;
            pressTime = 0;
            myinit();
        }
        else {
            bArray[isShoot].live = true;
            bArray[isShoot].dir = dir;
            if(bArray[isShoot].dir == 1) {
                bArray[isShoot].bx = squX + 200;
                bArray[isShoot].by = squY;
            }
            if(bArray[isShoot].dir == 2) {
                bArray[isShoot].bx = squX - 200;
                bArray[isShoot].by = squY;
            }
            if(bArray[isShoot].dir == 3) {
                bArray[isShoot].bx = squX;
                bArray[isShoot].by = squY + 60;
            }
            // add bullet
            isShoot++;
        }
    }
    if(key == 's' && loose) {
        for(int i = 0; i < isShoot; i++)
            bArray[i].live = false;
        for(int i = 0; i < sphNo; i++)
            sArray[i].live = false;
        isShoot = 0;
        sphNo = 0;
        squX = 30;
        squY = 30;
        dir = 1;
        win = false;
        loose = false;
        myinit();
    }
    if(key == 13 && win) { // Enter
        for(int i = 0; i < isShoot; i++)
            bArray[i].live = false;
        for(int i = 0; i < sphNo; i++)
            sArray[i].live = false;
        isShoot = 0;
        sphNo = 0;
        squX = 30;
        squY = 30;
        dir = 1;
        win = false;
        loose = false;
        mode++;
        tmpScore = score;
        myinit();
    }
    glutPostRedisplay();
}

// 特殊按鍵，上下左右
void SpecialKeys(int key, int x, int y) {
    if(!loose) {
        if(mode == 5) {
            if(key == GLUT_KEY_LEFT) {
                if(squX - 50 <= 0)
                    squX -= 0;
                else
                    squX -= 10.0f;
            }
            if(key == GLUT_KEY_RIGHT) {
                if(squX + 50 >= ww)
                    squX -= 0;
                else
                    squX += 10.0f;
            }
            dir = 3;
        }
        if(mode != 5) {
            if(key == GLUT_KEY_LEFT) {
                if(squX - 50 <= 0)
                    squX -= 0;
                else
                    squX -= 10.0f;
                dir = 2;
            }
            if(key == GLUT_KEY_RIGHT) {
                if(squX + 50 >= ww)
                    squX -= 0;
                else
                    squX += 10.0f;
                dir = 1;
            }
            if(key == GLUT_KEY_UP) {
                dir = 3;
            }
        }
    }
    glutPostRedisplay();
}

// hit big ball then plus two small balls
void hit(int type) {
    sphNo += 2;
    int i = sphNo / 2 - 1;
    if(type == 1) {
        score += 60;
        sArray[sphNo - 1].bx = sArray[i].bx - 50;
        sArray[sphNo - 1].by = sArray[i].by + 50;
        sArray[sphNo - 1].live = true;
        sArray[sphNo - 1].size = 35;
        sArray[sphNo - 1].xStep = -5;
        sArray[sphNo - 1].yStep = -8;
        sArray[sphNo - 1].type = type + 1;

        sArray[sphNo].bx = sArray[i].bx + 50;
        sArray[sphNo].by = sArray[i].by + 50;
        sArray[sphNo].live = true;
        sArray[sphNo].size = 35;
        sArray[sphNo].xStep = 5;
        sArray[sphNo].yStep = -8;
        sArray[sphNo].type = type + 1;
    }
    if(type == 2) {
        score += 30;
        sArray[sphNo - 1].bx = sArray[i].bx - 50;
        sArray[sphNo - 1].by = sArray[i].by + 50;
        sArray[sphNo - 1].live = true;
        sArray[sphNo - 1].size = 25;
        sArray[sphNo - 1].xStep = -8;
        sArray[sphNo - 1].yStep = -8;
        sArray[sphNo - 1].type = type + 1;

        sArray[sphNo].bx = sArray[i].bx + 50;
        sArray[sphNo].by = sArray[i].by + 50;
        sArray[sphNo].live = true;
        sArray[sphNo].size = 25;
        sArray[sphNo].xStep = 8;
        sArray[sphNo].yStep = -8;
        sArray[sphNo].type = type + 1;
    }
}

// 反彈
void bounce() {
    for(int i = 0; i <= sphNo; i++) {
        if(sArray[i].live) {
            // balls bounce wall
            if(sArray[i].bx > ww - sArray[i].size || sArray[i].bx < sArray[i].size)
                sArray[i].xStep = -sArray[i].xStep;
            if(sArray[i].by > wh - sArray[i].size || sArray[i].by < sArray[i].size)
                sArray[i].yStep = -sArray[i].yStep;
            if(sArray[i].bx > ww - sArray[i].size)
                sArray[i].bx = ww - sArray[i].size - 1;
            if(sArray[i].by > wh - sArray[i].size)
                sArray[i].by = wh - sArray[i].size - 1;

            // balls bounce obstacle
            switch(mode) {
                case 1:
                    if(sArray[i].bx < 80 + sArray[i].size && sArray[i].by < 100 + sArray[i].size) {
                        sArray[i].xStep = -sArray[i].xStep;
                        sArray[i].yStep = -sArray[i].yStep;
                    }
                case 2:
                    if(sArray[i].bx < 80 + sArray[i].size && sArray[i].by < 100 + sArray[i].size) {
                        sArray[i].xStep = -sArray[i].xStep;
                        sArray[i].yStep = -sArray[i].yStep;
                    }
                    if(sArray[i].bx + sArray[i].size > 720 + sArray[i].size && sArray[i].by < 100 + sArray[i].size) {
                        sArray[i].xStep = -sArray[i].xStep;
                        sArray[i].yStep = -sArray[i].yStep;
                    }
                    break;
                case 3:
                    if(sArray[i].bx + sArray[i].size > 320 + sArray[i].size && sArray[i].by < 100 + sArray[i].size &&
                       sArray[i].bx < 470 + sArray[i].size) {
                        sArray[i].xStep = -sArray[i].xStep;
                        sArray[i].yStep = -sArray[i].yStep;
                    }
                    break;
                case 4:
                    if((sArray[i].bx > obsX + sArray[i].size && sArray[i].bx < obsX + 150 + sArray[i].size) &&
                       (sArray[i].by < 240 + sArray[i].size && sArray[i].by > 200 + sArray[i].size)) {
                        sArray[i].xStep = -sArray[i].xStep;
                        sArray[i].yStep = -sArray[i].yStep;
                    }
                    break;
                case 5:
                    if(sArray[i].bx < 80 + sArray[i].size && sArray[i].by < 100 + sArray[i].size) {
                        sArray[i].xStep = -sArray[i].xStep;
                        sArray[i].yStep = -sArray[i].yStep;
                    }
                case 6:
                    if(sArray[i].bx < 80 + sArray[i].size && sArray[i].by < 100 + sArray[i].size) {
                        sArray[i].xStep = -sArray[i].xStep;
                        sArray[i].yStep = -sArray[i].yStep;
                    }
            }

            // balls bounce other balls
            for(int j = 0; j <= i; j++) {
                if(i == j) continue; // except self bounce
                else {
                    if(sArray[j].live) {
                        float dx = fabs(sArray[i].bx - sArray[j].bx);
                        float dy = fabs(sArray[i].by - sArray[j].by);
                        float distance = sqrt(dx * dx + dy * dy);
                        if(distance <= sArray[j].size + sArray[i].size) {
                            sArray[i].xStep = -sArray[i].xStep;
                            sArray[i].yStep = -sArray[i].yStep;
                            sArray[j].xStep = -sArray[j].xStep;
                            sArray[j].yStep = -sArray[j].yStep;
                        }
                    }
                }
            }
            // balls hit user's tank
            if(sArray[i].bx <= squX + 30 && sArray[i].by <= squY + 30 &&
               sArray[i].bx >= squX - 30 && sArray[i].by >= squY - 30) {
                loose = true;
            }
            // balls move
            sArray[i].bx += sArray[i].xStep;
            sArray[i].by += sArray[i].yStep;
            // obstacle move
            obsX += A;
            if(obsX > 1050) {
                A = -10;
            } else if(obsX < 0) {
                A = 10;
            }
        }
    }
}

// 時間函數
void TimerFunction(int value) {
    // shoot bullets
    for(int i = 0; i < isShoot; i++) {
        if(bArray[i].live) {
            if(bArray[i].dir == 1) {
                bArray[i].bx += 5;
                if(bArray[i].bx > ww - 10)
                    bArray[i].live = false;
            }
            if(bArray[i].dir == 2) {
                if(bArray[i].live)
                    bArray[i].bx -= 5;
                if(bArray[i].bx < 10)
                    bArray[i].live = false;
            }
            if(bArray[i].dir == 3) {
                if(bArray[i].live)
                    bArray[i].by += 5;
                if(bArray[i].by > wh - 10)
                    bArray[i].live = false;

                switch(mode) {
                    case 1:
                        if(bArray[i].bx <= 80 && bArray[i].by >= 80)
                            bArray[i].live = false;
                        break;
                    case 2:
                        if((bArray[i].bx <= 80 || bArray[i].bx >= 720) && bArray[i].by >= 80)
                            bArray[i].live = false;
                        break;
                    case 3:
                        if(bArray[i].bx <= 460 && bArray[i].by >= 80 &&
                           bArray[i].bx >= 320)
                            bArray[i].live = false;
                        break;
                }
            }

            // bullet hit the ball
            for(int j = 0; j <= sphNo; j++) {
                if(sArray[j].live) {
                    float dx = fabs(bArray[i].bx - sArray[j].bx);
                    float dy = fabs(bArray[i].by - sArray[j].by);
                    float distance = sqrt(dx * dx + dy * dy);
                    if(distance < sArray[j].size) {
                        sArray[j].live = false;
                        bArray[i].live = false;
                        // only big and medium ball can plus two smaller ball
                        if(sArray[j].type < 3)
                            hit(sArray[j].type);
                        else
                            score += 10;
                    }
                }
            }
        }
    }

    // judge win
    int winNo = 0;
    for(int i = 0; i <= sphNo; i++)
        if(!sArray[i].live)
            winNo++;
    if((winNo == 7 && mode == 1) || (winNo == 14 && mode == 2) || (winNo == 20 && mode == 3))
        win = true;

    // bounce wall or obstacle
    bounce();

    glutPostRedisplay();
    glutTimerFunc(25, TimerFunction, 1);
}

// 建立 windows
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(ww, wh);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("test");
    myinit();

    glutSpecialFunc(SpecialKeys);
    glutKeyboardFunc(processKeys);
    glutDisplayFunc(display);
    glutTimerFunc(25, TimerFunction, 1);
    glutMainLoop();
    return 0;
}


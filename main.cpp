#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

int xSelect=1, ySelect=0, zSelect=0; //za semi-transparentnata ramnina
int n=3; //brojot na kockicki na kockata
int planePos=0; //kade se naogja ramninata za sekoja od oskite x,y i z od 0 do numCubs-1
float **rotAngles; //dali ke se rotira za +90 ili -90
int **rotSelection; //ke se rotira samo ako e selektirana
int xMin=-10, xMax=10, yMin=-10, yMax=10, zMin=-10, zMax=10; //rangot na koordinatniot sistem
float cubesSize=1.5, minDistance=0.1, maxDistance=0.5, currentDistance=0.3, distanceStep=0.01; //golemina na kocki i rastojanie
float rubikSize = n*cubesSize+(n-1)*currentDistance; //golemina na rubikova kocka
float planeSize = rubikSize*1.2; //golemina na ramnina
float yAngle = 0.0, xAngle = 0.0; //rotacija na kamera
float zoomIn=1.0; //priblizuvanje po z
GLfloat white[3] = {1.0f, 1.0f, 1.0f}; //bela boja LEVA
GLfloat red[3] = {1.0f, 0.0f, 0.0f}; //crvena boja DESNA
GLfloat yellow[3] = {1.0f, 1.0f, 0.0f}; //zolta boja DOLNA
GLfloat blue[3] = {0.0f, 0.0f, 1.0f}; //sina boja GORNA
GLfloat green[3] = {0.0f, 1.0f, 0.0f}; //zelena boja PREDNA
GLfloat orange[3] = {1.0f, 0.5f, 0.0f}; //portokalova boja ZADNA
float rotateSpeed=0.0f; //brzina na rotacija na segment


struct Kocka {
    GLfloat left[3];
    GLfloat right[3];
    GLfloat down[3];
    GLfloat up[3];
    GLfloat near[3];
    GLfloat far[3];
};
Kocka ***rubikCube; //ova e rubikovata kocka

//inicijalizacija na rotaciite na segmentite i goleminata na rubikovata kocka
void initAll() {
    xSelect=ySelect=zSelect=0;
    xSelect=1;
    planePos=0;
    rubikSize = n*cubesSize+(n-1)*currentDistance;
    planeSize = rubikSize*1.2;
    rotAngles = new float *[3];
    for (int i=0; i<3; i++) {
        rotAngles[i] = new float[n];
    }
    for (int i=0; i<3; i++) {
        for (int j=0; j<n; j++) {
            rotAngles[i][j]=0;
        }
    }
    rotSelection = new int *[3];
    for (int i=0; i<3; i++) {
        rotSelection[i] = new int[n];
    }
    for (int i=0; i<3; i++) {
        for (int j=0; j<n; j++) {
            rotSelection[i][j]=0;
        }
    }
    rubikCube = new Kocka **[n];
    for (int i=0; i<n; i++) {
        rubikCube[i] = new Kocka *[n];
        for (int j=0; j<n; j++) {
            rubikCube[i][j] = new Kocka[n];
        }
    }
}
//ovaa funckija ke se povikuva pri pritiskanje na < ili >
void selectRot(int plane, int pos) {
    for (int i=0; i<n; i++) {
        if (i==pos) {
            rotSelection[plane][i]=1;
        }
        else {
            rotSelection[plane][i]=0;
        }
    }
}
//ovaa funkcija ke se povikuva samo pri menuvanje na brojot na kockicki
Kocka initCube(int xIndex, int yIndex, int zIndex) {
    Kocka temp;
    //leva strana
    if (xIndex==0) {
        for (int i=0; i<3; i++)
            temp.left[i]=white[i];
    }
    else {
        temp.left[0]=0.0f; temp.left[1]=0.0f; temp.left[2]=0.0f;
    }
    //desna strana
    if (xIndex==(n-1)) {
        for (int i=0; i<3; i++)
            temp.right[i]=red[i];
    }
    else {
        temp.right[0]=0.0f; temp.right[1]=0.0f; temp.right[2]=0.0f;
    }
    //dolna strana
    if (yIndex==0) {
        for (int i=0; i<3; i++)
            temp.down[i]=yellow[i];
    }
    else {
        temp.down[0]=0.0f; temp.down[1]=0.0f; temp.down[2]=0.0f;
    }
    //gorna strana
    if (yIndex==(n-1)) {
        for (int i=0; i<3; i++)
            temp.up[i]=blue[i];
    }
    else {
        temp.up[0]=0.0f; temp.up[1]=0.0f; temp.up[2]=0.0f;
    }
    //predna strana
    if (zIndex==0) {
        for (int i=0; i<3; i++)
            temp.near[i]=green[i];
    }
    else {
        temp.near[0]=0.0f; temp.near[1]=0.0f; temp.near[2]=0.0f;
    }
    //zadna strana
    if (zIndex==(n-1)) {
        for (int i=0; i<3; i++)
            temp.far[i]=orange[i];
    }
    else {
        temp.far[0]=0.0f; temp.far[1]=0.0f; temp.far[2]=0.0f;
    }
    //
    return temp;
}

//ovaa funkcija ke se koristi samo pri promena na brojot na kockicki vo kockata
void initRubikCube() {
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            for (int k=0; k<n; k++) {
                rubikCube[i][j][k]=initCube(i, j, k);
            }
        }
    }
}

//ovaa funkcija ke se koristi za crtanje na sekoj od segmentite
void drawCube(Kocka c) {
    float xmn=-cubesSize/2.0, xmx=cubesSize/2.0;
    float ymn=-cubesSize/2.0, ymx=cubesSize/2.0;
    float zmn=-cubesSize/2.0, zmx=cubesSize/2.0;
    glPushMatrix();
    //
    glBegin(GL_QUADS);
    //levo
    glColor3fv(c.left);
    glVertex3f(xmn, ymn, zmx);
    glVertex3f(xmn, ymn, zmn);
    glVertex3f(xmn, ymx, zmn);
    glVertex3f(xmn, ymx, zmx);
    //desno
    glColor3fv(c.right);
    glVertex3f(xmx, ymn, zmx);
    glVertex3f(xmx, ymn, zmn);
    glVertex3f(xmx, ymx, zmn);
    glVertex3f(xmx, ymx, zmx);
    //dole
    glColor3fv(c.down);
    glVertex3f(xmn, ymn, zmx);
    glVertex3f(xmn, ymn, zmn);
    glVertex3f(xmx, ymn, zmn);
    glVertex3f(xmx, ymn, zmx);
    //gore
    glColor3fv(c.up);
    glVertex3f(xmn, ymx, zmx);
    glVertex3f(xmn, ymx, zmn);
    glVertex3f(xmx, ymx, zmn);
    glVertex3f(xmx, ymx, zmx);
    //napred
    glColor3fv(c.near);
    glVertex3f(xmn, ymn, zmn);
    glVertex3f(xmx, ymn, zmn);
    glVertex3f(xmx, ymx, zmn);
    glVertex3f(xmn, ymx, zmn);
    //nazad
    glColor3fv(c.far);
    glVertex3f(xmn, ymn, zmx);
    glVertex3f(xmx, ymn, zmx);
    glVertex3f(xmx, ymx, zmx);
    glVertex3f(xmn, ymx, zmx);
    glEnd();
    //
    glPopMatrix();

}
//ovaa funkcija ke se koristi vo display za zamena na boite posle rotacijata
void rotateByXPositive(int x) {
    for (int i=0; i<ceil(n/2); i++) {
        //od napred kon gore
        int j = i;
        float tempUp[(n-i)-j][3];
        float tempFar[(n-i)-j][3];
        float tempDown[(n-i)-j][3];
        float tempNear[(n-i)-j][3];
        while (j < (n-i)) {
            for (int k=0; k<3; k++) {
                tempUp[j][k]=rubikCube[x][n-i-1][j].up[k];
                tempFar[j][k]=rubikCube[x][n-i-1][j].far[k];
                tempDown[j][k]=rubikCube[x][n-i-1][j].down[k];
                tempNear[j][k]=rubikCube[x][n-i-1][j].near[k];
                rubikCube[x][n-i-1][j].up[k]=rubikCube[x][j][i].near[k];
                rubikCube[x][n-i-1][j].far[k]=rubikCube[x][j][i].up[k];
                rubikCube[x][n-i-1][j].down[k]=rubikCube[x][j][i].far[k];
                rubikCube[x][n-i-1][j].near[k]=rubikCube[x][j][i].down[k];

            }
            j++;
        }
        //od gore kon nazad
        j=i;
        float tempUp2[(n-i)-j][3];
        float tempFar2[(n-i)-j][3];
        float tempDown2[(n-i)-j][3];
        float tempNear2[(n-i)-j][3];
        while (j < (n-i)) {
            for (int k=0; k<3; k++) {
                tempUp2[j][k]=rubikCube[x][n-i-j-1][n-i-1].up[k];
                tempFar2[j][k]=rubikCube[x][n-i-j-1][n-i-1].far[k];
                tempDown2[j][k]=rubikCube[x][n-i-j-1][n-i-1].down[k];
                tempNear2[j][k]=rubikCube[x][n-i-j-1][n-i-1].near[k];
                rubikCube[x][n-i-j-1][n-i-1].up[k]=tempNear[j][k];
                rubikCube[x][n-i-j-1][n-i-1].far[k]=tempUp[j][k];
                rubikCube[x][n-i-j-1][n-i-1].down[k]=tempFar[j][k];
                rubikCube[x][n-i-j-1][n-i-1].near[k]=tempDown[j][k];
            }
            j++;
        }
        j=i;
        float tempUp3[(n-i)-j][3];
        float tempFar3[(n-i)-j][3];
        float tempDown3[(n-i)-j][3];
        float tempNear3[(n-i)-j][3];
        while (j < (n-i)) {
            for (int k=0; k<3; k++) {
                tempUp3[j][k]=rubikCube[x][i][n-i-j-1].up[k];
                tempFar3[j][k]=rubikCube[x][i][n-i-j-1].far[k];
                tempDown3[j][k]=rubikCube[x][i][n-i-j-1].down[k];
                tempNear3[j][k]=rubikCube[x][i][n-i-j-1].near[k];
                rubikCube[x][i][n-i-j-1].up[k]=tempNear2[j][k];
                rubikCube[x][i][n-i-j-1].far[k]=tempUp2[j][k];
                rubikCube[x][i][n-i-j-1].down[k]=tempFar2[j][k];
                rubikCube[x][i][n-i-j-1].near[k]=tempDown2[j][k];
            }
            j++;
        }
        j=i;
        float tempUp4[(n-i)-j][3];
        float tempFar4[(n-i)-j][3];
        float tempDown4[(n-i)-j][3];
        float tempNear4[(n-i)-j][3];
        while (j < (n-i)) {
            for (int k=0; k<3; k++) {
                tempUp4[j][k]=rubikCube[x][j][i].up[k];
                tempFar4[j][k]=rubikCube[x][j][i].far[k];
                tempDown4[j][k]=rubikCube[x][j][i].down[k];
                tempNear4[j][k]=rubikCube[x][j][i].near[k];
                rubikCube[x][j][i].up[k]=tempNear3[j][k];
                rubikCube[x][j][i].far[k]=tempUp3[j][k];
                rubikCube[x][j][i].down[k]=tempFar3[j][k];
                rubikCube[x][j][i].near[k]=tempDown3[j][k];
            }
            j++;
        }
    }
}

//funkcija koja ke iscrtuva segment po x
void segmentByX(int x) {
    int a=0;
    for (float i=-rubikSize/2.0; i<(rubikSize/2.0); i=i+cubesSize+currentDistance) {
        int b=0;
        glPushMatrix();
        glTranslatef(0, i+cubesSize/2.0, 0);
        for (float j=-rubikSize/2.0; j<(rubikSize/2.0); j=j+cubesSize+currentDistance) {
            glPushMatrix();
            glTranslatef(0, 0, j+cubesSize/2.0);
            drawCube(rubikCube[x][a][b]);
            glPopMatrix();
            b++;
        }
        glPopMatrix();
        a++;
    }
}
//funkcija koja ke iscrtuva segment po y
void segmentByY(int y) {
    int a=0;
    for (float i=-rubikSize/2.0; i<(rubikSize/2.0); i=i+cubesSize+currentDistance) {
        int b=0;
        glPushMatrix();
        glTranslatef(i+cubesSize/2.0, 0, 0);
        for (float j=-rubikSize/2.0; j<(rubikSize/2.0); j=j+cubesSize+currentDistance) {
            glPushMatrix();
            glTranslatef(0, 0, j+cubesSize/2.0);
            drawCube(rubikCube[a][y][b]);
            glPopMatrix();
            b++;
        }
        glPopMatrix();
        a++;
    }
}
//funckija koja ke iscrtuva segment po z
void segmentByZ(int z) {
    int a=0;
    for (float i=-rubikSize/2.0; i<(rubikSize/2.0); i=i+cubesSize+currentDistance) {
        int b=0;
        glPushMatrix();
        glTranslatef(i+cubesSize/2.0, 0, 0);
        for (float j=-rubikSize/2.0; j<(rubikSize/2.0); j=j+cubesSize+currentDistance) {
            glPushMatrix();
            glTranslatef(0, j+cubesSize/2.0, 0);
            drawCube(rubikCube[a][b][z]);
            glPopMatrix();
            b++;
        }
        glPopMatrix();
        a++;
    }
}
//crtanje na ramninata po X
void drawPlaneByX() {
    float planeMin=-planeSize/2.0; float planeMax=planeSize/2.0;
    glPushMatrix();
    //
    glColor4f(0.8f, 0.8f, 0.8f, 0.5f);
    glBegin(GL_QUADS);
    glVertex3f(0, planeMin, planeMax);
    glVertex3f(0, planeMin, planeMin);
    glVertex3f(0, planeMax, planeMin);
    glVertex3f(0, planeMax, planeMax);
    glEnd();
    //
    glPopMatrix();
}
//crtanje na ramninata po Y
void drawPlaneByY() {
    float planeMin=-planeSize/2.0; float planeMax=planeSize/2.0;
    glPushMatrix();
    //
    glColor4f(0.8f, 0.8f, 0.8f, 0.5f);
    glBegin(GL_QUADS);
    glVertex3f(planeMin, 0, planeMax);
    glVertex3f(planeMin, 0, planeMin);
    glVertex3f(planeMax, 0, planeMin);
    glVertex3f(planeMax, 0, planeMax);
    glEnd();
    //
    glPopMatrix();
}
//crtanje na ramninata po Z
void drawPlaneByZ() {
    float planeMin=-planeSize/2.0; float planeMax=planeSize/2.0;
    glPushMatrix();
    //
    glColor4f(0.8f, 0.8f, 0.8f, 0.5f);
    glBegin(GL_QUADS);
    glVertex3f(planeMin, planeMin, 0);
    glVertex3f(planeMax, planeMin, 0);
    glVertex3f(planeMax, planeMax, 0);
    glVertex3f(planeMin, planeMax, 0);
    glEnd();
    //
    glPopMatrix();
}
//ovaa funkcija ke ja iscrtuva kockata i ramninata no po x ramnina
void drawRubikByX() {
    float xmin = -rubikSize/2.0; float xmax = rubikSize/2.0;
    int i=0;
    for (float k=xmin; k<xmax; k=k+cubesSize+currentDistance) {
        glPushMatrix();
        if (rotSelection[0][i]==1) {
            if (rotAngles[0][i]==90.0f || rotAngles[0][i]==-90.0f) {
                rotSelection[0][i]=0;
                rotAngles[0][i]=0.0f;
                rotateSpeed=0.0f;
            }
            else {
                glRotatef(rotAngles[0][i], 1, 0, 0);
                rotAngles[0][i]+=rotateSpeed;
                glutPostRedisplay();
            }
        }
        glTranslatef(k+cubesSize/2.0, 0, 0);
        if (i==planePos) {
            drawPlaneByX();
        }
        segmentByX(i);
        glPopMatrix();
        i++;
    }
}
//ovaa funkcija ke ja iscrtuva kockata i ramninata no po y ramnina
void drawRubikByY() {
    float ymin = -rubikSize/2.0; float ymax = rubikSize/2.0;
    int i=0;
    for (float k=ymin; k<ymax; k=k+cubesSize+currentDistance) {
        glPushMatrix();
        if (rotSelection[1][i]==1) {
            if (rotAngles[1][i]==90.0f || rotAngles[1][i]==-90.0f) {
                rotSelection[1][i]=0;
                rotAngles[1][i]=0.0f;
                rotateSpeed=0.0f;
            }
            else {
                glRotatef(rotAngles[1][i], 0, 1, 0);
                rotAngles[1][i]+=rotateSpeed;
                glutPostRedisplay();
            }
        }
        glTranslatef(0, k+cubesSize/2.0, 0);
        if (i==planePos) {
            drawPlaneByY();
        }
        segmentByY(i);
        glPopMatrix();
        i++;
    }
}
//ovaa funkcija ke ja iscrtuva kockata i ramninata no po z ramnina
void drawRubikByZ() {
    float zmin = -rubikSize/2.0; float zmax = rubikSize/2.0;
    int i=0;
    for (float k=zmin; k<zmax; k=k+cubesSize+currentDistance) {
        glPushMatrix();
        if (rotSelection[2][i]==1) {
            if (rotAngles[2][i]==-90.0f || rotAngles[2][i]==90.0f) {
                rotSelection[2][i]=0;
                rotAngles[2][i]=0.0f;
                rotateSpeed=0.0f;
            }
            else {
                glRotatef(rotAngles[2][i], 0, 0, 1);
                rotAngles[2][i]+=rotateSpeed;
                glutPostRedisplay();
            }
        }
        glTranslatef(0, 0, k+cubesSize/2.0);
        if (i==planePos) {
            drawPlaneByZ();
        }
        segmentByZ(i);
        glPopMatrix();
        i++;
    }
}
//iscrtuvanje na celata kocka, ovaa funkcija ke se povikuva vo display
void drawRubikCube() {
    if (xSelect==1) {
        drawRubikByX();
    }
    else if (ySelect==1) {
        drawRubikByY();
    }
    else if (zSelect==1) {
        drawRubikByZ();
    }
    else {

    }
}
//obicni kopcinja
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27:
        exit(0);
        break;
    case '3':
        n=3;
        initAll();
        initRubikCube();
        glutPostRedisplay();
        break;
    case '4':
        n=4;
        initAll();
        initRubikCube();
        glutPostRedisplay();
        break;
    case '5':
        n=5;
        initAll();
        initRubikCube();
        glutPostRedisplay();
        break;
    case '6':
        n=6;
        initAll();
        initRubikCube();
        glutPostRedisplay();
        break;
    case '7':
        n=7;
        initAll();
        initRubikCube();
        glutPostRedisplay();
        break;
    case '8':
        n=8;
        initAll();
        initRubikCube();
        glutPostRedisplay();
        break;
    case '9':
        n=9;
        initAll();
        initRubikCube();
        glutPostRedisplay();
        break;
    case 'x':
        xSelect=1;
        ySelect=0;
        zSelect=0;
        planePos=0;
        glutPostRedisplay();
        break;
    case 'y':
        xSelect=0;
        ySelect=1;
        zSelect=0;
        planePos=0;
        glutPostRedisplay();
        break;
    case 'z':
        xSelect=0;
        ySelect=0;
        zSelect=1;
        planePos=0;
        glutPostRedisplay();
        break;
    case '/':
        planePos = (planePos+1)%n;
        glutPostRedisplay();
        break;
    case '<':
        if (xSelect==1) {
            selectRot(0, planePos);
            rotateSpeed=0.3f;
        }
        else if (ySelect==1) {
            selectRot(1, planePos);
            rotateSpeed=0.3f;
        }
        else if (zSelect==1) {
            selectRot(2, planePos);
            rotateSpeed=0.3f;
        }
        else {
        }
        glutPostRedisplay();
        break;
    case '>':
        if (xSelect==1) {
            selectRot(0, planePos);
            rotateSpeed=-0.3f;
        }
        else if (ySelect==1) {
            selectRot(1, planePos);
            rotateSpeed=-0.3f;
        }
        else if (zSelect==1) {
            selectRot(2, planePos);
            rotateSpeed=-0.3f;
        }
        else {
        }
        glutPostRedisplay();
        break;
    case '+':
        currentDistance+=distanceStep;
        if (currentDistance>maxDistance) {
            currentDistance=maxDistance;
        }
        glutPostRedisplay();
        break;
    case '-':
        currentDistance-=distanceStep;
        if (currentDistance<minDistance) {
            currentDistance=minDistance;
        }
        glutPostRedisplay();
        break;
    }
}
void specialKeyboard(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_PAGE_UP:
        zoomIn+=0.1f;
        if (zoomIn>3) {
            zoomIn=3;
        }
        glutPostRedisplay();
        break;
    case GLUT_KEY_PAGE_DOWN:
        zoomIn-=0.1f;
        if (zoomIn<0.1) {
            zoomIn=0.1;
        }
        glutPostRedisplay();
        break;
    case GLUT_KEY_UP:
        xAngle++;
        if (xAngle > 360) {
            xAngle=0;
        }
        glutPostRedisplay();
        break;
    case GLUT_KEY_DOWN:
        xAngle--;
        if (xAngle < -360) {
            xAngle=0;
        }
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT:
        yAngle++;
        if (yAngle > 360) {
            yAngle=0;
        }
        glutPostRedisplay();
        break;
    case GLUT_KEY_LEFT:
        yAngle--;
        if (yAngle < -360) {
            yAngle=0;
        }
        glutPostRedisplay();
        break;
    }

}
void init() {
    glClearColor(0.7f, 0.9f, 0.9f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(xMin, xMax, yMin, yMax, zMin, zMax);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
}
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1/10.0, 1/10.0, 1/10.0); //ova ke bide goleminata na celiot prostor za modeliranje
    glPushMatrix();
    glScalef(zoomIn, zoomIn, zoomIn);
    glRotatef(xAngle, 1, 0, 0);
    glRotatef(yAngle, 0, 1, 0);
    drawRubikCube();
    glPopMatrix();
    glutSwapBuffers();
    glFlush();
}
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(600,600);
    glutCreateWindow("Rubik_Cube");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutMainLoop();
    return 0;
}







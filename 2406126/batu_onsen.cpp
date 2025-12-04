#include <windows.h>
#include <GL/freeglut.h>
#include <cmath>
#include <iostream>

const float PI = 3.14159265f;
const float MOVE_SPEED = 0.05f;
const float MOUSE_SENSITIVITY = 0.1f;

float cameraPosX = 0.0f, cameraPosY = 4.0f, cameraPosZ = 28.0f;
float cameraYaw = 0.0f, cameraPitch = 0.0f;
bool mouseActive = false;
bool keys[256] = {false};
int windowWidth = 1200, windowHeight = 800;

// ---------- MULTI LIGHT ----------
#define NUM_LIGHTS 6
struct Light {
    bool enabled;
    GLfloat position[4];
    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat specular[4];
    GLfloat spotDirection[3];
    float spotCutoff;
    float spotExponent;
};
Light lights[NUM_LIGHTS];
bool lightingEnabled = true;
bool isShiny = true;

// ---------- BATU-BATU ONSEN ----------
#define MAX_ROCKS 50
float rockX[MAX_ROCKS], rockY[MAX_ROCKS], rockZ[MAX_ROCKS];
float rockScale[MAX_ROCKS], rockRot[MAX_ROCKS];
int totalRocks = 50;

// ---------- INIT LIGHTING ----------
void initLighting() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    GLfloat mat_spec[] = {1.0f,1.0f,1.0f,1.0f};
    GLfloat mat_shin[] = {80.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shin);

    // Lampu 0: Matahari (directional)
    lights[0] = {true,
        {10.0f, 18.0f, -12.0f, 0.0f},
        {0.35f,0.30f,0.25f,1.0f},
        {0.9f,0.8f,0.7f,1.0f},
        {1.0f,0.9f,0.8f,1.0f},
        {0,0,0}, 180.0f, 0.0f};

    // Lampu 1-5: di pinggir kolam 
    float angles[5] = {20, 100, 180, 260, 340};
    for(int i = 1; i < NUM_LIGHTS; i++) {
        float ang = angles[i-1] * PI / 180.0f;
        float x = cosf(ang) * 17.5f;
        float z = sinf(ang) * 12.5f;
        lights[i] = {true,
            {x, 1.6f, z, 1.0f},
            {0.08f,0.06f,0.04f,1.0f},
            {0.95f,0.75f,0.45f,1.0f},
            {1.0f,0.9f,0.6f,1.0f},
            {0.0f, -1.0f, 0.0f}, 52.0f, 18.0f};
    }
}

void updateLightingLogic() {
    if (!lightingEnabled) {
        for(int i = 0; i < 8; i++) glDisable(GL_LIGHT0 + i);
        return;
    }
    for(int i = 0; i < NUM_LIGHTS; i++) {
        if (!lights[i].enabled) { glDisable(GL_LIGHT0 + i); continue; }
        glEnable(GL_LIGHT0 + i);
        glLightfv(GL_LIGHT0 + i, GL_POSITION, lights[i].position);
        glLightfv(GL_LIGHT0 + i, GL_AMBIENT,  lights[i].ambient);
        glLightfv(GL_LIGHT0 + i, GL_DIFFUSE,  lights[i].diffuse);
        glLightfv(GL_LIGHT0 + i, GL_SPECULAR, lights[i].specular);
        if (lights[i].spotCutoff < 180.0f) {
            glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, lights[i].spotDirection);
            glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, lights[i].spotCutoff);
            glLightf(GL_LIGHT0 + i, GL_SPOT_EXPONENT, lights[i].spotExponent);
        } else {
            glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 180.0f);
        }
        glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, 0.09f);
        glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, 0.03f);
    }
    GLfloat s[4] = {isShiny?1.0f:0.0f, isShiny?1.0f:0.0f, isShiny?1.0f:0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, s);
}
// ---------- BATU ONSEN ----------
void initRocks() {
    srand(1234);
    float a = 15.5f;
    float b = 10.8f;
    float offset = 2.1f;

    for(int i = 0; i < totalRocks; i++) {
        float t = i * 2*PI / totalRocks + (rand()%100)/800.0f;
        float r_extra = offset + (rand()%100)/100.0f * 1.5f;
        float x = (a + r_extra) * cosf(t);
        float z = (b + r_extra) * sinf(t);

        rockX[i] = x;
        rockZ[i] = z;
        rockY[i] = 0.5f - 0.18f + (rand()%80)/600.0f;
        rockScale[i] = 0.9f + (rand()%100)/100.0f * 2.3f;
        rockRot[i] = rand() % 360;
    }
}

void drawRocks() {
    glColor3f(0.35f, 0.35f, 0.38f);
    for(int i = 0; i < totalRocks; i++) {
        glPushMatrix();
        glTranslatef(rockX[i], rockY[i], rockZ[i]);
        glRotatef(rockRot[i], 1, 1, 0.3f);
        glRotatef(35, 0, 1, 0);
        glScalef(rockScale[i], rockScale[i]*0.75f, rockScale[i]);
        glutSolidDodecahedron();
        glPopMatrix();
    }
}

void drawOnsenPool() {
    float a = 15.5f;   // panjang sumbu X (lebar kolam)
    float b = 10.8f;   // panjang sumbu Z (lebih pendek ? lonjong)
    float edgeHeight = 0.5f;
    int seg = 96;

    // Air di oval
    glColor3f(0.10f, 0.30f, 0.48f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < seg; i++) {
        float t = i * 2*PI / seg;
        glVertex3f(a * cosf(t) * 0.93f, 0.06f, b * sinf(t) * 0.93f);
    }
    glEnd();

    //sisi luar oval
    glColor3f(0.28f, 0.24f, 0.18f);
    glBegin(GL_QUAD_STRIP);
    for(int i = 0; i <= seg; i++) {
        float t = i * 2*PI / seg;
        float x = a * cosf(t);
        float z = b * sinf(t);
        glVertex3f(x, 0.0f, z);
        glVertex3f(x, edgeHeight, z);
    }
    glEnd();

    // Dinding dalam oval
    glColor3f(0.18f, 0.15f, 0.12f);
    glBegin(GL_QUAD_STRIP);
    for(int i = 0; i <= seg; i++) {
        float t = i * 2*PI / seg;
        float x = a * cosf(t) * 0.93f;
        float z = b * sinf(t) * 0.93f;
        glVertex3f(x, 0.06f, z);
        glVertex3f(x, 0.06f - 0.48f, z);
    }
    glEnd();
}

// tanah
void drawGround() {
    glColor3f(0.28f, 0.24f, 0.18f);
    glBegin(GL_QUADS);
    glVertex3f(-60, -0.01f, -60);
    glVertex3f( 60, -0.01f, -60);
    glVertex3f( 60,  -0.01f, 60);
    glVertex3f(-60, -0.01f, 60);
    glEnd();
}

void drawLightSources() {
    glDisable(GL_LIGHTING);
    for(int i = 1; i < NUM_LIGHTS; i++) {
        glPushMatrix();
        glTranslatef(lights[i].position[0], lights[i].position[1], lights[i].position[2]);
        glColor3f(1.0f, 0.8f, 0.4f);
        glutSolidSphere(0.4, 16, 16);
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);
}

// ---------- KAMERA sama DISPLAY ----------
void updateCamera() {
    float yawRad = cameraYaw * PI / 180.0f;
    float pitchRad = cameraPitch * PI / 180.0f;
    float fx = sin(yawRad) * cos(pitchRad);
    float fy = -sin(pitchRad);
    float fz = -cos(yawRad) * cos(pitchRad);
    float rx = sin(yawRad + PI/2);
    float rz = -cos(yawRad + PI/2);

    if (keys['w'] || keys['W']) { cameraPosX += fx*MOVE_SPEED; cameraPosY += fy*MOVE_SPEED; cameraPosZ += fz*MOVE_SPEED; }
    if (keys['s'] || keys['S']) { cameraPosX -= fx*MOVE_SPEED; cameraPosY -= fy*MOVE_SPEED; cameraPosZ -= fz*MOVE_SPEED; }
    if (keys['a'] || keys['A']) { cameraPosX -= rx*MOVE_SPEED; cameraPosZ -= rz*MOVE_SPEED; }
    if (keys['d'] || keys['D']) { cameraPosX += rx*MOVE_SPEED; cameraPosZ += rz*MOVE_SPEED; }
    if (keys[' ']) cameraPosY += MOVE_SPEED;
    if (keys['c'] || keys['C']) cameraPosY -= MOVE_SPEED;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float yawRad = cameraYaw * PI / 180.0f;
    float pitchRad = cameraPitch * PI / 180.0f;
    float lookX = cameraPosX + sin(yawRad) * cos(pitchRad);
    float lookY = cameraPosY - sin(pitchRad);
    float lookZ = cameraPosZ - cos(yawRad) * cos(pitchRad);
    gluLookAt(cameraPosX, cameraPosY, cameraPosZ, lookX, lookY, lookZ, 0, 1, 0);

    updateLightingLogic();
    drawGround();
    drawRocks();
    drawLightSources();
    drawOnsenPool();
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    keys[key] = true;
    if (key == 27) exit(0);
    if (key == 'l' || key == 'L') lightingEnabled = !lightingEnabled;
    if (key == 'i' || key == 'I') isShiny = !isShiny;
}
void keyboardUp(unsigned char key, int x, int y) { keys[key] = false; }

void mouseMotion(int x, int y) {
    if (!mouseActive) return;
    int cx = windowWidth/2, cy = windowHeight/2;
    cameraYaw += (x - cx) * MOUSE_SENSITIVITY;
    cameraPitch += (y - cy) * MOUSE_SENSITIVITY;
    if (cameraPitch > 89.0f) cameraPitch = 89.0f;
    if (cameraPitch < -89.0f) cameraPitch = -89.0f;
    glutWarpPointer(cx, cy);
}
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouseActive = true;
        glutSetCursor(GLUT_CURSOR_NONE);
        glutWarpPointer(windowWidth/2, windowHeight/2);
    }
}
void idle() { updateCamera(); glutPostRedisplay(); }

void reshape(int w, int h) {
    windowWidth = w; windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, (double)w/h, 0.1, 300.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("batu onsen");
    glClearColor(0.45f, 0.65f, 0.85f, 1.0f);
    initLighting();
    initRocks();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMotion);
    glutMotionFunc(mouseMotion);
    glutMouseFunc(mouse);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}

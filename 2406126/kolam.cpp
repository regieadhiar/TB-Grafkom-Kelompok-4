// ---------------------------------- library
#include <GL/freeglut.h>
#include <cmath>
#include <iostream>

// ---------------------------------- konstanta
const float PI = 3.14159265f;
const float MOVE_SPEED = 5.0f;          // unit per detik
const float MOUSE_SENSITIVITY = 0.1f;   // sensitivitas mouse

// ---------------------------------- variabel kamera
float cameraPosX = 0.0f, cameraPosY = 3.0f, cameraPosZ = 8.0f;
float cameraYaw = 0.0f;      // rotasi horizontal (derajat)
float cameraPitch = -20.0f; // rotasi vertikal (derajat)

int windowWidth = 1200, windowHeight = 800;
bool mouseCaptured = false;

bool keys[256] = {false};

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

// ---------------------------------- inisialisasi OpenGL
void init() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    // Directional light tetap dari atas tengah (seperti matahari)
    GLfloat light_direction[] = {0.0f, -10.5f, 0.0f, 0.0f}; // w=0 → directional
    GLfloat light_diffuse[]   = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light_ambient[]   = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat light_specular[]  = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    // Material agar ada sedikit highlight
    GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat mat_shininess[] = {50.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void batu(int rockType = 0) {
    switch (rockType)
    {
    case 1:
        glColor3f(0.05f, 0.55f, 0.45f);
        glScalef(0.5f, 0.5f, 0.5f); //Scale
        glutSolidIcosahedron();
        break;
    default:
        glColor3f(0.05f, 0.55f, 0.45f);
        glScalef(0.25f, 0.25f, 0.25f); //Scale
        glutSolidDodecahedron();
        break;
    }
}

void drawObject() {
    glPushMatrix(); 
        glTranslatef(0.5f, 0.0f, 1.5f); //Translasi
        batu();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(1.5f, 0.0f, 1.5f); //Translasi
        batu();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(2.5f, 0.0f, 1.5f); //Translasi
        batu();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(3.5f, 0.0f, 1.5f); //Translasi
        batu(1);
    glPopMatrix();
    // Tambahkan bagian-bagian lain dari objek di sini
}

void drawFloor() {
    // Lantai abu-abu gelap
    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.2f, 0.25f);
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f(-10.0f, 0.0f,  10.0f);
    glVertex3f( 10.0f, 0.0f,  10.0f);
    glVertex3f( 10.0f, 0.0f, -10.0f);
    glEnd();

    // Garis grid
    glDisable(GL_LIGHTING);
    glColor3f(0.3f, 0.3f, 0.35f);
    glBegin(GL_LINES);
    for (float i = -10.0f; i <= 10.0f; i += 1.0f) {
        glVertex3f(i, 0.01f, -10.0f); glVertex3f(i, 0.01f, 10.0f);
        glVertex3f(-10.0f, 0.01f, i); glVertex3f(10.0f, 0.01f, i);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

// ---------------------------------- pergerakan kamera
void updateCameraMovement() {
    float yawRad = cameraYaw * PI / 180.0f;
    float pitchRad = cameraPitch * PI / 180.0f;

    float frontX = sin(yawRad) * cos(pitchRad);
    float frontY = sin(pitchRad);
    float frontZ = -cos(yawRad) * cos(pitchRad);

    float speed = MOVE_SPEED * deltaTime;

    if (keys['w'] || keys['W']) {
        cameraPosX += frontX * speed;
        cameraPosY += frontY * speed;
        cameraPosZ += frontZ * speed;
    }
    if (keys['s'] || keys['S']) {
        cameraPosX -= frontX * speed;
        cameraPosY -= frontY * speed;
        cameraPosZ -= frontZ * speed;
    }
    if (keys['a'] || keys['A']) {
        float rightX = cos(yawRad);
        float rightZ = sin(yawRad);
        cameraPosX -= rightX * speed;
        cameraPosZ -= rightZ * speed;
    }
    if (keys['d'] || keys['D']) {
        float rightX = cos(yawRad);
        float rightZ = sin(yawRad);
        cameraPosX += rightX * speed;
        cameraPosZ += rightZ * speed;
    }
    if (keys['e'] || keys['E']) cameraPosY += speed;  // Naik
    if (keys['q'] || keys['Q']) cameraPosY -= speed;  // Turun
}

// ---------------------------------- GLUT callbacks
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)windowWidth / windowHeight, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float yawRad = cameraYaw * PI / 180.0f;
    float pitchRad = cameraPitch * PI / 180.0f;

    float lookX = cameraPosX + sin(yawRad) * cos(pitchRad);
    float lookY = cameraPosY + sin(pitchRad);
    float lookZ = cameraPosZ - cos(yawRad) * cos(pitchRad);

    gluLookAt(cameraPosX, cameraPosY, cameraPosZ,
              lookX, lookY, lookZ,
              0.0f, 1.0f, 0.0f);

    drawFloor();
    drawObject();

    glutSwapBuffers();
}

void mouseMotion(int x, int y) {
    if (!mouseCaptured) return;

    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;

    float deltaX = (x - centerX) * MOUSE_SENSITIVITY;
    float deltaY = (y - centerY) * MOUSE_SENSITIVITY;

    cameraYaw   += deltaX;
    cameraPitch -= deltaY; // dibalik agar sesuai intuisi

    if (cameraPitch > 89.0f)  cameraPitch = 89.0f;
    if (cameraPitch < -89.0f) cameraPitch = -89.0f;

    glutWarpPointer(centerX, centerY);
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouseCaptured = true;
        glutSetCursor(GLUT_CURSOR_NONE);
        glutWarpPointer(windowWidth / 2, windowHeight / 2);
    }
}

void keyboard(unsigned char key, int x, int y) {
    key = tolower(key);
    keys[key] = true;

    if (key == 27) { // ESC
        if (mouseCaptured) {
            mouseCaptured = false;
            glutSetCursor(GLUT_CURSOR_INHERIT);
        } else {
            exit(0);
        }
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    key = tolower(key);
    keys[key] = false;
}

void timer(int value) {
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    updateCameraMovement();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Kolam");

    // glutFullScreen(); // uncomment jika ingin fullscreen

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMotion);
    glutMotionFunc(mouseMotion);
    glutMouseFunc(mouse);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
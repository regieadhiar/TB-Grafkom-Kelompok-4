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
    // Sedikit menggelapkan langit agar spotlight terlihat kontras
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f); 
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // Cahaya Matahari/Umum
    glEnable(GL_LIGHT1); // Cahaya Spotlight Lampu Taman (PENTING)
    
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE); // Agar lighting tetap akurat meski di-scale

    // --- Setting LIGHT 0 (Ambience Global) ---
    GLfloat light_direction[] = {0.0f, -10.5f, 0.0f, 0.0f}; 
    GLfloat light_diffuse[]   = {0.4f, 0.4f, 0.4f, 1.0f}; // Sedikit redup
    GLfloat light_ambient[]   = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat light_specular[]  = {0.5f, 0.5f, 0.5f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    // --- Setting LIGHT 1 (Persiapan Spotlight) ---
    // Kita set parameter fisik spotlight di sini
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0f);    // Lebar sorotan 45 derajat
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0f);   // Fokus sorotan
    
    GLfloat spot_diffuse[]  = {1.0f, 0.95f, 0.8f, 1.0f}; // Cahaya hangat
    GLfloat spot_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, spot_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spot_specular);
    
    // Attenuation (Meredup seiring jarak)
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.05f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.002f);
}

// ---------------------------------- ASET OBJEK (LAMPU)
// Fungsi Modular untuk Menggambar Lampu Taman Modern
void drawGardenLamp(float x, float y, float z, float scale, bool isActive) {
    glPushMatrix();
        glTranslatef(x, y, z);
        glScalef(scale, scale, scale);

        // --- 1. TIANG BESI (Batangan Bawah) ---
        glColor3ub(30, 30, 35); // Abu-abu gelap (besi)
        
        // Material Specular untuk besi (mengkilap tajam)
        GLfloat iron_spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat iron_shine[] = { 50.0f };
        glMaterialfv(GL_FRONT, GL_SPECULAR, iron_spec);
        glMaterialfv(GL_FRONT, GL_SHININESS, iron_shine);

        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.0f); 
            glRotatef(-90, 1.0f, 0.0f, 0.0f); // Tegakkan silinder
            // Tiang: radius 0.1, tinggi 3.0
            glutSolidCylinder(0.1, 3.0, 16, 16); 
            // Base/Dudukan
            glutSolidCylinder(0.3, 0.1, 16, 1);
        glPopMatrix();

        // --- 2. BOLA LAMPU (Bulb Diatas) ---
        glPushMatrix();
            glTranslatef(0.0f, 3.0f, 0.0f); // Posisi di ujung tiang
            
            // Material Emission (Agar bola terlihat menyala putih terang)
            GLfloat bulb_emission[] = { 1.0f, 1.0f, 0.9f, 1.0f }; 
            glMaterialfv(GL_FRONT, GL_EMISSION, bulb_emission);
            
            glColor3ub(255, 255, 240); // Warna visual bola
            glutSolidSphere(0.5, 20, 20); 

            // --- LOGIKA SPOTLIGHT (GL_LIGHT1) ---
            if (isActive) {
                // Posisi cahaya menempel pada bola (0,0,0 relatif terhadap bola)
                GLfloat light_pos[] = { 0.0f, 0.0f, 0.0f, 1.0f }; 
                // Arah sorotan ke bawah (Sumbu Y negatif)
                GLfloat spot_dir[]  = { 0.0f, -1.0f, 0.0f }; 

                glLightfv(GL_LIGHT1, GL_POSITION, light_pos);
                glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_dir);
            }

            // Matikan emission agar objek lain (lantai dll) tidak ikut "menyala"
            GLfloat no_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
            glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);

        glPopMatrix();

        // --- 3. TOPI/PENUTUP (Opsional) ---
        glPushMatrix();
            glColor3ub(30, 30, 35); // Kembali ke warna besi
            glTranslatef(0.0f, 2.5f, 0.0f);
            glRotatef(-90, 1.0f, 0.0f, 0.0f);
            glutSolidCone(0.15, 0.2, 10, 2); 
        glPopMatrix();

    glPopMatrix();
}

void drawObject() {
    // Panggil fungsi lampu disini
    // Parameter: x, y, z, scale, isActive
    // isActive = true artinya lampu ini yang memancarkan cahaya spotlight
    
    drawGardenLamp(0.0f, 0.0f, 0.0f, 1.0f, true);
}

void drawFloor() {
    // Lantai abu-abu gelap
    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.2f, 0.25f);
    glNormal3f(0.0f, 1.0f, 0.0f); // Normal ke atas agar memantulkan cahaya
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
    glutCreateWindow("Bangunan Depan Onsen - Lampu");

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
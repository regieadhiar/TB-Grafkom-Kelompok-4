// ---------------------------------- library
#include <GL/freeglut.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

// ---------------------------------- konstanta
const float PI = 3.14159265f;
const float MOVE_SPEED = 5.0f;
const float MOUSE_SENSITIVITY = 0.1f;

// ---------------------------------- variabel kamera
float cameraPosX = 0.0f, cameraPosY = 3.0f, cameraPosZ = 8.0f;
float cameraYaw = 0.0f;
float cameraPitch = -20.0f;
int windowWidth = 1200, windowHeight = 800;
bool mouseCaptured = false;
bool keys[256] = {false};
float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

// ---------------------------------- PARTIKEL ASAP ONSEN (sekarang berbentuk sphere)
struct Particle {
    float x, y, z;
    float vy;       // kecepatan vertikal
    float life;     // 1.0 = baru lahir, 0.0 = mati
    float size;     // ukuran sphere
};

std::vector<Particle> particles;
float lastBurstTime = 0.0f;
float nextBurstInterval = 7.0f;
bool randSeeded = false;

// Posisi sumber asap (di tengah scene, sedikit di atas lantai)
const float smokeSourceX = 0.0f;
const float smokeSourceY = -1.5f;   // mulai sedikit di atas lantai
const float smokeSourceZ = 0.0f;

void updateParticles() {
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    // Burst baru setiap 5-10 detik
    if (currentTime - lastBurstTime >= nextBurstInterval) {
        lastBurstTime = currentTime;
        nextBurstInterval = 5.0f + static_cast<float>(rand()) / RAND_MAX * 5.0f;

        int numParticles = 50 + rand() % 25;  // 30-54 partikel per burst
        for (int i = 0; i < numParticles; ++i) {
            Particle p;
            p.x = smokeSourceX + (static_cast<float>(rand()) / RAND_MAX - 1.0f) * 5.0f;
            p.y = smokeSourceY;
            p.z = smokeSourceZ + (static_cast<float>(rand()) / RAND_MAX - 1.0f) * 5.0f;
            p.vy = 0.25f + static_cast<float>(rand()) / RAND_MAX * 0.35f;  // naik pelan
            p.life = 1.0f;
            p.size = 0.0008f + static_cast<float>(rand()) / RAND_MAX * 0.12f;  // ukuran awal kecil
            particles.push_back(p);
        }
    }

    // Update partikel
    for (size_t i = 0; i < particles.size(); ) {
        Particle& p = particles[i];

        p.y += p.vy * deltaTime * 1.5f;
        p.vy += 0.06f * deltaTime;          // sedikit mengambang ke atas
        p.life -= deltaTime * 0.1f;        // hidup sekitar 4-5 detik
        p.size += deltaTime * 0.007f;         // melebar seiring waktu

        if (p.life <= 0.0f) {
            particles.erase(particles.begin() + i);
        } else {
            ++i;
        }
    }
}

void renderSmoke() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);  // tetap aktif agar tidak tembus lantai

    glDisable(GL_LIGHTING);   // agar warna asap tidak dipengaruhi cahaya terlalu kuat

    for (const Particle& p : particles) {
        float alpha = p.life * 0.1f;  // transparan tipis
        glColor4f(0.95f, 0.97f, 1.0f, alpha);  // putih kebiruan sangat lembut

        glPushMatrix();
        glTranslatef(p.x, p.y, p.z);
        glScalef(1.0f, 1.5f, 1.0f);
        glutSolidSphere(p.size * p.life * 1.8f, 12, 8);  // sphere bulat lembut, ukuran mengecil saat memudar
        glPopMatrix();
    }

    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
}

// ---------------------------------- inisialisasi OpenGL
void init() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    GLfloat light_direction[] = {0.0f, -10.5f, 0.0f, 0.0f};
    GLfloat light_diffuse[]   = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light_ambient[]   = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat light_specular[]  = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat mat_shininess[] = {50.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    if (!randSeeded) {
        srand(static_cast<unsigned>(time(NULL)));
        randSeeded = true;
    }
}

void drawObject() {
    // Kubus dihilangkan — sekarang hanya asap yang dirender
    renderSmoke();
}

void drawFloor() {
    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.2f, 0.25f);
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f(-10.0f, 0.0f,  10.0f);
    glVertex3f( 10.0f, 0.0f,  10.0f);
    glVertex3f( 10.0f, 0.0f, -10.0f);
    glEnd();

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

// ---------------------------------- pergerakan kamera (sama seperti sebelumnya)
void updateCameraMovement() {
    float yawRad = cameraYaw * PI / 180.0f;
    float pitchRad = cameraPitch * PI / 180.0f;
    float frontX = sin(yawRad) * cos(pitchRad);
    float frontY = sin(pitchRad);
    float frontZ = -cos(yawRad) * cos(pitchRad);
    float speed = MOVE_SPEED * deltaTime;

    if (keys['w'] || keys['W']) { cameraPosX += frontX * speed; cameraPosY += frontY * speed; cameraPosZ += frontZ * speed; }
    if (keys['s'] || keys['S']) { cameraPosX -= frontX * speed; cameraPosY -= frontY * speed; cameraPosZ -= frontZ * speed; }
    if (keys['a'] || keys['A']) { float rightX = cos(yawRad); float rightZ = sin(yawRad); cameraPosX -= rightX * speed; cameraPosZ -= rightZ * speed; }
    if (keys['d'] || keys['D']) { float rightX = cos(yawRad); float rightZ = sin(yawRad); cameraPosX += rightX * speed; cameraPosZ += rightZ * speed; }
    if (keys['e'] || keys['E']) cameraPosY += speed;
    if (keys['q'] || keys['Q']) cameraPosY -= speed;
}

// ---------------------------------- GLUT callbacks (sama)
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
    drawObject();  // hanya memanggil renderSmoke()

    glutSwapBuffers();
}

void mouseMotion(int x, int y) {
    if (!mouseCaptured) return;
    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;
    float deltaX = (x - centerX) * MOUSE_SENSITIVITY;
    float deltaY = (y - centerY) * MOUSE_SENSITIVITY;
    cameraYaw += deltaX;
    cameraPitch -= deltaY;
    if (cameraPitch > 89.0f) cameraPitch = 89.0f;
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
    if (key == 27) {
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
    updateParticles();

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
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
    glutCreateWindow("Efek Asap Onsen");
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
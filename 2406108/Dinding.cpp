#include <GL/glut.h>

// Variabel rotasi
float rotateX = 0.0f;
float rotateY = 0.0f;

void init() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // Background
    glEnable(GL_DEPTH_TEST);           // Aktifkan depth testing

}
void drawFloor() {
    glPushMatrix();
    
    // Posisi
    glTranslatef(1.0f, -1.1f, 1.0f); // turun kebawah

    // Bentuk
    glScalef(10.0f, 0.2f, 10.0f); 
    
    // Warna dan Gambar
    glColor3f(0.3f, 0.3f, 0.3f); 
    glutSolidCube(1.0f); 
    glPopMatrix();
}
void drawRoof() { 
    float LB = 2.2f; // Lebar Bawah
    float LA = 2.0f; // Lebar Atas 
    float TB = 0.3f; // Tebal Bawah
    float TA = 0.15f;// Tebal Atas
    float T  = 0.5f; // Tinggi Atap

    glBegin(GL_QUADS);
    
    // Sisi Depan
    glColor3f(1.0f, 0.0f, 0.0f); 
    glVertex3f(-LA, T, TA);    // Kiri Atas
    glVertex3f(-LB, 0.0f, TB); // Kiri Bawah
    glVertex3f( LB, 0.0f, TB); // Kanan Bawah
    glVertex3f( LA, T, TA);    // Kanan Atas

    // Sisi Belakang 
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-LA, T, -TA);
    glVertex3f( LA, T, -TA);
    glVertex3f( LB, 0.0f, -TB);
    glVertex3f(-LB, 0.0f, -TB);

    // Sisi Atas
    glColor3f(1.0f, 0.0f, 0.5f);
    glVertex3f(-LA, T,  TA);
    glVertex3f( LA, T,  TA);
    glVertex3f( LA, T, -TA);
    glVertex3f(-LA, T, -TA);

    // Sisi Bawah 
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-LB, 0.0f,  TB);
    glVertex3f(-LB, 0.0f, -TB);
    glVertex3f( LB, 0.0f, -TB);
    glVertex3f( LB, 0.0f,  TB);

    // Sisi Kiri 
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-LA, T, -TA);
    glVertex3f(-LB, 0.0f, -TB);
    glVertex3f(-LB, 0.0f, TB);
    glVertex3f(-LA, T, TA);

    // Sisi Kanan
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f( LA, T, TA);
    glVertex3f( LB, 0.0f, TB);
    glVertex3f( LB, 0.0f, -TB);
    glVertex3f( LA, T, -TA);

    glEnd();
}
void drawWall() 
{
    // --- DINDING 1 ---
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f); // Posisi dinding
    glScalef(4.0f, 2.0f, 0.2f);     // Bentuk 
    glColor3f(0.0f, 1.0f, 0.0f);    // Warna
    glutSolidCube(1.0f); 
    glPopMatrix();

    // --- ATAP 1---
    glPushMatrix();
    glTranslatef(0.0f, 1.0f, 0.0f); // Naik ke atas dinding
    drawRoof(); // Panggil fungsi atap
    glPopMatrix();

    // --- DINDING 2 ---
    glPushMatrix();
    glTranslatef(1.9f, 0.0f, 2.1f); // Posisi dinding
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // Putar 90 derajat 
    glScalef(4.0f, 2.0f, 0.2f);     // Bentuk
    glColor3f(0.0f, 0.0f, 1.0f);    // Warna 
    glutSolidCube(1.0f); 
    glPopMatrix();

    // --- ATAP 2---
    glTranslatef(1.9f, 1.0f, 2.1f); 
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f); 
    
    drawRoof(); // Panggil fungsi atap
    glPopMatrix();

}
// Fungsi display untuk menggambar scene
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    //Posisi kamera
    glTranslatef(0.0f, 0.0f, -10.0f); 

    //Rotasi berdasarkan input keyboard
    glRotatef(rotateX, 1.0f, 0.0f, 0.0f); 
    glRotatef(rotateY, 0.0f, 1.0f, 0.0f); 

    //Gambar lantai dan dinding dengan atap
    drawFloor();
    drawWall();
    glutSwapBuffers();
}

void mykeyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'd': rotateY += 5.0f; break; // Putar Kanan
        case 'a': rotateY -= 5.0f; break; // Putar Kiri
        case 'w': rotateX -= 5.0f; break; // Putar Atas
        case 's': rotateX += 5.0f; break; // Putar Bawah
    }
    glutPostRedisplay(); // Refresh layar setelah rotasi
}

// Fungsi reshape untuk menyesuaikan viewport dan proyeksi
void reshape(int w, int h) {
    if (h == 0) h = 1;
    float aspect = (float)w / (float)h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}
// Fungsi utama
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Aktifkan double buffering dan depth buffer
    glutInitWindowSize(800, 600);
    glutCreateWindow("Dinding dengan Atap");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(mykeyboard);

    glutMainLoop();
    return 0;
}
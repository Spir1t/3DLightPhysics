#define _CRT_SECURE_NO_WARNINGS

#include <GL/glut.h>
#include <cmath>
#include <cstdio>

float angle = 0.0f;
float faceOffset = 0.0f;
bool expand = false;
bool transparent = false;
bool useTexture = true;
GLuint textureID;
float rotateX = 0;
float rotateY = 0;

GLuint loadBMP(const char* filename) {
	FILE* file = fopen(filename, "rb");
	if (!file) {
		printf("�� ������� ������� BMP: %s\n", filename);
		return 0;
	}

	unsigned char header[54];
	fread(header, 1, 54, file);
	unsigned int dataPos = *(int*)&(header[0x0A]);
	unsigned int width = *(int*)&(header[0x12]);
	unsigned int height = *(int*)&(header[0x16]);
	unsigned int imageSize = *(int*)&(header[0x22]);
	if (imageSize == 0) imageSize = width * height * 3;
	if (dataPos == 0) dataPos = 54;

	unsigned char* data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);

	delete[] data;
	return texture;
}

void drawFace(float x, float y, float z, float nx, float ny, float nz) {
	glPushMatrix();
	glTranslatef(x + faceOffset * nx, y + faceOffset * ny, z + faceOffset * nz);

	float alpha = transparent ? 0.4f : 1.0f;
	glColor4f(1.0f, 1.0f, 1.0f, alpha);

	glBegin(GL_QUADS);
	glNormal3f(nx, ny, nz);

	// Выбираем порядок обхода в зависимости от направления нормали
	if (nx + ny + nz > 0) {
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.0f);
	}
	else {
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, -0.5f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, -0.5f, 0.0f);
	}

	glEnd();
	glPopMatrix();
}

void drawExplodableCube() {
	drawFace(0, 0, 0.5f, 0, 0, 1);
	drawFace(0, 0, -0.5f, 0, 0, -1);

	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	drawFace(0, 0, 0.5f, 0, 0, 1);
	drawFace(0, 0, -0.5f, 0, 0, -1);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	drawFace(0, 0, 0.5f, 0, 0, 1);
	drawFace(0, 0, -0.5f, 0, 0, -1);
	glPopMatrix();
}


void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(3, 3, 3, 0, 0, 0, 0, 1, 0);

	float lightX = 3.0f * cos(angle);
	float lightZ = 3.0f * sin(angle);
	GLfloat lightPos[] = { lightX, 0, lightZ, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glPushMatrix();
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(lightX, 0, lightZ);
	glutSolidSphere(0.1f, 20, 20);
	glEnable(GL_LIGHTING);
	glPopMatrix();

	if (useTexture) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}

	//glPushMatrix();
	glRotatef(rotateX, 1, 0, 0);
	glRotatef(rotateY, 0, 1, 0);
	drawExplodableCube();
	//glPopMatrix;

	glutSwapBuffers();
}

void timer(int value) {
	angle += 0.01f;
	if (expand && faceOffset < 1.0f) faceOffset += 0.002f;
	else if (!expand && faceOffset > 0.0f) faceOffset -= 0.002f;

	glutPostRedisplay();
	glutTimerFunc(16, timer, 0);
}

void reshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, float(w) / float(h), 1.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
	if (key == 'e') expand = true;
	else if (key == 'c') expand = false;
	else if (key == 't') transparent = true;
	else if (key == 'o') transparent = false;
	else if (key == 'x') useTexture = !useTexture;
	else if (key == 'a') rotateY -= 5;
	else if (key == 'd') rotateY += 5;
	else if (key == 'w') rotateX -= 5;
	else if (key == 's') rotateX += 5;
	else if (key == 27) exit(0);
}

void init() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	textureID = loadBMP("D:/coding projects/Comp Graphics/Lab345/brick_texture.bmp");
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Cube Rinchinov");

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(0, timer, 0);

	glutMainLoop();
	return 0;
}
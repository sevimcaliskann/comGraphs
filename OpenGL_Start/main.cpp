//#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES 
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <math.h>
#include <time.h>
#include <string>
#include <random>


#include "./models/mesh_bunny.h"
#include "./models/mesh_cow.h"
#include "./models/mesh_cube.h"
#include "./models/mesh_frog.h"
#include "./models/mesh_knot.h"
#include "./models/mesh_sphere.h"
#include "./models/mesh_teapot.h"
#include "./models/mesh_triceratops.h"

#include "algebra.h"
#include "shaders.h"
#include "mesh.h"


int screen_width = 1024;
int screen_height = 768;


Mesh *meshList = NULL; // Pointer to linked list of triangle meshes
Mesh *selectedMesh = NULL;
Mesh *boundingSpheres = NULL;
Sphere *bs;
bool isVFEnabled = false;
bool enableBS = false;
int multipleLight = 0;
int shader = 0;


Camera cam = {{0,0,20}, {0,0,0}, 60, 1, 10000}; // Setup the camera parameters


GLuint shprg; // Shader program id


// Transform matrices
// V is view transform
// P is the projection transform
// PV = P * V
Matrix V, P, PV, T, tempT = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
Vector S = { 1, 1, 1 }, TR = { 0, 0, 0 }, RO = { 0, 0, 0 };
extern long int getSeed(){
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, RAND_MAX);
	return dis(gen);
}


bool isVisible(Sphere *sphere){
	HomVector planes[6] = { {1,0,0,1}, {-1,0,0,1}, 
							{0,1,0,1}, {0,-1,0,1}, 
							{0,0,1,1}, {0,0,-1,1} };
	Matrix transposed = Transpose(PV);
	for (int i = 0; i < 6; i++){
		planes[i] = MatVecMul(transposed, { planes[i].x / planes[i].w, planes[i].y / planes[i].w, planes[i].z / planes[i].w });
		//Vector a = Homogenize(planes[i]);
		float result = planes[i].x *(sphere->center.x) + planes[i].y*(sphere->center.y) + planes[i].z*(sphere->center.z) + planes[i].w;
		//float result = a.x *(sphere->center.x) + a.y*(sphere->center.y) + a.z*(sphere->center.z);
		if (result < (-1)*sphere->radius)
			return false;
	}
	return true;
}

std::string readFile(const char *filePath) {
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}


void prepareShaderProgram() {
	GLint compiled, linked;
	shprg = glCreateProgram();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragShaderStr;// = readFile("C:/Users/sevimcaliskan/Documents/visual studio 2013/Projects/OpenGL_Start/OpenGL_Start/shaders/Gouraudshadingfs.txt");
	switch (shader){
	case 0:
		fragShaderStr = readFile("C:/Users/sevimcaliskan/Documents/visual studio 2013/Projects/OpenGL_Start/OpenGL_Start/shaders/Gouraudshadingfs.txt");
		std::cout << "0" << std::endl;
		break;
	case 1:
		fragShaderStr = readFile("C:/Users/sevimcaliskan/Documents/visual studio 2013/Projects/OpenGL_Start/OpenGL_Start/shaders/PhongShadingfs.txt");
		std::cout << "1" << std::endl;
		break;
	case 2:
		fragShaderStr = readFile("C:/Users/sevimcaliskan/Documents/visual studio 2013/Projects/OpenGL_Start/OpenGL_Start/shaders/cartoonshadingfs.txt");
		std::cout << "2" << std::endl;
		break;
	}
	const char *fragShaderSrc = fragShaderStr.c_str();
	glShaderSource(fs, 1, &fragShaderSrc, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compiled);
	if (!compiled){
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &compiled);
		GLchar *info = new GLchar[compiled];
		glGetShaderInfoLog(fs, compiled, &compiled, info);
		printf(info);
		delete info;
	}

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	std::string vertShaderStr;
	switch (shader){
	case 0:
		vertShaderStr = readFile("C:/Users/sevimcaliskan/Documents/visual studio 2013/Projects/OpenGL_Start/OpenGL_Start/shaders/Gouraudshadingvs.txt");
		std::cout << "0" << std::endl;
		break;
	case 1:
		vertShaderStr = readFile("C:/Users/sevimcaliskan/Documents/visual studio 2013/Projects/OpenGL_Start/OpenGL_Start/shaders/PhoneShadingvs.txt");
		std::cout << "1" << std::endl;
		break;
	case 2:
		vertShaderStr = readFile("C:/Users/sevimcaliskan/Documents/visual studio 2013/Projects/OpenGL_Start/OpenGL_Start/shaders/cartoonshadingvs.txt");
		std::cout << "2" << std::endl;
		break;
	}
	
	const char *vertShaderSrc = vertShaderStr.c_str();
	glShaderSource(vs, 1, &vertShaderSrc, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compiled);
	if (!compiled){
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &compiled);
		GLchar *info = new GLchar[compiled];
		glGetShaderInfoLog(vs, compiled, &compiled, info);
		printf(info);
		delete info;
	}

	glAttachShader(shprg, vs);
	glAttachShader(shprg, fs);

	glLinkProgram(shprg);
	glGetProgramiv(shprg, GL_LINK_STATUS, &linked);
	if (!linked) {printf("Link Error Shader Program!\n"); }
}

void transformMesh(Mesh *mesh, Matrix mat){
	HomVector temp;
	for (int i = 0; i < mesh->nv; i++){
		temp = MatVecMul(mat, mesh->vertices[i]);
		mesh->vertices[i] = { temp.x / temp.w, temp.y / temp.w, temp.z / temp.w };
	}
	calculateNorms(mesh);
}

void prepareMesh(Mesh *mesh) {
	T = ModelViewMatrix(RO.x, RO.y, RO.z, TR, S);
	TR = { 0, 0, 0 }; S = { 1, 1, 1 }; RO = { 0, 0, 0 };

	int sizeVerts = mesh->nv * 3 * sizeof(float);
	int sizeCols  = mesh->nv * 3 * sizeof(float);
	int sizeTris = mesh->nt * 3 * sizeof(int);

	// Allocate GPU buffer and load mesh data
	glGenBuffers(1, &mesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeVerts + sizeCols, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVerts, (void *)mesh->vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeVerts, sizeCols, (void *)mesh->vnorms);

	// For specification of the data stored in the vbo
	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	// Allocate GPU index buffer and load mesh indices
	glGenBuffers(1, &mesh->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeTris, (void *)mesh->triangles, GL_STATIC_DRAW);
}


void renderMesh(Mesh *mesh) {
	GLint loc_PV = glGetUniformLocation(shprg, "PV");
	glUniformMatrix4fv(loc_PV, 1, GL_FALSE, PV.e);

	GLint loc_cam = glGetUniformLocation(shprg, "camPos");
	glUniform3fv(loc_cam, 1, convertToArray(cam.position));

	GLint multiple = glGetUniformLocation(shprg, "multiple");
	glUniform1i(multiple, multipleLight);

	// Select current resources 
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBindBuffer(GL_VERTEX_ARRAY, mesh->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);

	// Set up vertex array
	GLint vPos = glGetAttribLocation(shprg, "vPos");
	glEnableVertexAttribArray(vPos);
	glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// Set up normal array
	GLint vNorm = glGetAttribLocation(shprg, "vNorm");
	glEnableVertexAttribArray(vNorm);
	glVertexAttribPointer(vNorm, 3, GL_FLOAT, GL_FALSE, 0, (void *)(mesh->nv * 3 *sizeof(float)));
	
	// To accomplish wireframe rendering (can be removed to get filled triangles)
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 

	
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	// Draw all triangles
	glDrawElements(GL_TRIANGLES, mesh->nt * 3, GL_UNSIGNED_INT, NULL); 
	//glFinish();
}

void moveCamera(void){
	T.e[0] = 1.0f; T.e[4] = 0.0f; T.e[8] = 0.0f; T.e[12] = -cam.position.x;
	T.e[1] = 0.0f; T.e[5] = 1.0f; T.e[9] = 0.0f; T.e[13] = -cam.position.y;
	T.e[2] = 0.0f; T.e[6] = 0.0f; T.e[10] = 1.0f; T.e[14] = -cam.position.z;
	T.e[3] = 0.0f; T.e[7] = 0.0f; T.e[11] = 0.0f; T.e[15] = 1.0f;

	V.e[0] = 1.0f; V.e[4] = 0.0f; V.e[8] = 0.0f; V.e[12] = 0;
	V.e[1] = 0.0f; V.e[5] = 1.0f; V.e[9] = 0.0f; V.e[13] = 0;
	V.e[2] = 0.0f; V.e[6] = 0.0f; V.e[10] = 1.0f; V.e[14] = 0;
	V.e[3] = 0.0f; V.e[7] = 0.0f; V.e[11] = 0.0f; V.e[15] = 1.0f;

	float t = (1 / tan(M_PI*cam.fov / (180 * 2)));
	P.e[0] = t / 1.25; P.e[4] = 0.000000f; P.e[8] = 0.000000f; P.e[12] = 0.0f;
	P.e[1] = 0.000000f; P.e[5] = t; P.e[9] = 0.000000f; P.e[13] = 0.0f;
	P.e[2] = 0.000000f; P.e[6] = 0.000000f; P.e[10] = (cam.farPlane + cam.nearPlane) / (cam.nearPlane - cam.farPlane); P.e[14] = 2 * cam.farPlane*cam.nearPlane / (cam.nearPlane - cam.farPlane);
	P.e[3] = 0.000000f; P.e[7] = 0.000000f; P.e[11] = -1.000000f; P.e[15] = 0.0f;

	Matrix R = RotationMatrix(cam.rotation.x, X);
	V = MatMatMul(V, R);
	R = RotationMatrix(cam.rotation.y, Y);
	V = MatMatMul(V, R);
	R = RotationMatrix(cam.rotation.z, Z);
	V = MatMatMul(V, R);
	V = MatMatMul(V, T);
	PV = MatMatMul(P, V);
}

Vector transformVector(const Vector &a, Matrix m){
	HomVector hv = MatVecMul(m, a);
	return Homogenize(hv);
}
void display(void) {
	Mesh *mesh, *boundingMesh;
	glClear(GL_COLOR_BUFFER_BIT);

	tempT = ModelViewMatrix(RO.x, RO.y, RO.z, TR, S);
	TR = { 0, 0, 0 }; S = { 1, 1, 1 }; RO = { 0, 0, 0 };
	
	moveCamera();
	mesh = meshList;
	boundingMesh = boundingSpheres;
	Sphere *test = bs;
	clock_t t = clock();
	while (mesh != NULL) {
		if (!selectedMesh || selectedMesh == mesh){
			transformMesh(mesh, tempT);
			transformMesh(boundingMesh, tempT);
			test->center = transformVector(test->center, tempT);
		}

		if (!isVFEnabled || isVisible(test)){
			prepareMesh(mesh);
			prepareMesh(boundingMesh);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			renderMesh(mesh);
			if (enableBS){
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				renderMesh(boundingMesh);
			}
		}
		else
			std::cout << "NOT RENDERING" << std::endl;
		mesh = mesh->next;
		boundingMesh = boundingMesh->next;
		test = test->next;
	}
	glUseProgram(shprg);
	//glFlush();
	glFinish();
	std::cout << "With isVFEnabled: " << isVFEnabled << " " << clock() - t << std::endl;
}
void changeSize(int w, int h) {
	screen_width = w;
	screen_height = h;
	glViewport(0, 0, screen_width, screen_height);

}

void processSpecialKeys(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_F1:
		TR = { 5.0f, 5.0f, 5.0f };
		break;
	case GLUT_KEY_F2:
		TR = { -5.0f, -5.0f, -5.0f };
		break;
	case GLUT_KEY_UP:
		TR = { 0, 0.2f, 0 };
		break;
	case GLUT_KEY_DOWN:
		TR = { 0, -0.2f, 0 };
		break;
	case GLUT_KEY_RIGHT:
		TR = { 0.2f, 0, 0 };
		break;
	case GLUT_KEY_LEFT:
		TR = { -0.2f, 0, 0 };
		break;
	}
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y){

}

void keypress(unsigned char key, int x, int y) {
	switch(key) {
	case '2':
		shader = (shader + 1) % 3;
		prepareShaderProgram();
		break;
	case 'a':
		isVFEnabled = true;
		break;
	case 'A':
		isVFEnabled = false;
		break;
	case 'r':
		enableBS = true;
		break;
	case 'R':
		enableBS = false;
		break;
	case 'x':
		cam.position.x -= 0.2f;
		break;
	case 'X':
		cam.position.x += 0.2f;
		break;
	case 'y':
		cam.position.y -= 0.2f;
		break;
	case 'Y':
		cam.position.y += 0.2f;
	case 'z':
		cam.position.z -= 0.2f;
		break;
	case 'Z':
		cam.position.z += 0.2f;
		break;
	case 'i':
		cam.rotation.x -=1.0f;
		break;
	case 'I':
		cam.rotation.x += 1.0f;
		break;
	case 'j':
		cam.rotation.y -= 1.0f;
		break;
	case 'J':
		cam.rotation.y += 1.0f;
		break;
	case 'k':
		cam.rotation.z -= 1.0f;
		break;
	case 'K':
		cam.rotation.z += 1.0f;
		break;

	case 'o':
		RO.x -= 1.0f;
		break;
	case 'O':
		RO.x += 1.0f;
		break;
	case 'p':
		RO.y -= 1.0f;
		break;
	case 'P':
		RO.y += 1.0f;
		break;
	case 'l':
		RO.z -= 1.0f;
		break;
	case 'L':
		RO.z += 1.0f;
		break;

	case 's':
		S.x -= 0.1f;
		S.y -= 0.1f;
		S.z -= 0.1f;
		break;
	case 'S':
		S.x += 0.1f;
		S.y += 0.1f;
		S.z += 0.1f;
		break;
	case 'w':
		selectedMesh = meshList;
		break;
	case 'n':
		if (selectedMesh){
			selectedMesh = selectedMesh->next;
		}
		else
			MessageBox(NULL, "Select A Mesh!", "Empty Mesh!",
			MB_OK | MB_ICONEXCLAMATION);
		break;
	case 'e':
		selectedMesh = NULL;
		break;
	//case 'Q':
	case 'q':
		exit(0);
		break;
	case '1':
		multipleLight = !multipleLight;
		break;
	}
	glutPostRedisplay();
}

void init(void) {
	// Setup OpenGL buffers for rendering of the meshes
	Mesh * mesh = meshList;
	while (mesh != NULL) {
		prepareMesh(mesh);
		mesh = mesh->next;
	}
	
	// Compile and link shader program
	prepareShaderProgram(); 
}

void cleanUp(void) {	
	// Free openGL resources
	// ...

	// Free meshes
	// ...
}

// Include data for some triangle meshes (hard coded in struct variables)

void insertBoundingVolumes(Mesh *mesh){
	Sphere *temp;
	temp = calculateBoundingSphere(mesh);
	insertModel(&boundingSpheres, sphere.nov, sphere.verts, sphere.nof, sphere.faces, temp->radius);
	transformMesh(boundingSpheres, TranslationMatrix(temp->center.x, temp->center.y, temp->center.z));
	temp->next = bs;
	bs = temp;
}

int main(int argc, char **argv) {
	// Setup freeGLUT	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(screen_width, screen_height);
	glutCreateWindow("DVA304 Assignments");
	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keypress);
	glutMouseFunc(mouse);
	glutSpecialFunc(processSpecialKeys);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// Specify your preferred OpenGL version and profile
	glutInitContextVersion(4, 5);
	//glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);	
	glutInitContextProfile(GLUT_CORE_PROFILE);

	// Use an OpenGL Loading Library to get access to modern core features as well as extensions
	GLenum err = glewInit(); 
	if (GLEW_OK != err) { fprintf(stdout, "Error: %s\n", glewGetErrorString(err)); return 1; }

	// Output OpenGL version info
	fprintf(stdout, "GLEW version: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, "OpenGL version: %s\n", (const char *)glGetString(GL_VERSION));
	fprintf(stdout, "OpenGL vendor: %s\n\n", glGetString(GL_VENDOR));


	// Insert the 3D models you want in your scene here in a linked list of meshes
	// Note that "meshList" is a pointer to the first mesh and new meshes are added to the front of the list
	//insertModel(&meshList, bunny.nov, bunny.verts, bunny.nof, bunny.faces, 60.0);
	//insertBoundingVolumes(meshList);
	//insertModel(&meshList, cow.nov, cow.verts, cow.nof, cow.faces, 20.0);
	//insertBoundingVolumes(meshList);
	//insertModel(&meshList, cube.nov, cube.verts, cube.nof, cube.faces, 5.0);
	//insertBoundingVolumes(meshList);
	//insertModel(&meshList, frog.nov, frog.verts, frog.nof, frog.faces, 2.5);
	//insertBoundingVolumes(meshList);
	//insertModel(&meshList, knot.nov, knot.verts, knot.nof, knot.faces, 1.0);
	//insertBoundingVolumes(meshList);
	//insertModel(&meshList, sphere.nov, sphere.verts, sphere.nof, sphere.faces, 12.0);
	//insertBoundingVolumes(meshList);
	insertModel(&meshList, teapot.nov, teapot.verts, teapot.nof, teapot.faces, 3.0);
	insertBoundingVolumes(meshList);
	//insertModel(&meshList, triceratops.nov, triceratops.verts, triceratops.nof, triceratops.faces, 3.0);
	//insertBoundingVolumes(meshList);

	init();
	glutMainLoop();

	cleanUp();	
	return 0;
}

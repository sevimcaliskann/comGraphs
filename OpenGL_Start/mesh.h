#ifndef _MESH_H_
#define _MESH_H_

#include "algebra.h"

typedef struct _Triangle {
	int vInds[3]; //vertex indices
} Triangle;

typedef struct _Sphere{
	Vector center;
	int radius;
	_Sphere *next;
	_Sphere() :center({0,0,0}), radius(0), next(NULL) {}
	_Sphere(Vector &c, int r, _Sphere *s) :center(c), radius(r), next(s)
	{}
}Sphere;

typedef struct _Mesh { 
	int nv;				
	Vector *vertices;
	Vector *vnorms;
	int nt;				
	Triangle *triangles;
	struct _Mesh *next; 
		
	unsigned int vbo, ibo, vao; // OpenGL handles for rendering
} Mesh;



typedef struct _Camera {
	Vector position;
	Vector rotation;
	double fov; 
	double nearPlane; 
	double farPlane; 
} Camera;

typedef struct lightSource {
	Vector ambient;              // Acli
	Vector diffuse;              // Dcli
	Vector specular;             // Scli
	Vector position;             // Ppli
	lightSource(){
		float R1 = float(rand() % 1000) / 1000;
		float R2 = float(rand() % 1000) / 1000;
		float R3 = float(rand() % 1000) / 1000;
		ambient = { R1, R2, R3 };
		R1 = float(rand() % 1000) / 1000;
		R2 = float(rand() % 1000) / 1000;
		R3 = float(rand() % 1000) / 1000;
		diffuse = { R1, R2, R3 };
		R1 = float(rand() % 1000) / 1000;
		R2 = float(rand() % 1000) / 1000;
		R3 = float(rand() % 1000) / 1000;
		specular = { R1, R2, R3 };
		R1 = float(rand() % 1000) / 1000;
		R2 = float(rand() % 1000) / 1000;
		R3 = float(rand() % 1000) / 1000;
		position = { R1, R2, R3 };
	}
	lightSource(bool empty){
		if (!empty)
			lightSource();
		else{
			ambient = { 0, 0, 0 };
			diffuse = { 0, 0, 0 };
			specular = { 0, 0, 0 };
			position = { 0, 0, 0 };
		}
	}
}LightSource;

void insertModel(Mesh ** objlist, int nv, float * vArr, int nt, int * tArr, float scale = 1.0);
void calculateNorms(Mesh *mesh);
Sphere* calculateBoundingSphere(Mesh *mesh);

#endif

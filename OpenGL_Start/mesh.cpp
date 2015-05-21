#include <stdlib.h>
#include "mesh.h"

float rnd() {
	return 2.0f * float(rand()) / float(RAND_MAX) - 1.0f;
}

void calculateNorms(Mesh *mesh){
	for (int i = 0; i < mesh->nv; i++) {

		mesh->vnorms[i].x = 0;
		mesh->vnorms[i].y = 0;
		mesh->vnorms[i].z = 0;
	}
	for (int i = 0; i < mesh->nt; i++){
		int index = mesh->triangles[i].vInds[0];
		int helper1 = mesh->triangles[i].vInds[1];
		int helper2 = mesh->triangles[i].vInds[2];
		Vector a = mesh->vertices[index];
		Vector b = mesh->vertices[helper1];
		Vector c = mesh->vertices[helper2];
		b = Subtract(b, a);
		c = Subtract(c, a);
		Vector result = CrossProduct(b, c);
		mesh->vnorms[index] = Add(mesh->vnorms[index], result);
		mesh->vnorms[helper1] = Add(mesh->vnorms[helper1], result);
		mesh->vnorms[helper2] = Add(mesh->vnorms[helper2], result);
	}
	for (int i = 0; i < mesh->nv; i++) {
		mesh->vnorms[i] = Normalize(mesh->vnorms[i]);
	}
}

void insertModel(Mesh **list, int nv, float * vArr, int nt, int * tArr, float scale) {
	Mesh * mesh = (Mesh *) malloc(sizeof(Mesh));
	mesh->nv = nv;
	mesh->nt = nt;	
	mesh->vertices = (Vector *) malloc(nv * sizeof(Vector));
	mesh->vnorms = (Vector *)malloc(nv * sizeof(Vector));
	mesh->triangles = (Triangle *) malloc(nt * sizeof(Triangle));
	
	// set mesh vertices
	for (int i = 0; i < nv; i++) {
		mesh->vertices[i].x = vArr[i*3] * scale;
		mesh->vertices[i].y = vArr[i*3+1] * scale;
		mesh->vertices[i].z = vArr[i*3+2] * scale;
	}

	// set mesh triangles
	for (int i = 0; i < nt; i++) {
		mesh->triangles[i].vInds[0] = tArr[i*3];
		mesh->triangles[i].vInds[1] = tArr[i*3+1];
		mesh->triangles[i].vInds[2] = tArr[i*3+2];
	}

	// Assignment 1: 
	// Calculate and store suitable vertex normals for the mesh here.
	// Replace the code below that simply sets some arbitrary normal values	
	calculateNorms(mesh);

	mesh->next = *list;
	*list = mesh;	
}

void calculateBoundingSphere(Mesh *mesh){
	Vector min, max;
	min = max = mesh->vertices[0];
	for (int i = 0; i < mesh->nv; i++){
		if (min.x> mesh->vertices[i].x) min.x = mesh->vertices[i].x;
		if (min.y> mesh->vertices[i].y) min.y = mesh->vertices[i].y;
		if (min.z> mesh->vertices[i].z) min.z = mesh->vertices[i].z;
		if (max.x< mesh->vertices[i].x) max.x = mesh->vertices[i].x;
		if (max.y< mesh->vertices[i].y) max.y = mesh->vertices[i].y;
		if (max.z< mesh->vertices[i].z) max.z = mesh->vertices[i].z;
	}
	Vector center = { (min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2 };
	float radius = Length(Vector{ center.x - min.x, center.y - min.y, center.z-min.z});
}

#define _USE_MATH_DEFINES // To get M_PI defined
#include <iostream>
#include <math.h>
#include <stdio.h>
#include "algebra.h"

Matrix Transpose(Matrix M){
	Matrix m;
	m.e[0] = M.e[0]; m.e[4] = M.e[1]; m.e[8] = M.e[2]; m.e[12] = M.e[3];
	m.e[1] = M.e[4]; m.e[5] = M.e[5]; m.e[9] = M.e[6]; m.e[13] = M.e[7];
	m.e[2] = M.e[8]; m.e[6] = M.e[9]; m.e[10] = M.e[10]; m.e[14] = M.e[11];
	m.e[3] = M.e[12]; m.e[7] = M.e[13]; m.e[11] = M.e[14]; m.e[15] = M.e[15];
	return m;
}

HomVector Normalize(HomVector a){
	float len = sqrt(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w);
	HomVector v = { a.x / len, a.y / len, a.z / len, a.w/len };
	return v;
}

Vector CrossProduct(Vector a, Vector b) {
	Vector v = { a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x };
	return v;
}

Matrix ModelViewMatrix(float angle_x, float angle_y, float angle_z, Vector translation, Vector scaling){
	Matrix T = TranslationMatrix(translation.x, translation.y, translation.z);
	Matrix S = ScalingMatrix(scaling.x, scaling.y, scaling.z);
	Matrix result = MatMatMul(T, S);
	Matrix R;
	if (angle_x != 0){
		R = RotationMatrix(angle_x, X);
		result = MatMatMul(R, result);
	}
	if (angle_y != 0){
		R = RotationMatrix(angle_y, Y);
		result = MatMatMul(R, result);
	}
	if (angle_z != 0){
		R = RotationMatrix(angle_z, Z);
		result = MatMatMul(R, result);
	}
	return result;
}

bool operator==(Vector &v1, Vector &v2){
	if (v1.x != v2.x)
		return false;
	else if (v1.y != v2.y)
		return false;
	else if (v1.z != v2.z)
		return false;
	else
		return true;
}

float DotProduct(Vector a, Vector b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vector Subtract(Vector a, Vector b) {
	Vector v = { a.x-b.x, a.y-b.y, a.z-b.z };
	return v;
}    

Vector Add(Vector a, Vector b) {
	Vector v = { a.x+b.x, a.y+b.y, a.z+b.z };
	return v;
}    

float Length(Vector a) {
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

Vector Normalize(Vector a) {
	float len = Length(a);
	Vector v = { a.x/len, a.y/len, a.z/len };
	return v;
}

Vector ScalarVecMul(float t, Vector a) {
	Vector b = { t*a.x, t*a.y, t*a.z };
	return b;
}

HomVector MatVecMul(Matrix a, Vector b) {
	HomVector h;
	h.x = b.x*a.e[0] + b.y*a.e[4] + b.z*a.e[8] + a.e[12];
	h.y = b.x*a.e[1] + b.y*a.e[5] + b.z*a.e[9] + a.e[13];
	h.z = b.x*a.e[2] + b.y*a.e[6] + b.z*a.e[10] + a.e[14];
	h.w = b.x*a.e[3] + b.y*a.e[7] + b.z*a.e[11] + a.e[15];
	return h;
}

Vector Homogenize(HomVector h) {
	Vector a;
	if (h.w == 0.0) {
		fprintf(stderr, "Homogenize: w = 0\n");
		a.x = a.y = a.z = 9999999;
		return a;
	}
	a.x = h.x / h.w;
	a.y = h.y / h.w;
	a.z = h.z / h.w;
	return a;
}

Matrix MatMatMul(Matrix a, Matrix b) {
	Matrix c;
	int i, j, k;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			c.e[j*4+i] = 0.0;
			for (k = 0; k < 4; k++)
				c.e[j*4+i] += a.e[k*4+i] * b.e[j*4+k];
		}
	}
	return c;
}

void PrintVector(char *name, Vector a) {
	printf("%s: %6.5lf %6.5lf %6.5lf\n", name, a.x, a.y, a.z);
}

void PrintHomVector(char *name, HomVector a) {
	printf("%s: %6.5lf %6.5lf %6.5lf %6.5lf\n", name, a.x, a.y, a.z, a.w);
}

void PrintMatrix(char *name, Matrix a) { 
	int i,j;

	printf("%s:\n", name);
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			printf("%6.5lf ", a.e[j*4+i]);
		}
		printf("\n");
	}
}

Matrix TranslationMatrix(float x, float y, float z){
	Matrix m = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, x, y, z, 1 };
	return m;
}

Matrix ScalingMatrix(float x, float y, float z){
	Matrix m = {x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1};
	return m;
}

Matrix RotationMatrix(float angle, Direction dir){
	Matrix m;
	float a = angle*M_PI / 180;
	switch (dir){
	case X:
		m = { {1, 0, 0, 0, 0, cos(a), sin(a), 0, 0, -sin(a), cos(a), 0, 0, 0, 0, 1} };
		break;
	case Y:
		m = { {cos(a), 0, -sin(a), 0, 0, 1, 0, 0, sin(a), 0, cos(a), 0, 0, 0, 0, 1} };
		break;
	case Z:
		m = { {cos(a), sin(a), 0, 0, -sin(a), cos(a), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1} };
		break;
	default:
		std::cout << "Wrong input as direction, resetted matrix!" <<std::endl;
		m = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
		break;
	}
	return m;
}

Matrix PersProjectionMatrix(float near, float far, float left, float right, float bottom, float top){
	Matrix m = {2*near/(right-left), 0, 0, 0,
				0, 2*near/(top-bottom), 0, 0,
				(right + left) / (right - left), (top + bottom) / (top - bottom), -(far + near) / (far-near), -1,
				0, 0, -2 * far*near / (far-near), 0
				};
	return m;
}

Matrix ParlProjectionMatrix(float near, float far, float left, float right, float bottom, float top){
	Matrix m = {2/(right-left), 0, 0, 0,
				0, 2/(top-bottom), 0, 0,
				0, 0, -2/(near-far), 0,
				-(right + left)/(right-left), -(top+bottom)/(top-bottom), -(near+far)/(far-near), 1};
	return m;
}

float *convertToArray(const Vector &a){
	float *x = new float[3];
	x[0] = a.x; x[1] = a.y; x[2] = a.z;
	return x;
}

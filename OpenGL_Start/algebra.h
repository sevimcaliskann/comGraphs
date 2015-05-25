#ifndef _ALGEBRA_H_
#define _ALGEBRA_H_
enum Direction {X, Y, Z};
typedef struct { float x, y, z; } Vector;
typedef struct { float x, y, z, w; } HomVector;

/* Column-major order are used for the matrices here to be compatible with OpenGL.
** The indices used to access elements in the matrices are shown below.
**  _                _
** |                  |
** |   0   4   8  12  |
** |                  |
** |   1   5   9  13  |
** |                  |
** |   2   6  10  14  |
** |                  |
** |   3   7  11  15  |
** |_                _|
*/
typedef struct matrix { float e[16]; } Matrix;

Matrix Transpose(Matrix M);
Vector Add(Vector a, Vector b);
Matrix ModelViewMatrix(float angle_x, float angle_y, float angle_z, Vector translation, Vector scaling);
bool operator==(Vector &v1, Vector &v2);
Vector Subtract(Vector a, Vector b);
Vector CrossProduct(Vector a, Vector b);
float DotProduct(Vector a, Vector b);
float Length(Vector a);
Vector Normalize(Vector a);
HomVector Normalize(HomVector a);
Vector ScalarVecMul(float t, Vector a);
HomVector MatVecMul(Matrix a, Vector b);
Vector Homogenize(HomVector a);
Matrix MatMatMul(Matrix a, Matrix b);
void PrintMatrix(char *name, Matrix m);
void PrintVector(char *name, Vector v);
void PrintHomVector(char *name, HomVector h);
Matrix TranslationMatrix(float x, float y, float z);
Matrix ScalingMatrix(float x, float y, float z);
Matrix RotationMatrix(float angle, Direction dir);
Matrix PersProjectionMatrix(float near, float far, float left, float right, float bottom, float top);
Matrix ParlProjectionMatrix(float near, float far, float left, float right, float bottom, float top);

#endif


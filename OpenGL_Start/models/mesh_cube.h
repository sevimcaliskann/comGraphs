/* Definition of a triangle mesh for a cube */

struct cube_ {
	int nov;
	int nof;
	float verts[24];
	int faces[36];
};

struct cube_ cube = {

	8, /* number of vertices */

	12, /* number of faces */

	{ /* vertices */
	0.000000, 0.000000, 1.000000,
	1.000000, 0.000000, 1.000000,
	1.000000, 1.000000, 1.000000,
	0.000000, 1.000000, 1.000000,
	0.000000, 0.000000, 0.000000,
	1.000000, 0.000000, 0.000000,
	1.000000, 1.000000, 0.000000,
	0.000000, 1.000000, 0.000000
	},

	{ /* face indices */
	0, 1, 2,
	0, 2, 3,
	1, 5, 6,
	1, 6, 2,
	4, 6, 5,
	4, 7, 6,
	4, 0, 3,
	4, 3, 7,
	3, 2, 6,
	3, 6, 7,
	0, 4, 5,
	0, 5, 1
	}
};
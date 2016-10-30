#include <math.h>
#include "utils.h"

void init_matrix(float matrix[16])
{
	matrix[0] = 1.0f;
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;
 
	matrix[4] = 0.0f;
	matrix[5] = 1.0f;
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;
 
	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = 1.0f;
	matrix[11] = 0.0f;
 
	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;
}

void multiply_matrix(float matrix[16], const float matrix0[16], const float matrix1[16])
{
	int i;
	int row;
	int column;
	float temp[16];
 
	for (column = 0; column < 4; column++) {
 
		for (row = 0; row < 4; row++) {
 
			temp[column * 4 + row] = 0.0f;
 
			for (i = 0; i < 4; i++)
				temp[column * 4 + row] += matrix0[i * 4 + row] * matrix1[column * 4 + i];
		}
	}
 
	for (i = 0; i < 16; i++)
		matrix[i] = temp[i];
}

void rotate_xyz(float matrix[16], const float anglex, const float angley, const float anglez)
{
	const float pi = 3.141592f;
	float temp[16];
	float rz = 2.0f * pi * anglez / 360.0f;
	float rx = 2.0f * pi * anglex / 360.0f;
	float ry = 2.0f * pi * angley / 360.0f;
	float sy = sinf(ry);
	float cy = cosf(ry);
	float sx = sinf(rx);
	float cx = cosf(rx);
	float sz = sinf(rz);
	float cz = cosf(rz);
 
	init_matrix(temp);
 
	temp[0] = cy * cz - sx * sy * sz;
	temp[1] = cz * sx * sy + cy * sz;
	temp[2] = -cx * sy;
 
	temp[4] = -cx * sz;
	temp[5] = cx * cz;
	temp[6] = sx;
 
	temp[8] = cz * sy + cy * sx * sz;
	temp[9] = -cy * cz * sx + sy * sz;
	temp[10] = cx * cy;
 
	multiply_matrix(matrix, matrix, temp);
}

void translate_xyz(float matrix[16], const float x, const float y, const float z)
{
	init_matrix(matrix);

	matrix[3] = matrix[0] * x + matrix[4] * y + matrix[8] * z;
	matrix[7] = matrix[1] * x + matrix[5] * y + matrix[9] * z;
	matrix[11] = matrix[2] * x + matrix[6] * y + matrix[10] * z; 
}

int view_set_ortho(float result[16], const float left, const float right,
		const float bottom, const float top, const float near, const float far)
{
	if ((right - left) == 0.0f || (top - bottom) == 0.0f || (far - near) == 0.0f) return 0;
 
	result[0] = 2.0f / (right - left);
	result[1] = 0.0f;
	result[2] = 0.0f;
	result[3] = 0.0f;
	result[4] = 0.0f;
	result[5] = 2.0f / (top - bottom);
	result[6] = 0.0f;
	result[7] = 0.0f;
	result[8] = 0.0f;
	result[9] = 0.0f;
	result[10] = -2.0f / (far - near);
	result[11] = 0.0f;
	result[12] = -(right + left) / (right - left);
	result[13] = -(top + bottom) / (top - bottom);
	result[14] = -(far + near) / (far - near);
	result[15] = 1.0f;
 
	return 1;
}
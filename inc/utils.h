#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

void init_matrix(float matrix[16]);

void multiply_matrix(float matrix[16], const float matrix0[16], const float matrix1[16]);

void rotate_xyz(float matrix[16], const float anglex, const float angley, const float anglez);

void translate_xyz(float matrix[16], const float x, const float y, const float z);

int view_set_ortho(float result[16], const float left, const float right,
		const float bottom, const float top, const float near, const float far);

#ifdef __cplusplus
}
#endif

#endif
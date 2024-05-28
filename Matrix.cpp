#include "Matrix.h"

glm::mat4 RotateX(float alpha) {
	glm::mat4 M = glm::mat4(1.0f);
	M[1][1] = cosf(alpha);
	M[1][2] = -sinf(alpha);
	M[2][1] = sinf(alpha);
	M[2][2] = cosf(alpha);
	return M;
}

glm::mat4 RotateY(float alpha) {
	glm::mat4 M = glm::mat4(1.0f);
	M[0][0] = cosf(alpha);
	M[0][2] = sinf(alpha);
	M[2][0] = -sinf(alpha);
	M[2][2] = cosf(alpha);
	return M;
}

glm::mat4 RotateZ(float alpha) {
	glm::mat4 M = glm::mat4(1.0f);
	M[0][0] = cosf(alpha);
	M[0][1] = -sinf(alpha);
	M[1][0] = sinf(alpha);
	M[1][1] = cosf(alpha);
	return M;
}

glm::mat4 Scale(float scale)
{
	glm::mat4 S = glm::mat4(scale);
	S[3][3] = 1;
	return S;
}

glm::mat4 Translate(float t_x, float t_y, float t_z)
{
	glm::mat4 T = glm::mat4(1);
	T[3][0] = t_x;
	T[3][1] = t_y;
	T[3][2] = t_z;
	T[3][3] = 1;
	return T;
}
glm::quat RotateAroundAxis(float angle, glm::vec3 const& axis) {

	float const a = angle * 0.5f;
	float const s = glm::sin(a);

	return glm::quat(glm::cos(a), axis.x * s, axis.y * s, axis.z * s);
}
glm::mat4 QuaternionToMat4(const glm::quat& q) {
    float x = q.x;
    float y = q.y;
    float z = q.z;
    float w = q.w;

    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

    glm::mat4 mat;
    mat[0][0] = 1.0f - 2.0f * (yy + zz);
    mat[0][1] = 2.0f * (xy + wz);
    mat[0][2] = 2.0f * (xz - wy);
    mat[0][3] = 0.0f;

    mat[1][0] = 2.0f * (xy - wz);
    mat[1][1] = 1.0f - 2.0f * (xx + zz);
    mat[1][2] = 2.0f * (yz + wx);
    mat[1][3] = 0.0f;

    mat[2][0] = 2.0f * (xz + wy);
    mat[2][1] = 2.0f * (yz - wx);
    mat[2][2] = 1.0f - 2.0f * (xx + yy);
    mat[2][3] = 0.0f;

    mat[3][0] = 0.0f;
    mat[3][1] = 0.0f;
    mat[3][2] = 0.0f;
    mat[3][3] = 1.0f;

    return mat;
}

std::vector<glm::vec3> SolveTridiagonal(std::vector<float> a, std::vector<float> d, std::vector<float> c, std::vector<glm::vec3> R) {

	int N = d.size();
	std::vector<glm::vec3> x(N);

	std::vector<float> scratch(N);

	scratch[0] = c[0] / d[0];
	x[0] = R[0] / d[0];

	for (int i = 1; i < N; i++) {
		if (i < N - 1) {
			scratch[i] = c[i] / (d[i] - a[i] * scratch[i - 1]);
		}
		x[i] = (R[i] - a[i] * x[i - 1]) / (d[i] - a[i] * scratch[i - 1]);
	}

	for (int i = N - 2; i >= 0; i--) {
		x[i] = x[i] - scratch[i] * x[i + 1];
	}

	return x;
}
glm::mat4 E2Berstein() {
	return glm::transpose(glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f / 3.0f, 0.0f, 0.0f,
		1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	));
}
#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

glm::mat4 RotateX(float alpha);
glm::mat4 RotateY(float alpha);
glm::mat4 RotateZ(float alpha);
glm::mat4 Scale(float scale);
glm::mat4 Translate(float t_x, float t_y, float t_z);

glm::quat RotateAroundAxis(float angle, glm::vec3 const& axis);
glm::mat4 QuaternionToMat4(const glm::quat& q);

std::vector<glm::vec3> SolveTridiagonal(std::vector<float> a, std::vector<float> d, std::vector<float> c, std::vector<glm::vec3> R);

glm::mat4 E2Berstein();

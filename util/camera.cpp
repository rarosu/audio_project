#include "camera.hpp"

Camera::Camera() {}


void Camera::setProjection(const glm::mat4& projection) {
    m_projection = projection;
}

void Camera::setUp(const glm::vec3& up) {
    m_up = up;
}

void Camera::setFacing(const glm::vec3& facing) {
    m_facing = facing;
}

void Camera::setPosition(const glm::vec3& position) {
    m_position = position;
}


void Camera::commit() {
    glm::vec3 z = -glm::normalize(m_facing);
    glm::vec3 x = glm::normalize(glm::cross(m_up, z));
    glm::vec3 y = glm::cross(z, x);

    //m_view = glm::mat4(x.x,                      x.y,                      x.z,                      0,
    //                   y.x,                      y.y,                      y.z,                      0,
    //                   z.x,                      z.y,                      z.z,                      0,
    //                   -glm::dot(x, m_position), -glm::dot(y, m_position), -glm::dot(z, m_position), 1);
    m_view = glm::mat4(x.x, y.x, z.x, 0,
                       x.y, y.y, z.y, 0,
                       x.z, y.z, z.z, 0,
                       -glm::dot(x, m_position), -glm::dot(y, m_position), -glm::dot(z, m_position), 1);

    m_projectionView = m_projection * m_view;
}

glm::mat4 Camera::createPerspectiveProjection(float near, float far, float fovY, float aspect) {
    float yScale = 1.0f / std::tan(fovY * 0.5f);
    float xScale = yScale / aspect;
    float len = far - near;

    glm::mat4 perspective(xScale, 0,      0,                       0,
                          0,      yScale, 0,                       0,
                          0,      0,      -(far + near) / len,     -1,
                          0,      0,      -(2 * far * near) / len, 0);

    return perspective;
}


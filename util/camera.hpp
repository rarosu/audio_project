#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

class Camera {
public:
    Camera();

    void setProjection(const glm::mat4& projection);
    void setUp(const glm::vec3& up);
    void setFacing(const glm::vec3& facing);
    void setPosition(const glm::vec3& position);

    void commit();

    const glm::vec3& getUp() const { return m_up; }
    const glm::vec3& getPosition() const { return m_position; }
    const glm::vec3& getFacing() const { return m_facing; }
    const glm::mat4& getView() const { return m_view; }
    const glm::mat4& getProjection() const { return m_projection; }
    const glm::mat4& getProjectionView() const { return m_projectionView; }

    static glm::mat4 createPerspectiveProjection(float near, float far, float fovY, float aspect);
private:
    glm::vec3 m_up;
    glm::vec3 m_position;
    glm::vec3 m_facing;

    glm::mat4 m_view; 
    glm::mat4 m_projection;
    glm::mat4 m_projectionView;
};

#endif

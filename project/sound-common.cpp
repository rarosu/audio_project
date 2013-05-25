#include "sound-common.hpp"

glm::vec3 Listener::getRight() const {
	return glm::cross(m_facing, glm::vec3(0, 1, 0));
}

glm::vec3 Listener::getLeft() const {
	return -getRight();
}
#ifndef SOUND_COMMON_HPP
#define SOUND_COMMON_HPP

#include <glm/glm.hpp>

/** Stores the data describing a listener in the world */
struct Listener {
	glm::vec3 m_position;
	glm::vec3 m_facing;

	glm::vec3 getRight() const;
	glm::vec3 getLeft() const;
	glm::vec3 getFacing() const { return m_facing; }
};

#endif
#pragma once

#include <glm/fwd.hpp>
#include <vector>

namespace TCC
{
	class Display;
	class GridBuffer;
	class CancerBehaviour;
	class MedecineManager;

	//////////
	/// VERY DIRTY GLOBALS - Shame on me :(

	static unsigned int windowWidth = 1024;
	static unsigned int windowHeight = 768;
	static Display *displayBuffer;
	static GLuint fontTex;
	double mouse_x, mouse_y;
	bool rMouse, lMouse;
	int injectionRadius = 2;
	int injectionThickness = 4;

	int cancerPercent = 33;
	int healthyPercent = 33;

	int threadNumber = 4;
	bool running;

	GridBuffer *buffer;

	MedecineManager *medecineManager;

	std::vector<std::unique_ptr<TCC::CancerBehaviour>> cancerBehaviours;

	typedef glm::uvec2 Position;
	typedef glm::uvec3 Color;
}
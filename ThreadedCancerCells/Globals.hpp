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
	int injectionRadius = 30;
	int injectionThickness = 1;
	int cancerPercent = 30;
	int threadNumber = 4;
	bool running;

	GridBuffer *buffer1;
	GridBuffer *buffer2;
	GridBuffer *readBuf;
	GridBuffer *writeBuf;

	MedecineManager *medecineManager;

	std::vector<std::unique_ptr<TCC::CancerBehaviour>> cancerBehaviours;

	typedef glm::uvec2 Position;
	typedef glm::uvec3 Color;
}
#pragma once

#include <glm/fwd.hpp>

namespace TCC
{
	class Display;
	class GridBuffer;

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

	GridBuffer *buffer1;
	GridBuffer *buffer2;
	GridBuffer *readBuf;
	GridBuffer *writeBuf;

	typedef glm::uvec2 Position;
	typedef glm::uvec3 Color;
}
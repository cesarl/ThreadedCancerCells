#pragma once

#include "Globals.hpp"
#include <glm/glm.hpp>

namespace TCC
{
	enum CellType : unsigned char
	{
		Healthy = 0
		, Cancer = 1
		, Medecine = 2
		, END = 3
	};

	struct Cell
	{
		CellType type = Medecine;
		bool medecine = false;
		glm::i16vec2 direction;
	};

	static const unsigned char Style[9] = {
		0, 255, 0 // Healthy
		, 255, 0, 0 // Cancer
		, 0, 0, 255 }; // Medecine
}
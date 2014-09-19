#pragma once

#include <vector>
#include "Globals.hpp"
#include <glm/glm.hpp>

namespace TCC
{
	class MedecineManager
	{
		struct Medecine
		{
			glm::uvec2 dest;
			glm::uvec2 pos;
		};

		std::vector <Medecine> _collection;
	public:
		void inject(unsigned int x, unsigned int y)
		{
		}

		void update()
		{
			for (auto &e : )
		}
	private:
	};
}
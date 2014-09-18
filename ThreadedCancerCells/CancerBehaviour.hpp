#pragma once

#include "GridBuffer.hpp"
#include "Globals.hpp"

namespace TCC
{
	class CancerBehaviour
	{
	public:
		CancerBehaviour() = delete;
		CancerBehaviour(const CancerBehaviour &) = delete;

		CancerBehaviour(unsigned int from, unsigned int to)
		{
			for (auto i = from; i < to; ++i)
			{
				auto r = readBuf->computeCancer(i % TCC::windowWidth, i / TCC::windowWidth);
				writeBuf->setCell(i, r);
			}
		}
	};
}
#pragma once

#include "GridBuffer.hpp"
#include "Globals.hpp"
#include "ThreadQueue.hpp"
#include <array>

namespace TCC
{
	class CancerBehaviour : public ThreadQueue
	{
	public:
		struct Compute : TMQ::FutureData<std::array<unsigned int, 3>>
		{
			Compute(unsigned int _from, unsigned int _to)
				: from(_from)
				, to(_to)
			{}
			unsigned int from;
			unsigned int to;
		};

		virtual bool _update()
		{
			bool result = true;
			_commandQueue.getDispatcher()
				.handle<Compute>([&](Compute& msg)
			{
				for (auto i = msg.from; i < msg.to; ++i)
				{
					auto r = readBuf->computeCancer(i % TCC::windowWidth, i / TCC::windowWidth);
					writeBuf->setCell(i, r);
				}
				std::array<unsigned int, 3> t{ { 1, 2, 3 } };
				msg.result.set_value(t);
			})
				.handle<TMQ::CloseQueue>([&](TMQ::CloseQueue &msg){
				result = false;
			});
			return result;
		}

		virtual bool _init()
		{
			return true;
		}

		virtual bool _initInNewThread()
		{
			return true;
		}

		virtual bool _release()
		{
			return true;
		}

		virtual bool _releaseInNewThread()
		{
			return true;
		}

		CancerBehaviour()
		{
		}
	};
}
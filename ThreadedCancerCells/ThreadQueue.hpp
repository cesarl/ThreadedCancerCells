#pragma once

#include <thread>
#include "Vendors/tmq/queue.hpp"
#include "Vendors/tmq/templateDispatcher.hpp"

namespace TCC
{
	class ThreadQueue
	{
	protected:
		std::thread _thread;
		TMQ::Queue _commandQueue;
		virtual bool _update() = 0;
		virtual bool _init() = 0;
		virtual bool _initInNewThread() = 0;
		virtual bool _release() = 0;
		virtual bool _releaseInNewThread() = 0;
		std::atomic_bool _run;

		bool update()
		{
			bool run = true;
			run = _initInNewThread();
			while (_run && run)
			{
				run = _update();
			}
			return _releaseInNewThread();
		}

	public:
		ThreadQueue(const ThreadQueue &) = delete;
		ThreadQueue(ThreadQueue &&) = delete;
		ThreadQueue& operator=(const ThreadQueue &) = delete;
		ThreadQueue& operator=(ThreadQueue &&) = delete;

		ThreadQueue()
		{}

		virtual ~ThreadQueue()
		{
		}

		bool launch()
		{
			_run = true;
			auto res = _init();
			_commandQueue.launch();
			_thread = std::thread(&ThreadQueue::update, std::ref(*this));
			return res;
		}

		void quit()
		{
			if (!_thread.joinable())
				return;
			_commandQueue.autoPriorityEmplace<TMQ::CloseQueue>();
			_run = false;
			_thread.join();
			_release();
		}

		TMQ::Queue &getCommandQueue()
		{
			return _commandQueue;
		}
	};
}
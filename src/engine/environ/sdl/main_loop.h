#pragma once
#include <memory>
#include <functional>
#include <chrono>
#include "src/basic_types.h"

namespace engine::sdl
{
	class EnvEvents;

	class MainLoop {
	public:
		using UpdateCallback = std::function<void(number dt, number ts)>;

		MainLoop();
		~MainLoop();

		number timeStamp() const;
		void start();
		void terminate();

		void flexStep(UpdateCallback&& fn);
		void fixedStep(UpdateCallback&& fn);

		EnvEvents& manageEvents();
	private:
		struct Impl;
		std::unique_ptr<Impl> _impl = {};
		std::chrono::time_point<std::chrono::steady_clock> prevTick;
	private:
		static void tickOuter(void *arg);
		void tick();
	};
}
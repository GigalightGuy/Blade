#pragma once

#include <spdlog/spdlog.h>

#include <memory>

namespace BladeEngine 
{

	class Log
	{
	public:
		static void Init();

		inline static const std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static const std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

#ifdef SKR_BUILD_DEBUG

#define BLD_CORE_TRACE(...)       BladeEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define BLD_CORE_DEBUG(...)       BladeEngine::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define BLD_CORE_INFO(...)        BladeEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define BLD_CORE_WARN(...)        BladeEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define BLD_CORE_ERROR(...)       BladeEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define BLD_CORE_CRITICAL(...)    BladeEngine::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define BLD_TRACE(...)            BladeEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define BLD_DEBUG(...)            BladeEngine::Log::GetClientLogger()->debug(__VA_ARGS__)
#define BLD_INFO(...)             BladeEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define BLD_WARN(...)             BladeEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define BLD_ERROR(...)            BladeEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define BLD_CRITICAL(...)         BladeEngine::Log::GetClientLogger()->critical(__VA_ARGS__)

#else

#define BLD_CORE_TRACE(...)
#define BLD_CORE_DEBUG(...)
#define BLD_CORE_INFO(...)
#define BLD_CORE_WARN(...)
#define BLD_CORE_ERROR(...)
#define BLD_CORE_CRITICAL(...)

#define BLD_TRACE(...)
#define BLD_DEBUG(...)
#define BLD_INFO(...)
#define BLD_WARN(...)
#define BLD_ERROR(...)
#define BLD_CRITICAL(...)

#endif // BLD_BUILD_DEBUG
#pragma once
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace fpgfx
{
    class Log
    {
    private:
        static std::shared_ptr<spdlog::async_logger> s_Logger;

    public:
        Log() = default;
        static void Init();
        static auto GetLogger() -> std::shared_ptr<spdlog::async_logger> & { return s_Logger; }
    };
}

#define fpgfx_trace(...) ::fpgfx::Log::GetLogger()->trace(__VA_ARGS__)
#define fpgfx_debug(...) ::fpgfx::Log::GetLogger()->debug(__VA_ARGS__)
#define fpgfx_info(...) ::fpgfx::Log::GetLogger()->info(__VA_ARGS__)
#define fpgfx_warn(...) ::fpgfx::Log::GetLogger()->warn(__VA_ARGS__)
#define fpgfx_error(...) ::fpgfx::Log::GetLogger()->error(__VA_ARGS__)
#define fpgfx_critical(...) ::fpgfx::Log::GetLogger()->critical(__VA_ARGS__)

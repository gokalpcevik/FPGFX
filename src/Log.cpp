#include "Log.h"

namespace fpgfx
{
    std::shared_ptr<spdlog::async_logger> Log::s_Logger;

    void Log::Init()
    {
        spdlog::init_thread_pool(8192, 2);

        std::vector<spdlog::sink_ptr> logSinks;
        auto stdoutSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        logSinks.emplace_back(stdoutSink);

        logSinks[0]->set_pattern("%T [%n] %^%v%$ ");

        s_Logger = std::make_shared<spdlog::async_logger>("FPGFX", std::begin(logSinks), std::end(logSinks), spdlog::thread_pool());
        spdlog::register_logger(s_Logger);
        s_Logger->set_level(spdlog::level::trace);
        s_Logger->flush_on(spdlog::level::trace);
    }
}
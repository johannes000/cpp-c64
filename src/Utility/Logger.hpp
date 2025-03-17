#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>

inline std::shared_ptr<spdlog::logger> AddLogger(const std::string &name, spdlog::level::level_enum level = spdlog::level::info) {
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/" + name + ".log");
	auto logger = std::make_shared<spdlog::logger>(name, console_sink);

	logger->set_level(level);

	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%^%l%$] %v");

	spdlog::register_logger(logger);
	logger->info("{} Logger Added", name);

	return logger;
}
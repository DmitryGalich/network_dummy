#pragma once

#include <filesystem>
#include <string>
#include <thread>

#define LOG_INFO(text) ::info(text)
#define LOG_WARNING(text) ::warning(text)
#define LOG_ERROR(text) ::error(text)

namespace
{
    void set_path(const std::filesystem::path &path);
    void clear_path();

    void set_application_name(const std::string &app_name);
    void clear_application_name();

    void info(const std::string &message, const std::string function_prefix = "");
    void warning(const std::string &message,
                 const std::string function_prefix = "");
    void error(const std::string &message, const std::string function_prefix = "");

} // namespace logger

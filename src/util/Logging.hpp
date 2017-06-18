#pragma once

#ifndef CRYPTO_UTIL_LOGGING_H
#define CRYPTO_UTIL_LOGGING_H

#include <util/Strings.hpp>

#include <boost/log/common.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/expressions/attr.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/detail/header.hpp>


namespace crypto
{
enum class LoggingSeverity
{
    INFO = 0x0,
    DEBUG = 0x1,
    WARNING = 0x2,
    ERROR = 0x3,
    FATAL = 0x4
};

inline std::ostream& operator<<(std::ostream& s, const LoggingSeverity& severity)
{
    switch (severity)
    {
    case LoggingSeverity::INFO:
        s << "INFO";
        break;
    case LoggingSeverity::DEBUG:
        s << "DEBUG";
        break;
    case LoggingSeverity::WARNING:
        s << "WARNING";
        break;
    case LoggingSeverity::ERROR:
        s << "ERROR";
        break;
    case LoggingSeverity::FATAL:
        s << "FATAL";
        break;
    default: s << "UNKNOWN";
    }

    return s;
}

namespace log
{
BOOST_LOG_ATTRIBUTE_KEYWORD(line_number, "Line", int);
BOOST_LOG_ATTRIBUTE_KEYWORD(file_name, "File", std::string);
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", LoggingSeverity);
}

#define CRYPTO_LOG_LOCATION do { \
    boost::log::attribute_cast<boost::log::attributes::mutable_constant<int>>( \
        boost::log::core::get()->get_global_attributes()["Line"] \
    ).set(__LINE__); \
    boost::log::attribute_cast<boost::log::attributes::mutable_constant<std::string>>( \
        boost::log::core::get()->get_global_attributes()["File"] \
    ).set(__FILENAME__); \
    } while(false)

#define CRYPTO_LOG_SEV(logger, severity) CRYPTO_LOG_LOCATION; \
    BOOST_LOG_SEV(logger, severity)
#define CRYPTO_LOG(logger) CRYPTO_LOG_LOCATION; \
    BOOST_LOG(logger)
#define CRYPTO_LOG_FATAL(logger) CRYPTO_LOG_SEV(logger, crypto::LoggingSeverity::FATAL)
#define CRYPTO_LOG_ERROR(logger) CRYPTO_LOG_SEV(logger, crypto::LoggingSeverity::ERROR)
#define CRYPTO_LOG_WARNING(logger) CRYPTO_LOG_SEV(logger, crypto::LoggingSeverity::WARNING)
#define CRYPTO_LOG_DEBUG(logger) CRYPTO_LOG_SEV(logger, crypto::LoggingSeverity::DEBUG)
#define CRYPTO_LOG_INFO(logger) CRYPTO_LOG_SEV(logger, crypto::LoggingSeverity::INFO)

typedef boost::log::sources::severity_channel_logger_mt<LoggingSeverity, std::string> Logger;

/**
 * Defines a global logger that may be used when a class context is not available, or when
 * local logger objects are not an option. The channel will always appear as Global.
 */
BOOST_LOG_INLINE_GLOBAL_LOGGER_CTOR_ARGS(g_logger, Logger, (boost::log::keywords::channel = "Global"));

/**
 * This function initializes the logging backend and must be called before anything else of the
 * library is used.
 */
inline void initLogging()
{
    namespace expr = boost::log::expressions;
    namespace keywords = boost::log::keywords;
    namespace log = boost::log;

    auto core = log::core::get();

    auto textBackend = boost::make_shared<log::sinks::text_file_backend>
    (
        keywords::file_name = "crypto_%Y-%m-%d_%N.log",
        keywords::rotation_size = 10 * 1024 * 1024, // TODO: Make this configurable
        keywords::open_mode = std::ios_base::app,
        keywords::time_based_rotation = log::sinks::file::rotation_at_time_point(0, 0, 0)
    );

    textBackend->set_file_collector(log::sinks::file::make_collector(
        keywords::target = "logs",
        keywords::max_files = 64 // TODO: Make this configurable
    ));

    textBackend->scan_for_files();

    typedef log::sinks::synchronous_sink<log::sinks::text_file_backend> sink_t;
    boost::shared_ptr<sink_t> sink(new sink_t(textBackend));

    sink->set_formatter(
        expr::stream
        << "<"
        << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
        << "> [" << expr::attr<LoggingSeverity>("Severity") << "] ["
        << expr::attr<std::string>("Channel") << "] "
        << "(" << crypto::log::file_name << ", Line "
        << crypto::log::line_number << "): "
        << expr::smessage
    );

    core->add_sink(sink);

    log::add_console_log
    (
        std::cout,
        keywords::format =
        (
            expr::stream
            << "<"
            << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
            << "> [" << expr::attr<LoggingSeverity>("Severity") << "] ["
            << expr::attr<std::string>("Channel") << "] "
            << "(" << crypto::log::file_name << ", Line "
            << crypto::log::line_number << "): "
            << expr::smessage
        )
    );

    log::add_common_attributes();
    core->add_global_attribute("Line", log::attributes::mutable_constant<int>(0));
    core->add_global_attribute("File", log::attributes::mutable_constant<std::string>(""));

    core->set_filter(crypto::log::severity >= static_cast<LoggingSeverity>(0)); // TODO: Make this configurable
}
}

#endif // CRYPTO_UTIL_LOGGING_H

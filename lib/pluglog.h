#ifndef PLUGLOG_H
#define PLUGLOG_H
#include <string>
#include <sstream>

#ifndef __ANDROID__
#include <iostream>
#endif

#ifdef __ANDROID__
#include <android/log.h>
#endif

inline char separator()
{
#ifdef _WIN32
    return '\\';
#else
    return '/';
#endif
}

class Plog{
private:
    Plog() = delete;
    Plog(const Plog&) = delete;
    Plog(Plog&&) = default;
public:
    enum class LogPriority{
        /** For internal use only.  */
        UNKNOWN,
        /** The default priority, for internal use only.  */
        DEFAULT,
        /** Verbose logging. Should typically be disabled for a release apk. */
        VERBOSE,
        /** Debug logging. Should typically be disabled for a release apk. */
        DEBUG,
        /** Informational logging. Should typically be disabled for a release apk. */
        INFO,
        /** Warning logging. For use with recoverable failures. */
        WARN,
        /** Error logging. For use with unrecoverable failures. */
        ERROR,
        /** Fatal logging. For use when aborting. */
        FATAL,
        /** For internal use only.  */
        SILENT, /* only for SetMinPriority(); must be last */
    };

    static void log(const LogPriority priority, const std::string& tag, const std::string& s) {

// Android only
#ifdef __ANDROID__
        switch (priority) {
        case LogPriority::DEBUG:
            __android_log_print(ANDROID_LOG_DEBUG, tag.c_str(), ": %s", s.c_str());
            break;
        case LogPriority::INFO:
            __android_log_print(ANDROID_LOG_INFO, tag.c_str(), ": %s", s.c_str());
            break;
        case LogPriority::WARN:
            __android_log_print(ANDROID_LOG_WARN, tag.c_str(), ": %s", s.c_str());
            break;
        case LogPriority::ERROR:
            __android_log_print(ANDROID_LOG_ERROR, tag.c_str(), ": %s", s.c_str());
        default:
            break;
        }

// Anything but Android
#else
        switch (priority) {
            case LogPriority::UNKNOWN:
            case LogPriority::DEFAULT:
            case LogPriority::VERBOSE:
            case LogPriority::DEBUG:
            case LogPriority::INFO:
            case LogPriority::WARN:
                std::cout<< tag <<": " << s <<std::endl;
                break;
            case LogPriority::ERROR:
            case LogPriority::FATAL:
                std::cerr<< tag <<": " << s <<std::endl;
                break;
            case LogPriority::SILENT:
                break;

        }

#endif

    }
};

#endif // PLUGLOG_H

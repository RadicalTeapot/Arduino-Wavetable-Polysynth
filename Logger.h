#ifndef LOGGER_H
#define LOGGER_H

const size_t MAX_BUFFER_SIZE = 256;

class Logger {
  public:
    enum LogLevel {
      Debug = 0U,
      Info,
      Warning,
      Error,
    };
    
    static void trace(const char* file, int line) {
      snprintf(print_buffer, MAX_BUFFER_SIZE, "TRACE File: %s  Line: %d", file, line);
      println(logLevel);
    }

    static void logDebug(const char* fmt, ...) {
      va_list args;
      va_start(args, fmt);
      log(LogLevel::Debug, fmt, args);
      va_end(args);
    }

    static void logInfo(const char* fmt, ...) {
      va_list args;
      va_start(args, fmt);
      log(LogLevel::Info, fmt, args);
      va_end(args);
    }

    static void logWarning(const char* fmt, ...) {
      va_list args;
      va_start(args, fmt);
      log(LogLevel::Warning, fmt, args);
      va_end(args);
    }

    static void logError(const char* fmt, ...) {
      va_list args;
      va_start(args, fmt);
      log(LogLevel::Error, fmt, args);
      va_end(args);
    }

    static void setLogLevel(LogLevel level) {
      logLevel = level;
    }

  private:
    static char print_buffer[MAX_BUFFER_SIZE];
    static LogLevel logLevel;

    static void log(LogLevel level, const char* fmt, va_list args) {
      if (level < logLevel) return;
      vsnprintf(print_buffer, MAX_BUFFER_SIZE, fmt, args); // Use vsnprintf for variadic arguments
      println(level);
    }

    static const char* levelName(LogLevel level) {
      switch (level) {
        case LogLevel::Debug:
          return "[DEBUG   ]";
        case LogLevel::Info:
          return "[INFO   ] ";
        case LogLevel::Warning:
          return "[WARNING] ";
        case LogLevel::Error:
          return "[ERROR  ] ";
      }
      return "[]";
    }
    
    static void println(LogLevel level) {
      Serial.print(levelName(level));
      Serial.println(print_buffer);
      print_buffer[0] = '\0';
    }
};

char Logger::print_buffer[MAX_BUFFER_SIZE]; // Define the static member outside the class definition
Logger::LogLevel Logger::logLevel;

#if defined(NO_TRACE)
#  define TRACE()
#else
#  define TRACE() { Logger::trace( __FILE__, __LINE__); }
#endif

#endif /* LOGGER_H */
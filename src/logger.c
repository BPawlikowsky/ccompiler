#include "logger.h"

typedef enum LogType { TRACE, WARN, ERROR, DEBUG, INFO } LogType;

void logger(va_list args, const char *msg, LogType type) {
  time_t current_time;
  time(&current_time);
  struct tm *time_info;
  time_info = localtime(&current_time);
  // char *time_msg = asctime(time_info);
  char time_msg[32];
  sprintf(time_msg, "%.2d/%.2d/%.4d %.2d:%.2d:%.2d", time_info->tm_mday,
          time_info->tm_mon + 1, 1900 + time_info->tm_year, time_info->tm_hour,
          time_info->tm_min, time_info->tm_sec);

  char tmp[32] = "";

  strncpy(tmp, time_msg, strlen(time_msg));

  char *type_string;

  switch (type) {
  case TRACE:
    type_string = "TRACE";
    break;
  case WARN:
    type_string = "WARN";
    break;
  case ERROR:
    type_string = "ERROR";
    break;
  case DEBUG:
    type_string = "DEBUG";
    break;
  case INFO:
  default:
    type_string = "INFO";
    break;
  }

  printf("%s [%s]: ", type_string, tmp);

  vprintf(msg, args);
  printf("\n");
}

void log_trace(const char *msg, ...) {
  va_list args;
  va_start(args, msg);

  logger(args, msg, TRACE);

  va_end(args);
}

void log_error(const char *msg, ...) {
  va_list args;
  va_start(args, msg);

  logger(args, msg, ERROR);

  va_end(args);
}

void log_warn(const char *msg, ...) {
  va_list args;
  va_start(args, msg);

  logger(args, msg, WARN);

  va_end(args);
}

void log_debug(const char *msg, ...) {
  va_list args;
  va_start(args, msg);

  logger(args, msg, DEBUG);

  va_end(args);
}

void log_info(const char *msg, ...) {
  va_list args;
  va_start(args, msg);

  logger(args, msg, INFO);

  va_end(args);
}

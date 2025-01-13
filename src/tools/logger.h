#include "../external.h"
#define LOG_DEBUG
#define LOG_TRACE
#define LOG_ERROR

void log_trace(const char *msg, ...);
void log_error(const char *msg, ...);
void log_warn(const char *msg, ...);
void log_debug(const char *msg, ...);
void log_info(const char *msg, ...);
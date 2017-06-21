#ifndef LOGGING_H_
#define LOGGING_H_

/**
 * @brief Write information into a log file
 *
 * logging.h / logging.cpp
 *     There are APIs to open, read and write log information to a specified file, “gl.log”.
 *     There are specific APIs for each purpose, such as normal log, error report and from shader program.
 *
 */
namespace gl
{
/**
 * @brief   Restart the Log file
 *          Open a file named "gl.log" in the same directory with executable file.
 *          This call erases the previous log data and writes current time information.
 *          File handler will be closed internally.
 */
void RestartLog();

/**
 * @brief   Write specified message to log file
 *          Write a log message to the specified log file, gl.log.
 *          This api adds prefix, "[GL LOG]" in front of the message when writes.
 *          The usage is same with the C default printf function.
 *
 * @param message     format and data
 */
void Log(const char* message, ...);

/**
 * @brief   Write error message to log file
 *          Write a error log message to the specified log file, gl.log and stderr file.
 *          This api adds prefix, "[GL ERROR]" in front of the message when writes.
 *          The usage is same with the C default printf function.
 *
 * @param message     format and data
 */
void LogError(const char* message, ...);

/**
 * @brief   Log installed OpenGl Version information and supported renderer's version
 *
 */
void LogGlVersionInfo();

/**
 * @brief   Log a string with any error information from shader into log file.
 *
 * @param shaderObject     The shader object to investigate shader errors
 */
void LogShaderErrorInfo( GLuint shaderObject );

/**
 * @brief   Log a string with any information from link and validate into log file.
 *
 * @param programObject     The program object to get information from link
 */
void LogShaderProgramInfo( GLuint programObject );

} /// namespace gl

#endif  /// LOGGING_H_

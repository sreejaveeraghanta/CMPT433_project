#include "util.h"

#include "PVRUtils/OpenGLES/BindingsGles.h"

#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>

void debugLog(const char* format, ...)
{
    // Get the current time
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);

    // Create a time string in the format YYYY-MM-DD HH:MM:SS
    std::ostringstream time_stream;
    time_stream << std::put_time(local_time, "%H:%M:%S");
    std::string time_str = time_stream.str();

    // Print the prefix with the timestamp
    std::printf("[%s][Debug] ", time_str.c_str());

    // Handle the variable arguments
    va_list args;
    va_start(args, format);
    std::vprintf(format, args);
    va_end(args);

    // Print a newline character
    std::printf("\n");
}

void convertRGBAtoRGB565(uint8_t *rgba, uint16_t *rgb565, int width, int height)
{
    int numPixels = width * height;
    for (int i = 0; i < numPixels; i++)
    {
        // Extract red, green, blue components from RGBA.
        uint8_t r = rgba[i * 4 + 0];
        uint8_t g = rgba[i * 4 + 1];
        uint8_t b = rgba[i * 4 + 2];
        // Alpha is ignored in RGB565.

        // Convert 8-bit components to RGB565:
        // Red:   5 bits, Green: 6 bits, Blue: 5 bits.
        uint16_t r5 = (r >> 3) & 0x1F;  // 5 bits
        uint16_t g6 = (g >> 2) & 0x3F;  // 6 bits
        uint16_t b5 = (b >> 3) & 0x1F;  // 5 bits

        // Pack into a 16-bit value in RGB565 format.
        rgb565[i] = (r5 << 11) | (g6 << 5) | (b5);
    }

}

bool testEGLError(const char* functionLastCalled)
{
	//	eglGetError returns the last error that occurred using EGL, not necessarily the status of the last called function. The user has to
	//	check after every single EGL call or at least once every frame. Usually this would be for debugging only, but for this example
	//	it is enabled always.
	EGLint lastError = egl::GetError();
	if (lastError != EGL_SUCCESS)
	{
		printf("%s failed (%x).\n", functionLastCalled, lastError);
		return false;
	}
	return true;
}

bool testGLError(const char* functionLastCalled)
{
	//	glGetError returns the last error that occurred using OpenGL ES, not necessarily the status of the last called function. The user
	//	has to check after every single OpenGL ES call or at least once every frame. Usually this would be for debugging only, but for this
	//	example it is enabled always

	GLenum lastError = gl::GetError();
	if (lastError != GL_NO_ERROR)
	{
		printf("%s failed (%x).\n", functionLastCalled, lastError);
		return false;
	}

	return true;
}
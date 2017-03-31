// Created by Margherita Donnici on 12/27/16.

#include "Utils.h"
#include <SDL2/SDL_timer.h>
#include <sstream>
#include "point2.h"


float utils::random_float() {
    return (float) rand()/(float)RAND_MAX;
}

float utils::get_elapsed_seconds() {
    return SDL_GetTicks()/1000.f;
}

float utils::distance(const Point3 &A, const Point3 &B) {
    return (A - B).modulo();
}

float utils::radiansToDegrees(const float angleRadians) {
    return angleRadians * 180.f / (float) M_PI;
}

void utils::setCoordToPixel(const Point2& screenSize) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-1.f, -1.f, 0.f);
    glScalef(2.f / screenSize.X(), 2.f / screenSize.Y(), 1.f);
	utils::checkGLError(__FILE__, __LINE__);
}

GLenum utils::checkGLError(const char* file, int line)
{
	GLenum errorCode =  glGetError();
	while (errorCode != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM: error = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE: error = "INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION: error = "INVALID_OPERATION";
			break;
		case GL_STACK_OVERFLOW: error = "STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW: error = "STACK_UNDERFLOW";
			break;
		case GL_OUT_OF_MEMORY: error = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		default: 
			assert(false);
			break;
		}
		std::stringstream ss;
		ss << error << " | " << file << " (" << line << ")" << std::endl;
		const auto errorstring = ss.str();
		printf("%s", errorstring.c_str());
		errorCode = glGetError();
	}
	return errorCode;
}

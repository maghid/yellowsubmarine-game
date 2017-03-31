#include "Textures.h"

#include <GL/glu.h>
#include "Utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cassert>

GLuint Texture::Font = 0;
GLuint Texture::WaterBackground = 0;
GLuint Texture::SubmarineHull = 0;
GLuint Texture::SubmarineMotor = 0;
GLuint Texture::SubmarineConningTower = 0;
GLuint Texture::Barrels = 0;
GLuint Texture::Jellyfish = 0;
GLuint Texture::Sand = 0;
GLuint Texture::Shark = 0;
GLuint Texture::Sonar = 0;
GLuint Texture::Caustics = 0;
GLuint Texture::PersonalPicture = 0;
GLuint Texture::Seaweed = 0;
GLuint Texture::Rock = 0;


bool Texture::Load(/*out*/ GLuint &textureID, const char *filename) {
	assert(filename);
	assert(!glIsTexture(textureID));
	SDL_Surface * const s = IMG_Load(filename);
	if (!s) {
        printf("Texture not found: %s", filename);
        return false;
    }
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	gluBuild2DMipmaps(
		GL_TEXTURE_2D,
		GL_RGB,
		s->w, s->h,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		s->pixels);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);

	SDL_FreeSurface(s);
	utils::checkGLError(__FILE__, __LINE__);
	return true;
}

bool Texture::LoadAllTextures()
{
	if (!Load(WaterBackground, "../assets/textures/underwater.jpg")) return false;
	if (!Load(SubmarineHull,  "../assets/textures/corpo.jpg")) return false;
	if (!Load(SubmarineMotor, "../assets/textures/motor.jpg")) return false;
	if (!Load(SubmarineConningTower, "../assets/textures/box.jpg")) return false;
	if (!Load(Barrels, "../assets/textures/barrels.jpg")) return false;
	if (!Load(Jellyfish, "../assets/textures/jellyfish.png")) return false;
    if (!Load(Sand, "../assets/textures/sand.jpg")) return false;
    if (!Load(Shark, "../assets/textures/shark.jpg")) return false;
    if (!Load(Sonar, "../assets/textures/sonar2.jpg")) return false;
    if (!Load(Caustics, "../assets/textures/Caustics1.png")) return false;
	if (!Load(PersonalPicture, "../assets/textures/fotopersonale.jpg")) return false;
    if (!Load(Seaweed, "../assets/textures/seaweed.png")) return false;
	if (!Load(Rock, "../assets/textures/rock.jpg")) return false;

	return true;
}

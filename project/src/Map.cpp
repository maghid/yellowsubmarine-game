// Created by Margherita Donnici on 12/29/16.

#include "Map.h"
#include "Textures.h"
#include "Utils.h"
#include "Game.h"

// Variabili di tipo mesh
static Mesh rockMesh((char *) "../assets/obj/rock.obj");


void Map::drawSand(const Point3& subPosition) const {

  glBindTexture(GL_TEXTURE_2D, Texture::Sand);
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.f, 1.f, 1.f);
  glEnable(GL_LIGHTING);

  // Imposto le proprietà del materiale
  {
    float specular[4] = {0.f, 0.f, 0.f, 1.f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    float diffuse[4] = {0.f, 0.f, 0.f, 1.f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    float ambient[4] = {0.0f, 0.0f, 0.0f, 1.f};
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialf(GL_FRONT, GL_SHININESS, 32.f);
  }

  // Terreno a basso dettaglio
  drawPlane(terrainWidth, /*quadsPerSide*/ 50, /*tileSize*/ 5.f);

  // Terreno ad alto dettaglio
  drawPlane(terrainWidth/10.f, /*quadCount*/ 150, /*tileSize*/ 5.f, Point3{subPosition.X(), 0.05f, subPosition.Z()});

  glDisable(GL_TEXTURE_2D);
  utils::checkGLError(__FILE__, __LINE__);
}

void Map::drawPlane(const float width, const int quadsPerSide, const float tileSize, const Point3& center) const {

	// Reference: http://www.glprogramming.com/red/chapter09.html#name7
    // la funzione di generazione è una combinazione lineare delle coordinate dei vertici; piano di riferimento in coord mondo
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	float sLinearFactors[] = {1.f/tileSize, 0.f, 0.f, 0.f};
	glTexGenfv(GL_S, GL_OBJECT_PLANE, sLinearFactors); // parametri per la funzione di generazione della texture
	glEnable(GL_TEXTURE_GEN_S);

	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	float tLinearFactors[] = {0.f, 0.f, 1.f/tileSize, 0.f};
	glTexGenfv(GL_T, GL_OBJECT_PLANE, tLinearFactors);
	glEnable(GL_TEXTURE_GEN_T);

	  // Disegno quadCount x quadCount quad
	  glBegin(GL_QUADS);
	  {
		const float cx = center.X();
		const float cy = center.Y();
		const float cz = center.Z();
		glNormal3f(0.0f, 1.0f, 0.0f); // normale verticale uguale per tutti
		for (int x = 0; x < quadsPerSide; x++)
		  for (int z = 0; z < quadsPerSide; z++) {
			float x0 = -width + 2 * (x + 0) * width / quadsPerSide;
			float x1 = -width + 2 * (x + 1) * width / quadsPerSide;
			float z0 = -width + 2 * (z + 0) * width / quadsPerSide;
			float z1 = -width + 2 * (z + 1) * width / quadsPerSide;
			glVertex3f(x0 + cx, cy, z0 + cz);
			glVertex3f(x1 + cx, cy, z0 + cz);
			glVertex3f(x1 + cx, cy, z1 + cz);
			glVertex3f(x0 + cx, cy, z1 + cz);
		  }
	  }
	  glEnd();
	  glDisable(GL_TEXTURE_GEN_S);
	  glDisable(GL_TEXTURE_GEN_T);
	  utils::checkGLError(__FILE__, __LINE__);
}

void Map::drawCaustics(const Game& game) const {
  glPushMatrix();
  {
	  // Reference:
	  // https://www.opengl.org/archives/resources/faq/technical/transparency.htm

    glBindTexture(GL_TEXTURE_2D, Texture::Caustics); // applico la texture
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.f, 1.f, 1.f, 0.1f);

    const auto elapsedSeconds = game.getGameSeconds();
    const Point2 texCoordOffset{elapsedSeconds * waterPanSpeed,
                                elapsedSeconds * waterPanSpeed};

    static constexpr float planeVerticalOffset = 0.1f;

	// Reference: http://www.glprogramming.com/red/chapter09.html#name8

    // Caustics plane 1
    {
      glTranslatef(0.f, planeVerticalOffset, 0.f); // Draw the caustics above the sand
      glMatrixMode(GL_TEXTURE);
      glPushMatrix();
      {

        const Point2 offset = -texCoordOffset;
        glTranslatef(offset.X(), offset.Y(), 0.f);
        drawPlane(terrainWidth, /*quadCount*/ 75, 5.f);
      }
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
    }

    // Caustics plane 2
    {
      glTranslatef(0.f, planeVerticalOffset, 0.f); // Sort back to front

	  glMatrixMode(GL_TEXTURE);
	  glPushMatrix();
	  {
      const Point2 fixedTextureOffset{0.34f, 0.23f};
	  const Point2 offset = texCoordOffset/2.f + fixedTextureOffset;
	  glTranslatef(offset.X(), offset.Y(), 0.f);
      drawPlane(terrainWidth, /*quadsPerSide*/ 75, 6.f);
	  }
	  glPopMatrix();
	  glMatrixMode(GL_MODELVIEW);
    }

    // Caustics plane 3
    {
      glTranslatef(0.f, planeVerticalOffset, 0.f); // Sort back to front
      // offset costante per evitare il momento in cui le tre texture hanno le
      // stesse coordinate
      glMatrixMode(GL_TEXTURE);
      glPushMatrix();
      {
        const Point2 fixedTextureOffset{0.87f, 0.12f};
		const Point2 offset = texCoordOffset * Point2{-1.f, 1.f} + fixedTextureOffset;
        glTranslatef(offset.X(), offset.Y(), 0.f);
        drawPlane(terrainWidth, /*quadsPerSide*/ 75, 4.f);
      }
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_FOG);
    glColor3f(1.f, 1.f, 1.f);
  }
  glPopMatrix();
  utils::checkGLError(__FILE__, __LINE__);
}

void drawCubeFill(const float size) {
  const float S = size / 2.f;

  glBegin(GL_QUADS);

  glNormal3f(0, 0, +1);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(+S, +S, +S);
  glTexCoord2f(1.f, 0.0f);
  glVertex3f(-S, +S, +S);
  glTexCoord2f(1.f, 1.f);
  glVertex3f(-S, -S, +S);
  glTexCoord2f(0.0f, 1.f);
  glVertex3f(+S, -S, +S);

  glNormal3f(0, 0, -1);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(+S, -S, -S);
  glTexCoord2f(1.f, 1.0f);
  glVertex3f(-S, -S, -S);
  glTexCoord2f(1.f, 0.f);
  glVertex3f(-S, +S, -S);
  glTexCoord2f(0.0f, 0.f);
  glVertex3f(+S, +S, -S);

  // Top
  glNormal3f(0, +1, 0);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(+S, +S, +S);
  glTexCoord2f(1.f, 0.0f);
  glVertex3f(-S, +S, +S);
  glTexCoord2f(1.f, 1.f);
  glVertex3f(-S, +S, -S);
  glTexCoord2f(0.0f, 1.f);
  glVertex3f(+S, +S, -S);

  glNormal3f(+1, 0, 0);
  glTexCoord2f(0.0f, 0.f);
  glVertex3f(+S, +S, +S);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(+S, -S, +S);
  glTexCoord2f(1.f, 1.0f);
  glVertex3f(+S, -S, -S);
  glTexCoord2f(1.f, 0.f);
  glVertex3f(+S, +S, -S);

  glNormal3f(-1, 0, 0);
  glTexCoord2f(0.0f, 0.f);
  glVertex3f(-S, +S, -S);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-S, -S, -S);
  glTexCoord2f(1.f, 1.0f);
  glVertex3f(-S, -S, +S);
  glTexCoord2f(1.f, 0.f);
  glVertex3f(-S, +S, +S);

  glEnd();
  utils::checkGLError(__FILE__, __LINE__);
}

void Map::drawWaterBackground() const {
  glColor3f(1.f, 1.f, 1.f);
  glDisable(GL_LIGHTING);
  glDisable(GL_FOG);
  glPushMatrix();
  {
    glBindTexture(GL_TEXTURE_2D, Texture::WaterBackground);
    glEnable(GL_TEXTURE_2D);
    static constexpr float skyboxSize = 800.f;
    glTranslatef(0.f, skyboxSize / 2.f, 0.f);
    glTranslatef(
        0.f, -3.f,
        0.f); // Make sure there is no seam between the terrain and the skybox
    drawCubeFill(skyboxSize);
    glDisable(GL_TEXTURE_2D);
  }
  glPopMatrix();
  glEnable(GL_FOG);
  glEnable(GL_LIGHTING);
  utils::checkGLError(__FILE__, __LINE__);
}

void Map::initialize()
{
	for (int i = 0; i < rockCount; i++)
	{
		Vector3 position = Point3{
			utils::random_float() * width - width / 2.f,
			0.f,
			utils::random_float() * width - width / 2.f
		};
		const float Scale = (1.f + utils::random_float()) / 10000000.f;
		const Vector3 scale = Vector3{Scale,Scale,Scale};
		const Vector3 rotation = Vector3{0.f, utils::random_float()*360.f, 0.f};
		rocks.emplace_back(position, scale, rotation);
	}
}

void Map::render(const Game& game) const {
  drawWaterBackground();
  drawSand(game.sub.getPosition());
    drawRocks();
  if (caustics) { drawCaustics(game); }
  utils::checkGLError(__FILE__, __LINE__);
}

void Map::drawRocks() const {

	for (const Rock& rock : rocks)
	{
		glPushMatrix();
		{
			glEnable(GL_LIGHTING);

			glTranslate(rock.position);

			glScalef(rock.scale.X(), rock.scale.Y(), rock.scale.Z());

			glRotatef(rock.rotation.X(), 1.f, 0.f, 0.f); // rotazione intorno all'asse X
			glRotatef(rock.rotation.Y(), 0.f, 1.f, 0.f); // rotazione intorno all'asse Y
			glRotatef(rock.rotation.Z(), 0.f, 0.f, 1.f); // rotazione intorno all'asse Z

			// Attivo la texture
			glBindTexture(GL_TEXTURE_2D, (GLuint)Texture::Rock);
			glEnable(GL_TEXTURE_2D);
			// Rendering della mesh
			rockMesh.RenderNxV(/*useTextureCoords*/ true);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);
			utils::checkGLError(__FILE__, __LINE__);
		}
		glPopMatrix();
	}
}

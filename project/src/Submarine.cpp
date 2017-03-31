#include <math.h>

#include <GL/gl.h>

#include <vector> // la classe vector di STL 

#include "Submarine.h"
#include "mesh.h"
#include "Textures.h"
#include "Utils.h"
#include "Game.h"

static TriangleWinding submarineMeshWinding = TriangleWinding::CCW;

// Variabili di tipo mesh
Mesh hull("../assets/obj/corpo.obj", submarineMeshWinding); // chiama il costruttore
Mesh motor("../assets/obj/motor.obj", submarineMeshWinding);
Mesh conningTower("../assets/obj/box.obj", submarineMeshWinding);

extern bool useWireframe; // var globale esterna: per usare l'evnrionment mapping

// DoStep: facciamo un passo di fisica (a delta_t costante)
//
// Indipendente dal rendering.
void Submarine::DoStep(Game &game) {
    // Computiamo l'evolversi del sottomarino
    float vxm, vym, vzm; // velocita' in spazio macchina
    // da vel frame mondo a vel frame macchina
    float cosf = cos(facing * (float) M_PI / 180.f);
    float sinf = sin(facing * (float) M_PI / 180.f);
    vxm = +cosf * vx - sinf * vz;
    vym = vy;
    vzm = +sinf * vx + cosf * vz;

    // Gestione dello sterzo
    if (game.inputManager.isLeftKeyPressed()) sterzo += velSterzo;
    if (game.inputManager.isRightKeyPressed()) sterzo -= velSterzo;
    sterzo *= velRitornoSterzo; // ritorno a volante dritto

    if (game.inputManager.isMoveForwardPressed()) vzm -= accMax; // accelerazione in avanti
    if (game.inputManager.isMoveBackwardPressed()) vzm += accMax; // accelerazione indietro

    if (game.inputManager.isUpKeyPressed()) vym += accMax; // salita
    if (game.inputManager.isDownKeyPressed()) vym -= accMax; // discesa

    // Se il turbo è attivato
    if (turbo) {

        if (game.getGameSeconds() < startTurboTime + turboDuration) {
            vzm -= accMax * turboAcc;
        } else {
            turbo = false;
            nextTurbo = game.getGameSeconds() + turboDelay;
        }
    }

    // Attriti
    vxm *= attritoX;
    vym *= attritoY;
    vzm *= attritoZ;

    // L'orientamento della macchina segue quello dello sterzo
    // (a seconda della velocita' sulla z)
    facing = facing - (vzm * grip) * sterzo;

    // Rotazione mozzo (a seconda della velocita' sulla z)
    float da; //delta angolo
    da = (360.f * vzm) / (2.f * (float) M_PI * raggioRuotaA);
    mozzoA += da;
    da = (360.f * vzm) / (2.f * (float) M_PI * raggioRuotaP);
    mozzoP += da;

    // Ritorno a vel coord mondo
    vx = +cosf * vxm + sinf * vzm;
    vy = vym;
    vz = -sinf * vxm + cosf * vzm;

        // posizione = posizione + velocita * delta t (ma delta t e' costante)

        px += vx;
        if (px < minX) { px = minX; }
        if (px > maxX) { px = maxX; }
        py += vy;
        if (py < minY) { py = minY; }
        if (py > maxY) { py = maxY; }
        pz += vz;
        if (pz < minZ) { pz = minZ; }
        if (pz > maxZ) { pz = maxZ; }

}

void Submarine::Init() {
    // inizializzo lo stato della macchina
    px = pz = facing = 0.f; // posizione e orientamento
    py = minY;

    mozzoA = mozzoP = sterzo = 0.f;   // stato
    vx = vy = vz = 0.f;      // velocita' attuale


    //velSterzo=3.4f;         // A
    velSterzo = 2.4f;         // A
    velRitornoSterzo = 0.93f; // B, sterzo massimo = A*B / (1-B)

    accMax = 0.0011f;

    // Attriti: percentuale di velocita' che viene mantenuta
    // 1 = no attrito
    // <<1 = attrito grande
    attritoZ = 0.991f;  // piccolo attrito sulla Z
    attritoX = 0.8f;  // grande attrito sulla X (per non fare slittare il sottomarino)
    attritoY = 0.95f; // attrito per la salita e discesa

    // Nota: vel max = accMax*attritoZ / (1-attritoZ)

    raggioRuotaA = 0.25f;
    raggioRuotaP = 0.35f;

    grip = 0.45f; // quanto il facing del sottomarino si adegua velocemente allo sterzo
}

// Attiva una luce di openGL per simulare un faro della macchina
void Submarine::SetLight() const {

        GLenum usedLight = GL_LIGHT1;
        glEnable(usedLight);

        float position[4] = {0.f, 2.f, 0.f, 1.f};
        glLightfv(usedLight, GL_POSITION, position);

        float direction[3] = {0.f, 0.f, -1.f};
        glLightfv(usedLight, GL_SPOT_DIRECTION, direction);

        glLightf(usedLight, GL_SPOT_CUTOFF, 30.f);
        glLightf(usedLight, GL_SPOT_EXPONENT, 5.f);

        float ambient[4] = {0.0f, 0.0f, 0.0f, 1.f};
        glLightfv(usedLight, GL_AMBIENT, ambient);

        float diffuse[4] = {1.f, 1.f, 1.f, 1.f};
        glLightfv(usedLight, GL_DIFFUSE, diffuse);

        float specular[4] = {0.f, 0.f, 0.f, 1.f};
        glLightfv(usedLight, GL_SPECULAR, specular);

        glLightf(usedLight, GL_CONSTANT_ATTENUATION, 0.f);
        glLightf(usedLight, GL_LINEAR_ATTENUATION, 0.0014f);
        glLightf(usedLight, GL_QUADRATIC_ATTENUATION, 0.007f);


	utils::checkGLError(__FILE__, __LINE__);
}


void Submarine::RenderAllParts(bool usecolor) const {

    // Disegna corpo del sottomarino con una mesh
    glPushMatrix();
    {
        static constexpr float SubmarineScale = 0.006f;
        const Vector3 ObjectScale{SubmarineScale, SubmarineScale, -SubmarineScale};
        glScalef(ObjectScale.X(), ObjectScale.Y(), ObjectScale.Z());
        static constexpr bool useTexCoords = true;
		utils::checkGLError(__FILE__, __LINE__);
        if (usecolor) {
			glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
			{
				float specular[4] = {1.f, 1.f, 1.f, 1.f};
				glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
				float diffuse[4] = {1.f, 1.f, 1.f, 1.f};
				glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
				float ambient[4] = {0.0f, 0.0f, 0.0f, 1.f};
				glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
				glMaterialf(GL_FRONT, GL_SHININESS, 32.f);
			}
			utils::checkGLError(__FILE__, __LINE__);
			glColor3f(1.f, 1.f, 1.f);

        	glEnable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, Texture::SubmarineHull);
			hull.RenderNxV(useTexCoords); // rendering delle mesh hull usando normali per vertice
			glBindTexture(GL_TEXTURE_2D, Texture::SubmarineMotor);
			motor.RenderNxV(useTexCoords);
			glBindTexture(GL_TEXTURE_2D, Texture::SubmarineConningTower);
			conningTower.RenderNxV(useTexCoords);
			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);

			utils::checkGLError(__FILE__, __LINE__);
		} else {
			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			hull.RenderNxV(useTexCoords); // rendering delle mesh hull usando normali per vertice
			motor.RenderNxV(useTexCoords);
			conningTower.RenderNxV(useTexCoords);
		}
		utils::checkGLError(__FILE__, __LINE__);
    }
    glPopMatrix();
}

// disegna a schermo
void Submarine::render(const Game &game) const {
    // sono nello spazio mondo



    glPushMatrix();
    {
        glTranslatef(px, py, pz);
        glRotatef(facing, 0.f, 1.f, 0.f);

        // sono nello spazio MACCHINA

        SetLight();
        if (!useHeadlight) {glDisable(GL_LIGHT1);}
        RenderAllParts(/*usecolor*/ true);

    }
    glPopMatrix();

	// Render Shadow
	if (game.useShadow) {
        glPushMatrix();
        {
			glTranslatef(px, 0.35f, pz);
			glRotatef(facing, 0.f, 1.f, 0.f);
            glScalef(1-(getPosition().Y()/40.f), 1- (getPosition().Y()/40.f), 1-(getPosition().Y()/40.f));
			glColor3f(12.f/255.f, 12.f/255.f, 12.f/255.f); // colore fisso
			glScalef(1.01f, 0.f, 1.01f); // appiattisco sulla Y, ingrandisco dell'1% sulla X e sulla Z
			glTranslatef(0.f, -pz + 5.f, 0.f); // alzo l'ombra di un epsilon per evitare z-fighting con il pavimento
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			RenderAllParts(/*usecolor*/ false); // disegno il sottomarino appiattito
			glDisable(GL_BLEND);
			glColor3f(1.f, 1.f,1.f);
        }
        glPopMatrix();
    }



    utils::checkGLError(__FILE__, __LINE__);
}

void Submarine::onInputEvent(InputEvent inputEvent, Game& game) {
    switch (inputEvent){

        case InputEvent::TurboButtonPressed:
                if (game.getGameSeconds() > nextTurbo) {
                    turbo = true;
                    startTurboTime = game.getGameSeconds();
                }
            break;

        case InputEvent::SwitchCamera:
            game.switchCamera();
            break;
        case InputEvent::UseHeadlight:
            useHeadlight = !useHeadlight;
            break;

        case InputEvent::ToggleHelpMenu:
            if (game.getGameSeconds() > game.tutorial.tutorialEndDate) {
                game.menuManager.setMenuState(MenuState::graphicsMenu);
                game.setState(GameState::Paused);
            }
            break;

        case InputEvent::ToggleCaustics:
            game.map.caustics = !game.map.caustics;
            break;

        case InputEvent::UseShadows:
            game.useShadow = !game.useShadow;
            break;
    }
}

    float Submarine::getTurboCharge(const Game& game)const {
    float turboCharge = 1.f;
    const float dateNow = game.getGameSeconds();
    const float turboEndDate = startTurboTime + turboDuration;
    if (turbo) {
        turboCharge = 0.f;
    } else if (dateNow >= nextTurbo) {
        turboCharge = 1.f;
    } else {
        float rawTurboCharge = (dateNow - turboEndDate)* (1/(turboDelay));
        turboCharge = fminf(fmaxf(rawTurboCharge, 0.f), 1.f); // deve essere compreso tra 0 ed 1
    }
    return turboCharge;
}

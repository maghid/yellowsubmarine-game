# Project: Yellow Submarine

CPP  = g++
CC   = gcc
BIN  = yellowSubmarine

OBJ  = main.o Submarine.o mesh.o Junk.o Input.o Utils.o JunkManager.o Enemy.o EnemyManager.o Map.o Textures.o Minimap.o TextRenderer.o MenuManager.o Game.o Tutorial.o
LINKOBJ  = main.o Submarine.o mesh.o Junk.o Input.o Utils.o JunkManager.o Enemy.o EnemyManager.o Map.o Textures.o Minimap.o TextRenderer.o MenuManager.o Game.o Tutorial.o
LIBS = -L/usr/X11R6 -lGL -lGLU -lSDL2_image -lSDL2 -lSDL2_ttf -lm 
INCS = -I. -I/usr/X11R6/include
CXXINCS= -std=c++11 
CXXFLAGS = $(CXXINCS)
CFLAGS = $(INCS)
RM = rm -f

all: $(BIN)


clean:
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

main.o: main.cpp
	$(CPP) -c main.cpp -o main.o $(CXXFLAGS)

Submarine.o: Submarine.cpp
	$(CPP) -c Submarine.cpp -o Submarine.o $(CXXFLAGS)

mesh.o: mesh.cpp
	$(CPP) -c mesh.cpp -o mesh.o $(CXXFLAGS)

Junk.o: Junk.cpp
	$(CPP) -c Junk.cpp -o Junk.o $(CXXFLAGS)

Input.o: Input.cpp
	$(CPP) -c Input.cpp -o Input.o $(CXXFLAGS)

Utils.o: Utils.cpp
	$(CPP) -c Utils.cpp -o Utils.o $(CXXFLAGS)

JunkManager.o: JunkManager.cpp
	$(CPP) -c JunkManager.cpp -o JunkManager.o $(CXXFLAGS)

Enemy.o: Enemy.cpp
	$(CPP) -c Enemy.cpp -o Enemy.o $(CXXFLAGS)

EnemyManager.o: EnemyManager.cpp
	$(CPP) -c EnemyManager.cpp -o EnemyManager.o $(CXXFLAGS)

Map.o: Map.cpp
	$(CPP) -c Map.cpp -o Map.o $(CXXFLAGS)

Textures.o: Textures.cpp
	$(CPP) -c Textures.cpp -o Textures.o $(CXXFLAGS)

Minimap.o: Minimap.cpp
	$(CPP) -c Minimap.cpp -o Minimap.o $(CXXFLAGS)

TextRenderer.o: TextRenderer.cpp
	$(CPP) -c TextRenderer.cpp -o TextRenderer.o $(CXXFLAGS)

MenuManager.o: MenuManager.cpp
	$(CPP) -c MenuManager.cpp -o MenuManager.o $(CXXFLAGS)

Game.o: Game.cpp
	$(CPP) -c Game.cpp -o Game.o $(CXXFLAGS)

Tutorial.o: Tutorial.cpp
	$(CPP) -c Tutorial.cpp -o Tutorial.o $(CXXFLAGS)
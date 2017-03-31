#pragma once

// classe Vertex:
// i vertici della mesh

#include "point3.h"
#include <vector>

class Vertex
{ 
public: 
  Point3 p; // posizione

  // attributi per verice
  Vector3 n; // normale (per vertice)
};

struct TexCoords
{
	float t;
	float s;
	float w;
};


class Edge
{
public: 
  Vertex* v[2]; // due puntatori a Vertice (i due estremi dell'edge)
  // attributi per edge:
};

class Face
{
public:
  Vertex *v[3]; // tre puntatori a Vertice (i tre vertici del triangolo)

  TexCoords *uv[3]; // tre coordinate di texture (possono essere null)

	// Constructor for face without texture coords
  Face(Vertex *a, Vertex *b, Vertex *c) {
    v[0] = a;
    v[1] = b;
    v[2] = c;
    uv[0] = nullptr;
    uv[1] = nullptr;
    uv[2] = nullptr;
  }

	// Constructor for face with texture coords
  Face(Vertex *a, Vertex *b, Vertex *c, TexCoords *at, TexCoords *bt,
       TexCoords *ct) {
    v[0] = a;
    v[1] = b;
    v[2] = c;
    uv[0] = at;
    uv[1] = bt;
    uv[2] = ct;
  }

  // attributi per faccia
  Vector3 n; // normale (per faccia)
  
  // computa la normale della faccia
  void ComputeNormal() {
    n= -( (v[1]->p - v[0]->p) % (v[2]->p - v[0]->p) ).Normalize();
  }
  
  // attributi per wedge
  
};

enum class TriangleWinding
{
	CCW, // senso anti-orario
	CW // senso orario
};


class Mesh
{
  std::vector<Vertex> v; // vettore di vertici
  std::vector<Face> f;   // vettore di facce
  std::vector<Edge> e;   // vettore di edge (per ora, non usato)
  std::vector<TexCoords> texCoords; // vettore di coordinate texture

                           
public:  
  
  // costruttore con caricamento[
	explicit Mesh(const char* const filename, const TriangleWinding triangleWinding = TriangleWinding::CW);

  // metodi
  void RenderNxF() const; // manda a schermo la mesh Normali x Faccia
  void RenderNxV(const bool useTexCoords = false) const; // manda a schermo la mesh Normali x Vertice
  void RenderWire() const; // manda a schermo la mesh in wireframe
  
  bool LoadFromObj(const char* const filename); //  carica la mesh da un file OBJ
  
  void ComputeNormalsPerFace();
  void ComputeNormalsPerVertex();
  
  void ComputeBoundingBox();

  // centro del axis aligned bounding box
  Point3 Center() const { return (bbmin+bbmax)/2.0; };
  
  Point3 bbmin,bbmax; // bounding box 
};

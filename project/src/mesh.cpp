// file mesh.cpp
//
// Implementazione dei metodi di Mesh

#include <stdio.h>
#include <string.h>
#include <vector>
#include <GL/gl.h>
#include "point3.h"
#include "mesh.h"

extern bool useWireframe; // quick hack: una var globale definita altrove

void Mesh::ComputeNormalsPerFace()
{
  for (size_t i=0; i<f.size(); i++)
  {
    f[i].ComputeNormal();
  }
}

// Computo normali per vertice
// (come media rinormalizzata delle normali delle facce adjacenti)
void Mesh::ComputeNormalsPerVertex()
{
  // uso solo le strutture di navigazione FV (da Faccia a Vertice)!
  
  // fase uno: ciclo sui vertici, azzero tutte le normali
  for (size_t i=0; i<v.size(); i++) {
    v[i].n = Point3(0.f,0.f,0.f);
  }
  
  // fase due: ciclo sulle facce: accumulo le normali di F nei 3 V corrispondenti
  for (size_t i=0; i<f.size(); i++) {
    f[i].v[0]->n=f[i].v[0]->n + f[i].n;
    f[i].v[1]->n=f[i].v[1]->n + f[i].n;
    f[i].v[2]->n=f[i].v[2]->n + f[i].n;
  }
  
  // fase tre: ciclo sui vertici; rinormalizzo
  // la normale media rinormalizzata e' uguale alla somma delle normnali, rinormalizzata
  for (size_t i=0; i<v.size(); i++) {
    v[i].n = -v[i].n.Normalize();
  }
  
}

// renderizzo la mesh in wireframe
void Mesh::RenderWire() const{
  glLineWidth(1.0);
  // (nota: ogni edge viene disegnato due volte. 
  // sarebbe meglio avere ed usare la struttura edge)
  for (size_t i=0; i<f.size(); i++)
  {
    glBegin(GL_LINE_LOOP);
      f[i].n.SendAsNormal();    
      (f[i].v[0])->p.SendAsVertex();   
      (f[i].v[1])->p.SendAsVertex();   
      (f[i].v[2])->p.SendAsVertex();
    glEnd();
  }
}

Mesh::Mesh(const char* const filename, const TriangleWinding triangleWinding)
{
	bool isMeshLoaded = LoadFromObj(filename);
	assert(isMeshLoaded);

	if (triangleWinding == TriangleWinding::CCW)
	{
		// inverti l'ordine dei vertex nel triangolo
		for (Face& face : f)
		{
			std::swap(face.v[0], face.v[2]);
			std::swap(face.uv[0], face.uv[2]);
		}
	}

	ComputeNormalsPerFace();
	ComputeNormalsPerVertex();
	ComputeBoundingBox();
}// Render usando la normale per faccia (FLAT SHADING)
void Mesh::RenderNxF() const
{
  
  // mandiamo tutti i triangoli a schermo
  glBegin(GL_TRIANGLES);
  for (size_t i=0; i<f.size(); i++)
  {
    f[i].n.SendAsNormal(); // flat shading
    
    f[i].v[0]->p.SendAsVertex();    
    f[i].v[1]->p.SendAsVertex();    
    f[i].v[2]->p.SendAsVertex();
  }
  glEnd();
}

// Render usando la normale per vertice (GOURAUD SHADING)
void Mesh::RenderNxV(const bool useTexCoords) const
{
  
  // mandiamo tutti i triangoli a schermo
  glBegin(GL_TRIANGLES);


  for (size_t faceIndex=0; faceIndex<f.size(); faceIndex++)
  {
	const Face& face = f[faceIndex];
	for (size_t vertexIndex = 0; vertexIndex < 3; vertexIndex++)
	{
		const Vertex* vert = face.v[vertexIndex];
		const TexCoords* texCoords = face.uv[vertexIndex];

		assert(vert);
		vert->n.SendAsNormal(); // gouroud shading
		if (useTexCoords) {
			assert(texCoords);
			glTexCoord3f(texCoords->t, texCoords->s, texCoords->w);
		}
		vert->p.SendAsVertex();
	}
  }
  glEnd();
}

// trova l'AXIS ALIGNED BOUNDIG BOX
void Mesh::ComputeBoundingBox(){
  // basta trovare la min x, y, e z, e la max x, y, e z di tutti i vertici
  // (nota: non e' necessario usare le facce: perche?) 
  if (!v.size()) return;
  bbmin=bbmax=v[0].p;
  for (size_t i=0; i<v.size(); i++){
    for (size_t k=0; k<3; k++) {
      if (bbmin.coord[k]>v[i].p.coord[k]) bbmin.coord[k]=v[i].p.coord[k];
      if (bbmax.coord[k]<v[i].p.coord[k]) bbmax.coord[k]=v[i].p.coord[k];
    }
  }
}

//#define DO_LOG_OBJ
#ifdef DO_LOG_OBJ
#define LOG_OBJ(stringFormat, ...) printf((stringFormat), __VA_ARGS__);
#else
#define LOG_OBJ(f_, ...)
#endif

// carica la mesh da un file in formato Obj
//   Nota: nel file, possono essere presenti sia quads che tris
//   ma nella rappresentazione interna (classe Mesh) abbiamo solo tris.
//
bool Mesh::LoadFromObj(const char* const filename)
{
  
  FILE* file = fopen(filename, "rt"); // apriamo il file in lettura
  if (!file) {
	  LOG_OBJ("%s %s \n", "Error opening file:", filename);
	  return false;
  }

  LOG_OBJ("%s %s \n", "Parsing obj", filename);

  //make a first pass through the file to get a count of the number
  //of vertices, normals, texcoords & triangles 
  char buf[128];
  int nv,nf,nt;
  float x,y,z;
  int va,vb,vc,vd;
  int na,nb,nc,nd;
  int ta,tb,tc,td;

  nv=0; nf=0; nt=0;
  while(fscanf(file, "%s", buf) != EOF) {
        switch(buf[0]) {
        case '#':               // comment
            // eat up rest of line
            fgets(buf, sizeof(buf), file);
            break;
        case 'v':               // v, vn, vt
            switch(buf[1]) {
            case '\0':          // vertex
                // eat up rest of line 
                fgets(buf, sizeof(buf), file);
                nv++;
                break;
            default:
                break;
            }
            break;
         case 'f':               // face
                fscanf(file, "%s", buf);
                // can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d
                if (strstr(buf, "//")) {
                    // v//n
                    sscanf(buf, "%d//%d", &va, &na);
                    fscanf(file, "%d//%d", &vb, &nb);
                    fscanf(file, "%d//%d", &vc, &nc);
                    nf++;
                    nt++;
                    while(fscanf(file, "%d//%d", &vd, &nd) > 0) {
                        nt++;
                    }
                } else if (sscanf(buf, "%d/%d/%d", &va, &ta, &na) == 3) {
                    // v/t/n
                    fscanf(file, "%d/%d/%d", &vb, &tb, &nb);
                    fscanf(file, "%d/%d/%d", &vc, &tc, &nc);
                    nf++;
                    nt++;
                    while(fscanf(file, "%d/%d/%d", &vd, &td, &nd) > 0) {
                        nt++;
                    }
                 } else if (sscanf(buf, "%d/%d", &va, &ta) == 2) {
                    // v/t
                    fscanf(file, "%d/%d", &vb, &tb);
                    fscanf(file, "%d/%d", &vc, &tc);
                    nf++;
                    nt++;
                    while(fscanf(file, "%d/%d", &vd, &td) > 0) {
                        nt++;
                    }
                } else {
                    // v
                    fscanf(file, "%d", &va);
                    fscanf(file, "%d", &vb);
                    nf++;
                    nt++;
                    while(fscanf(file, "%d", &vc) > 0) {
                        nt++;
                    }
                }
                break;

            default:
                // eat up rest of line
                fgets(buf, sizeof(buf), file);
                break;
        }
   }
 
 LOG_OBJ("dopo FirstPass nv=%d nf=%d nt=%d\n",nv,nf,nt);
  
  // allochiamo spazio per nv vertici
  v.resize(nv);

	// allochiamo spazio per nt coordinate uv
  texCoords.resize(nt);

  // rewind to beginning of file and read in the data this pass
  rewind(file);
  
  //on the second pass through the file, read all the data into the
  //allocated arrays

  nv = 0;
  int tcCounter = 0;
  while(fscanf(file, "%s", buf) != EOF) {
      switch(buf[0]) {
      case '#':               // comment
            // eat up rest of line
            fgets(buf, sizeof(buf), file);
            break;
      case 'v':               // v, vn, vt
            switch(buf[1]) {
            case '\0':          // vertex
                fscanf(file, "%f %f %f", &x, &y, &z);
                v.at(nv).p = Point3( x,y,z );
                nv++;
                break;
			case 't': // texture coords
				{
					float s;
					float t;
					float w;
					int result = fscanf(file, "%f %f %f", &s, &t, &w);
					if (result == 2)
					{
						// le coordinate sono flipped
						texCoords.at(tcCounter) = TexCoords{ 1.f-s, 1.f-t, 1.f };
					}
					else if (result == 3)
					{
						texCoords.at(tcCounter) = TexCoords{ 1.f-s, 1.f-t, w};
					}
					else
					{
						assert(false);
					}
					tcCounter++;
				}
				break;
            default:
                break;
            }
            break;


     case 'f':               // face
           fscanf(file, "%s", buf);
           // can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d
           if (strstr(buf, "//")) {
              // v//n
              sscanf(buf, "%d//%d", &va, &na);
              fscanf(file, "%d//%d", &vb, &nb);
              fscanf(file, "%d//%d", &vc, &nc);
              va--; vb--; vc--;
              Face newface( &v.at(va), &v.at(vc), &v.at(vb)); // invoco il costruttore di faccia
              f.push_back( newface ); // inserico la nuova faccia in coda al vettore facce
              vb=vc;
              while(fscanf(file, "%d//%d", &vc, &nc) > 0) {
                 vc--;
                 Face newface( &v.at(va), &v.at(vc), &v.at(vb)); // invoco il costruttore di faccia
                 f.push_back( newface ); // inserico la nuova faccia in coda al vettore facce
                 nt++;
                 vb=vc;
              }
           } else if (sscanf(buf, "%d/%d/%d", &va, &ta, &na) == 3) {
                // v/t/n
                fscanf(file, "%d/%d/%d", &vb, &tb, &nb);
                fscanf(file, "%d/%d/%d", &vc, &tc, &nc);
                va--; vb--; vc--; ta--;tb--;tc--;
                Face newface( &v.at(va), &v.at(vc), &v.at(vb), &texCoords.at(ta), &texCoords.at(tc), &texCoords.at(tb)); // invoco il costruttore di faccia
                f.push_back( newface ); // inserico la nuova faccia in coda al vettore facce
				vb = vc;tb = tc;
                while(fscanf(file, "%d/%d/%d", &vc, &tc, &nc) > 0) {
					vc--;tc--;
                   Face newface( &v.at(va), &v.at(vc), &v.at(vb), &texCoords.at(ta), &texCoords.at(tc), &texCoords.at(tb)); // invoco il costruttore di faccia
                   f.push_back( newface ); // inserico la nuova faccia in coda al vettore facce
                   nt++;
				   vb = vc;tb = tc;
                }
           } else if (sscanf(buf, "%d/%d", &va, &ta) == 2) {
                // v/t
                fscanf(file, "%d/%d", &vb, &tb);
                fscanf(file, "%d/%d", &vc, &tc);
				va--; vb--; vc--; ta--;tb--;tc--;
				
                Face newface( &v.at(va), &v.at(vc), &v.at(vb), &texCoords.at(ta), &texCoords.at(tc), &texCoords.at(tb)); // invoco il costruttore di faccia
                f.push_back( newface ); // inserico la nuova faccia in coda al vettore facce
				vb = vc;tb = tc;
                while(fscanf(file, "%d/%d", &vc, &tc) > 0) {
					vc--;tc--;
                   Face newface( &v.at(va), &v.at(vc), &v.at(vb), &texCoords.at(ta), &texCoords.at(tc), &texCoords.at(tb)); // invoco il costruttore di faccia
                   f.push_back( newface ); // inserico la nuova faccia in coda al vettore facce
				   vb = vc;tb = tc;
                }
           } else {
                // v
                sscanf(buf, "%d", &va);
                fscanf(file, "%d", &vb);
                fscanf(file, "%d", &vc);
                va--; vb--; vc--;
                Face newface( &v.at(va), &v.at(vc), &v.at(vb)); // invoco il costruttore di faccia
                f.push_back( newface ); // inserico la nuova faccia in coda al vettore facce
                vb=vc;
                while(fscanf(file, "%d", &vc) > 0) {
                   vc--;
                   Face newface( &v.at(va), &v.at(vc), &v.at(vb)); // invoco il costruttore di faccia
                   f.push_back( newface ); // inserico la nuova faccia in coda al vettore facce
                   vb=vc;
                }
            }
            break;

            default:
                // eat up rest of line
                fgets(buf, sizeof(buf), file);
                break;
    }
  }

//printf("dopo SecondPass nv=%d nt=%d\n",nv,nt);
  
  fclose(file);
  return true;
}

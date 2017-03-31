#pragma once

class Point2 {
public:
    Point2() {
      coord[0] = 0.f;
      coord[1] = 0.f;
    }

  Point2(float x, float y) {
    coord[0] = x;
    coord[1] = y;
  }

  float coord[2];

  float X() const { return coord[0]; }
  float Y() const { return coord[1]; }

  float& X() { return coord[0]; }
  float& Y() { return coord[1]; }

  // operatore "-" unario: inversione del verso del vettore
  Point2 operator-() const { return Point2(-coord[0], -coord[1]); }

  // somma fra vettori
  Point2 operator+(const Point2 &a) const {
    return Point2(coord[0] + a.coord[0], coord[1] + a.coord[1]);
  }

  // Operatore "*" binario: moltiplicazione per un'altro vettore
  // le coordinate x ed y del primo saranno moltiplicate con le coordinate x ed
  // y del secondo
  Point2 operator*(const Point2 &otherPoint) const {
    return Point2(coord[0] * otherPoint.X(), coord[1] * otherPoint.Y());
  }

  Point2 operator/(const Point2 &otherPoint) const {
	  return Point2(coord[0] / otherPoint.X(), coord[1] / otherPoint.Y());
  }

  Point2 operator/(const float& f) const {
	  return Point2{coord[0] / f, coord[1] / f};
  }
};

using Vector2 = Point2;
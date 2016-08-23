/**********************************************************
 * Version $Id$
 *********************************************************/
/* diverses.h */

#ifndef __DIVERSES_H 
#define __DIVERSES_H 

#include "grid_bsl.h"
#include <vector>
/*
class I_Vec : public  C_Vec2
{
 public:
  I_Vec(double x = 0.0, double y = 0.0);
  I_Vec(const C_Vec2& V);
  ~I_Vec();
   
  int  X(void) const;
  int  Y(void) const;
  double  X_binary(void) const;
  double  Y_binary(void) const;
  void adjust(void);
  int operator ==(const I_Vec& V);
  int operator ==(const I_Vec& V) const;
};
*/

class I_Vec
{
 public:
//  I_Vec(double x = 0.0, double y = 0.0);
	 I_Vec(int xx = 0, int yy = 0) : x(xx), y(yy) {};
  I_Vec(const C_Vec2& V) { x = (int)V.X(); y = (int)V.Y(); };
  ~I_Vec() {};
   
  inline int  X(void) const {return x; };
  inline int  Y(void) const {return y; };
//  void adjust(void);
  int operator ==(const I_Vec& V) {return x == V.X() && y == V.Y(); };
  int operator ==(const I_Vec& V) const {return x == V.X() && y == V.Y(); };
  inline I_Vec operator+(const I_Vec& v) const {return I_Vec(x+v.X(), y+v.Y());};
  inline I_Vec operator-(const I_Vec& v) const {return I_Vec(x-v.X(), y-v.Y());};
  inline I_Vec& operator = (const I_Vec& v) {x = v.X(); y = v.Y();  return *this;};
  inline I_Vec& operator += (const I_Vec& v) { x += v.X(); y += v.Y(); return *this;};
  inline I_Vec& operator -= (const I_Vec& v) { x -= v.X(); y -= v.Y(); return *this;};

  inline double Length(void) { return double(sqrt((double)(x*x+y*y))); };
 protected:
	 int x, y;
};


/*class IVecWerte
{
 public:
	IVecWerte( GridWerte& G) : Grid(G) {};
	~IVecWerte() {};

	inline double& operator [] ( I_Vec& I) 
	{
		return Grid(int(I.X()),int(I.Y()));
	}

	const GridWerte& operator () () const
	{
		return Grid;
	}
 protected:
	const GridWerte&  Grid;
};
*/
/*
class IVecWerteInt
{
 public:
	IVecWerteInt(GridWerteInt& G) : Grid(G) {};
	~IVecWerteInt() {};

	inline int& operator [] (const I_Vec& I) const
	{
		return Grid.Z[I.Y()][I.X()];
	}

	GridWerteInt& operator () () const
	{
		return Grid;
	}
 protected:
	GridWerteInt&  Grid;
};
*/

bool Rand(int x, int y, const GridWerte& W, int abstand = 0);
//int Rand(const I_Vec& I, const IVecWerte& W, int abstand = 0);
//int Rand(const I_Vec& I, const GridWerte& W, int abstand = 0);
//int innerhalb(const I_Vec& I, const GridWerte& W);
//int innerhalb(const I_Vec& I, const IVecWerte& W);
int innerhalb(int x, int y, const GridWerte& W);
//int isRandPunkt(const I_Vec iv, const GridWerte& G);

void normieren(C_Vec2& G);
//double WinkelDiff(double x, double y);

double max3(double x, double y, double z);
double min3(double x, double y, double z);



void
LinRand( GridWerte& G, GridWerte& Erg);
/*  Speicher: Erg.getMem */




void DGMGlatt(const GridWerte& G, GridWerte& Erg);
/* Speicher: Erg.getMem */

void copyGrid(GridWerte& Dest,  GridWerte& Source, bool newmem = true);

void calcExpoAbweichung4erFeld(GridWerte& Erg,  GridWerte& Hoehe);
/* Speicher: Erg.getMem */

void calcExpoAbweichung(GridWerte& Erg,  GridWerte& Expo);
/* Speicher: Erg.getMem */




#endif

/* End of file */

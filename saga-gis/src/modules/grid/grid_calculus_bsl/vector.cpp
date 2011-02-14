/**********************************************************
 * Version $Id$
 *********************************************************/
////////////////////////////////////////////////////////////////////////////
// Programm    :                                                            
// Bibliothek  : vector.cpp                                                 
// Header-Datei:                                                            
//                                                                          
////////////////////////////////////////////////////////////////////////////
//                                                                          
// Autoren        :  Christian Trachinow                                    
// 		     Joerg Dolle                                            
// 		                                                            
// Telefon-Nr.    :  0551/66437                                             
// Mail-Adresse   :  jdolle@gwdg.de                                         
//                                                                          
////////////////////////////////////////////////////////////////////////////
//                                                                          
// Compiler       :  Borland C++   V.: 4.00                                 
// Ersterstellung :  16.11.95                                               
// Ueberarbeitung :  22.04.96                                               
//                                                                          
// Programmversion:  1.01                                                   
//                                                                          
////////////////////////////////////////////////////////////////////////////


/****************** Include-Dateien **************/

#include "MLB_Interface.h"
//#include ".\..\INCLUDE\MAT_Tools.h"	// für M_PI etc...

#include "vector.h"


//Do * 2D-Vectorklasse ***********************************************
C_Vec2::C_Vec2(double x_neu, double y_neu)
{
  x = x_neu;
  y = y_neu;
}

C_Vec2::C_Vec2(const C_Vec2& V)
{
  x = V.x;
  y = V.y;
}

C_Vec2::~C_Vec2()
{
}

void C_Vec2::setPolar(double l, double a)
{
	x = double(l*cos(a));
	y = double(l*sin(a));
}


C_Vec2& C_Vec2::operator = (const C_Vec2& v)
{
  x = v.x;
  y = v.y;
  return *this;
}

C_Vec2& C_Vec2::operator += (const C_Vec2& v)
{
  x += v.x;
  y += v.y;
  return *this;
}

C_Vec2& C_Vec2::operator -= (const C_Vec2& v)
{
  x -= v.x;
  y -= v.y;
  return *this;
}

C_Vec2 operator + (const C_Vec2& w,const C_Vec2& v)
{
  
  return C_Vec2(w.x+v.x, w.y+v.y);
}

C_Vec2 operator - (const C_Vec2& w,const C_Vec2& v)
{
  return C_Vec2(w.x-v.x, w.y-v.y);
}

C_Vec2 C_Vec2::operator - ()
{
  return C_Vec2(-x, -y); 
}

C_Vec2& C_Vec2::operator /= (double v)
{
  x = x/v;
  y = y/v;
  return *this;
}

C_Vec2& C_Vec2::operator *= (double v)
{
  x = x*v;
  y = y*v;
  return *this;
}

C_Vec2 operator * (const C_Vec2& w, double v)
{
  return C_Vec2(double (w.x*v), double (w.y*v));
}

C_Vec2 operator / (const C_Vec2& w, double v)
{
  return C_Vec2(double(w.x/v), double(w.y/v));
}

C_Vec2 operator * (double v,const C_Vec2& w)
{
  return C_Vec2(double(w.x*v), double(w.y*v));
}

C_Vec2 operator / (double v,const C_Vec2& w)
{
  return C_Vec2(double(w.x/v), double(w.y/v));
}


double operator * (const C_Vec2& v, const C_Vec2& w)
{
  return ((v.x*w.x) + (v.y*w.y));
}


double operator < (const C_Vec2& v, const C_Vec2& w)
{
  return double(acos((v*w) / (v.Length() * w.Length())));
}


// *** Funktionen **************
double C_Vec2::X(void) const
{
  return x;
}

double C_Vec2::Y(void) const
{
  return y;
}

double C_Vec2::Length(void) const
{
  return double(sqrt(x*x+y*y));
}

double C_Vec2::Angle(void) const
{
	// Angabe es Polarwinkels in rad
	if (x > 0.0f) return double(atan(y/x));
   if (x < 0.0f) return double(atan(y/x) + M_PI);
	if (x == 0.0f || y > 0.0f) return double(M_PI_090);
	if (x == 0.0f || y < 0.0f) return double(-M_PI_090);
   return 0.0f;
} 

//****************************************************************



//Do * 3D-Vectorklasse ***********************************************
C_Vec3::C_Vec3(double x_neu, double y_neu, double z_neu)
{
  x = x_neu;
  y = y_neu;
  z = z_neu;
}


C_Vec3::C_Vec3(const C_Vec3& V)
{
  x = V.x;
  y = V.y;
  z = V.z;
}

C_Vec3::~C_Vec3()
{
}

void C_Vec3::setPolar(double l, double a_phi, double a_psi)
{
  x = double(l*sin(a_psi)*sin(a_phi));
  y = double(l*sin(a_psi)*cos(a_phi));
  z = double(l*cos(a_psi));
}



C_Vec3& C_Vec3::operator = (const C_Vec3& v)
{
  x = v.x;
  y = v.y;
  z = v.z;
  return *this;
}

C_Vec3& C_Vec3::operator += (const C_Vec3& v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}

C_Vec3& C_Vec3::operator -= (const C_Vec3& v)
{
  x -= v.x;
  y -= v.y;
  z -= v.z;
  return *this;
}

C_Vec3 operator + (const C_Vec3& w,const C_Vec3& v)
{
  return C_Vec3(w.x+v.x, w.y+v.y, w.z+v.z);
}

C_Vec3 operator - (const C_Vec3& w,const C_Vec3& v)
{
  return C_Vec3(w.x-v.x, w.y-v.y, w.z-v.z);
}

C_Vec3 C_Vec3::operator - (const C_Vec3 &v)
{
  return C_Vec3(-v.x, -v.y, -v.z); 
}

C_Vec3& C_Vec3::operator /= (double v)
{
  x = x/v;
  y = y/v;
  z = z/v;
  return *this;
}

C_Vec3& C_Vec3::operator *= (double v)
{
  x = x*v;
  y = y*v;
  z = z*v;
  return *this;
}

C_Vec3 operator * (const C_Vec3& w, double v)
{
  return C_Vec3(w.x*v, w.y*v, w.z*v);
}

C_Vec3 operator / (const C_Vec3& w, double v)
{
  return C_Vec3(w.x/v, w.y/v, w.z/v);
}

C_Vec3 operator * (double v,const C_Vec3& w)
{
  return C_Vec3(w.x*v, w.y*v, w.z*v);
}

C_Vec3 operator / (double v,const C_Vec3& w)
{
  return C_Vec3(w.x/v, w.y/v, w.z/v);
}


double operator * (const C_Vec3& v, const C_Vec3& w)
{
  return ((v.x*w.x) + (v.y*w.y) + (v.z*w.z));
}


double operator < (const C_Vec3& v, const C_Vec3& w)
{
  return double(acos((v*w) / (v.Length() * w.Length())));
}


// *** Funktionen **************
double C_Vec3::X(void) const
{
  return x;
}

double C_Vec3::Y(void) const
{
  return y;
}

double C_Vec3::Z(void) const
{
  return z;
}

double C_Vec3::Length(void) const
{
  return double(sqrt(x*x+y*y+z*z));
}

double C_Vec3::Angle_Phi(void)const
{	// Angabe es Polarwinkels in rad
	if (x > 0.0f) return double(atan(y/x));
	if (x < 0.0f) return double(atan(y/x) + M_PI);
	if (x == 0.0f || y > 0.0f) return double(M_PI_090);
	if (x == 0.0f || y < 0.0f) return double(-M_PI_090);
   return 0.0f;
}

double C_Vec3::Angle_Psi(void)const
{	// Angabe es Höhenwinkels in rad
   return double(asin(z/(sqrt(x*x+y*y))));
}


C_Vec3 C_Vec3::Kreuzprod(C_Vec3& v){
  return C_Vec3( ( (y *v.Z()) - (z *v.Y()) ) ,
		 ( (z *v.X()) - (x *v.Z()) ) ,
		 ( (x *v.Y()) - (y *v.X()) ) );
  }


double C_Vec3::Spatprod(C_Vec3& v, C_Vec3& w){
  return (*this * (v.Kreuzprod(w)));
  }


//DO * Rechteck-Klasse ***********************************************
C_Rect::C_Rect()
{
  double startx, endx, starty, endy;

  startx = 0.0f;
  starty = 0.0f;
  endx = 0.0f;
  endy = 0.0f;
  
  Start = C_Vec2(startx, starty);
  End = C_Vec2(endx, endy);
}


C_Rect::C_Rect(const double startX,const double startY,
               const double endX, const double endY)
{
  double startx, endx, starty, endy;

  startx = startX;
  starty = startY;
  endx = endX;
  endy = endY;
  
  if (startX > endX)
    {
      startx = endX;
      endx = startX;
    }
  if (startY > endY)
    {
      starty = endY;
      endy = startY;
    }

  Start = C_Vec2(startx, starty);
  End = C_Vec2(endx, endy);
}


C_Rect::C_Rect(const C_Vec2& start, const C_Vec2& end)
{
  double startx, endx, starty, endy;
  
  Start = start;
  End = end;

  startx = start.X();
  starty = start.Y();
  endx = end.X();
  endy = end.Y();

  if (start.X() > end.X())
    {
      startx = end.X();
      endx = start.X();
    }
  if (start.Y() > end.Y())
    {
      starty = end.Y();
      endy = start.Y();
    }

  Start = C_Vec2(startx, starty);
  End = C_Vec2(endx, endy);
}

C_Rect::C_Rect(const C_Rect& r)
{
  Start = r.Start;
  End = r.End;
}

C_Rect::~C_Rect()
{
}

C_Rect& C_Rect::operator = (const C_Rect& r)
{
  Start = r.Start;
  End = r.End;
  return *this;
}

C_Rect& C_Rect::operator += (const C_Vec2& v)
{
  Start += v;
  End += v;
  return *this;
}

C_Rect& C_Rect::operator -= (const C_Vec2& v)
{
  Start -= v;
  End -= v;
  return *this;
}

C_Rect operator + (const C_Rect& r, const C_Vec2& v)
{
  return C_Rect(r.Start + v, r.End + v);
}

C_Rect operator + (const C_Vec2&v, const C_Rect& r)
{
  return C_Rect(r.Start + v, r.End + v);
}

C_Rect operator - (const C_Rect& r, const C_Vec2& v)
{
  return C_Rect(r.Start - v, r.End - v);
}


C_Rect operator * (const C_Rect& r, double v)
{
	return C_Rect(r.X1()*v, r.Y1()*v, r.X2()*v, r.Y2()*v);
}

C_Rect operator * (double v, const C_Rect& r)
{
	return C_Rect(r.X1()*v, r.Y1()*v, r.X2()*v, r.Y2()*v);
}

C_Rect operator / (const C_Rect& r, double v)
{
	return C_Rect(r.X1()/v, r.Y1()/v, r.X2()/v, r.Y2()/v);	
}

C_Rect C_Rect::intersect(const C_Rect& R)
{
/*	double tx, ty, tw, th;
	
	tw = MIN (X2(), R.X2());
	th = MIN (Y2(), R.Y2());
	tx = MAX (X1(), R.X1());
	ty = MAX (Y1(), R.Y1());

	if ((tw > tx) && (th > ty))
		return C_Rect(tx, ty, tw, th);
	else
		return C_Rect(0.0f, 0.0f, 0.0f, 0.0f);*/
return C_Rect(0.0f, 0.0f, 0.0f, 0.0f);
}


double C_Rect::X(void) const
{
  return Start.X();
}

double C_Rect::Y(void) const
{
  return Start.Y();
}

double C_Rect::X1(void) const
{
  return Start.X();
}

double C_Rect::Y1(void) const
{
  return Start.Y();
}

double C_Rect::W(void) const
{
  return End.X() - Start.X();
}

double C_Rect::H(void) const 
{
  return End.Y() - Start.Y();
}

double C_Rect::X2(void) const 
{
  return End.X();
}

double C_Rect::Y2(void) const 
{
  return End.Y();
}

double C_Rect::diagonale(void) const 
{
  double x = X2() - X1();
  double y = Y2() - Y1();
  return double(sqrt(x*x+y*y));
}

C_Vec2 C_Rect::showStart(void) const 
{
  return Start;
}

C_Vec2 C_Rect::showEnd(void) const
{
  return End;
}

int C_Rect::Inside(C_Vec2 v)
{
  if( (v.X()>=Start.X())&&
		(v.X()<=End.X())&&
		(v.Y()>=Start.Y())&&
		(v.Y()<=End.Y()) ) return 1;
  return 0;
}


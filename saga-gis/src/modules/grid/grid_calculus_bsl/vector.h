/**********************************************************
 * Version $Id$
 *********************************************************/
////////////////////////////////////////////////////////////////////////////
// Programm    :                                                            
// Bibliothek  :                                                            
// Header-Datei: vector.h                                                   
//                                                                          
////////////////////////////////////////////////////////////////////////////
//                                                                          
// Autoren        :  Joerg Dolle                                            
// 		     Christian Trachimow                                    
// 		                                                            
// Telefon-Nr.    :  0561/774807                                            
// Mail-Adresse   :  jdolle@gwdg.de                                         
//                                                                          
////////////////////////////////////////////////////////////////////////////
//                                                                          
// Compiler       :  Borland C++   V.: 4.00                                 
// Ersterstellung :  16.11.95                                               
// Ueberarbeitung :  22.04.96                                               
//                                                                          
// Programmversion:  2.00                                                   
//                                                                          
////////////////////////////////////////////////////////////////////////////


/****************** Include-Dateien **************/
#ifndef __vector_h__
#define __vector_h__
//#include <iostream.h>
#include <math.h>

//!!#undef  HEADER_INCLUDED__DATA
//!!#define HEADER_INCLUDED__DATA HEADER_INCLUDED__EXT_CLASS


//#define M_PI 3.14159265358979323846
//#define M_PI_2 M_PI/2.0


// Umrechnung von Grad in rad und umgekehrt
#define PI_180 (M_PI/180)
#define GRAD(arc) (arc/PI_180)
#define ARC(grad) (grad*PI_180)


//Do * 2D-Vectorklasse ***********************************************
class  C_Vec2
{
 public:
  C_Vec2() : x(0.0f), y(0.0f) {};
  C_Vec2(double x_neu, double y_neu);
  C_Vec2(const C_Vec2& V);
  ~C_Vec2();

  void setPolar(double l, double a);

  C_Vec2& operator = (const C_Vec2& v);
  C_Vec2& operator += (const C_Vec2& v);
  C_Vec2& operator -= (const C_Vec2& v);

//  friend HEADER_INCLUDED__EXT_API const C_Vec2 operator + (const C_Vec2& w, const C_Vec2& v);
//  friend HEADER_INCLUDED__EXT_API const C_Vec2 operator - (const C_Vec2& w, const C_Vec2& v);
  C_Vec2 operator -();
//  class C_Vec2 operator -();

  
  C_Vec2& operator /= (double v);
  C_Vec2& operator *= (double v);
  friend  C_Vec2 operator + (const C_Vec2& w, const C_Vec2& v);
  friend  C_Vec2 operator - (const C_Vec2& w, const C_Vec2& v);
  friend C_Vec2 operator * (const C_Vec2& w, double v);
  friend  C_Vec2 operator / (const C_Vec2& w, double v);
  friend  C_Vec2 operator * (double v, const C_Vec2& w);
  friend  C_Vec2 operator / (double v, const C_Vec2& w);

  friend  double operator * (const C_Vec2& v, const C_Vec2& w);
  friend  double operator < (const C_Vec2& v, const C_Vec2& w);
  
  double X(void) const;
  double Y(void) const;
  double Length(void) const;
  double Angle(void) const;

 protected:
  double x, y;
};
   
 C_Vec2 operator + (const C_Vec2& w,const C_Vec2& v);
 C_Vec2 operator - (const C_Vec2& w,const C_Vec2& v);
 C_Vec2 operator * (const C_Vec2& w, double v);
 C_Vec2 operator / (const C_Vec2& w, double v);
 C_Vec2 operator * (double v,const C_Vec2& w);
 C_Vec2 operator / (double v,const C_Vec2& w);
 double operator * (const C_Vec2& v, const C_Vec2& w);
 double operator < (const C_Vec2& v, const C_Vec2& w);




//Do * 3D-Vectorklasse ***********************************************
class  C_Vec3
{
 public:
  C_Vec3() : x(0.0f), y(0.0f), z(0.0f) {}; 
  C_Vec3(double x_neu, double y_neu, double z_neu);
  C_Vec3(const C_Vec3& V);
  ~C_Vec3();

  void setPolar(double l, double a_phi, double a_psi);

  C_Vec3& operator = (const C_Vec3& v);
  C_Vec3& operator += (const C_Vec3& v);
  C_Vec3& operator -= (const C_Vec3& v);
  friend  C_Vec3 operator + (const C_Vec3& w, const C_Vec3& v);
  friend  C_Vec3 operator - (const C_Vec3& w, const C_Vec3& v);
  C_Vec3 operator - (const C_Vec3 &v);
  
  
  C_Vec3& operator /= (double v);
  C_Vec3& operator *= (double v);
  friend  C_Vec3 operator * (const C_Vec3& w, double v);
  friend  C_Vec3 operator / (const C_Vec3& w, double v);
  friend  C_Vec3 operator * (double v, const C_Vec3& w);
  friend  C_Vec3 operator / (double v, const C_Vec3& w);

  friend  double operator * (const C_Vec3& v, const C_Vec3& w);
  friend  double operator < (const C_Vec3& v, const C_Vec3& w);

  double X(void) const;
  double Y(void) const;
  double Z(void) const;
  double Length(void) const;
  double Angle_Phi(void) const;
  double	Angle_Psi(void) const;
  C_Vec3 Kreuzprod(C_Vec3& v);
  double  Spatprod(C_Vec3& v, C_Vec3& w);

 protected:
  double x, y, z;
};

 C_Vec3 operator + (const C_Vec3& w,const C_Vec3& v);
 C_Vec3 operator - (const C_Vec3& w,const C_Vec3& v);
 C_Vec3 operator * (const C_Vec3& w, double v);
 C_Vec3 operator / (const C_Vec3& w, double v);
 C_Vec3 operator * (double v,const C_Vec3& w);
 C_Vec3 operator / (double v,const C_Vec3& w);
 double operator * (const C_Vec3& v, const C_Vec3& w);
 double operator < (const C_Vec3& v, const C_Vec3& w);


//DO *einfache Rechteck-Klasse z.B. zum Klipping**********************
class  C_Rect
{
 public:
  C_Rect();
  C_Rect(const double x1,const double y1,const double x2,const double y2);
  C_Rect(const C_Vec2& start, const C_Vec2& end);
  C_Rect(const C_Rect& r);
  ~C_Rect();

  C_Rect& operator = (const C_Rect& r);
  C_Rect& operator += (const C_Vec2& v);
  C_Rect& operator -= (const C_Vec2& v);
  friend  C_Rect operator + (const C_Rect& r, const C_Vec2& v);
  friend  C_Rect operator + (const C_Vec2&v, const C_Rect& r);
  friend  C_Rect operator - (const C_Rect& r, const C_Vec2& v);
  friend  C_Rect operator * (const C_Rect& r, double v);
  friend  C_Rect operator * (double v, const C_Rect& r);
  friend  C_Rect operator / (const C_Rect& r, double v);


  double X(void) const;
  double Y(void) const;
  double X1(void) const;
  double Y1(void) const;
  double W(void) const;
  double H(void) const;
  double X2(void) const;
  double Y2(void) const;
  double diagonale(void) const;
  C_Rect intersect(const C_Rect& R);
  C_Vec2 showStart(void) const;
  C_Vec2 showEnd(void) const;
  int Inside(C_Vec2 v);
 private:
  C_Vec2 Start, End;

};

 C_Rect operator + (const C_Rect& r, const C_Vec2& v);
 C_Rect operator + (const C_Vec2&v, const C_Rect& r);
 C_Rect operator - (const C_Rect& r, const C_Vec2& v);
 C_Rect operator * (const C_Rect& r, double v);
 C_Rect operator * (double v, const C_Rect& r);
 C_Rect operator / (const C_Rect& r, double v);



//!!#undef HEADER_INCLUDED__DATA
//!!#define HEADER_INCLUDED__DATA

#endif

/**********************************************************
 * Version $Id$
 *********************************************************/
// diverses.cc

//#include "../stdafx.h"
#include <iostream>
#include <sstream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "grid_bsl.h"
#include "diverses.h"

#include <vector>
#include <algorithm>

using namespace std;

#define false 0
#define true 1

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;


/*int Rand(const I_Vec& I, const GridWerte& G, int abstand)
{
	IVecWerte W((GridWerte &) G);
	return Rand(I, W, abstand);
}
*/
/*
int Rand(const I_Vec& I, const IVecWerte& W, int abstand)
{
	return Rand(I.X(), I.Y(), W(), abstand);

}
*/
bool Rand(int x, int y, const GridWerte& W, int abstand )
{
	return (x == abstand || x == W.xanz-1-abstand
		|| y == abstand || y == W.yanz-1-abstand);
}

/*
int innerhalb(const I_Vec& I, const GridWerte& W)
{
	int x = I.X();
	int y = I.Y();
	
	return (x >= 0 && x < W.xanz && y >= 0 && y < W.yanz);
}

int innerhalb(const I_Vec& I, const IVecWerte& W)
{
	int x = I.X();
	int y = I.Y();
	
	return (x >= 0 && x < W().xanz && y >= 0 && y < W().yanz);
}
*/
int innerhalb(int x, int y, const GridWerte& W)
{
	return (x >= 0 && x < W.xanz && y >= 0 && y < W.yanz);
}


ostream& operator << (ostream& o, const C_Vec2& V)
{
	o << "(" << V.X() << ", " << V.Y() << ")";
	return o;
}

void normieren(C_Vec2& G)
	// Normiert einen Vektor
{
	double r = G.Length();
	G /= r;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    min3 max3
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


double max3(double x, double y, double z)
{
	return( x > y ? (x > z ? x : z) : (y > z ? y : z) );
}

double min3(double x, double y, double z)
{
	return( x < y ? (x < z ? x : z) : (y < z ? y : z) );
}


static int
NachbarBesetzt(int y0, int y1, int x0, int x1, int y, int x, 
			   GridWerte& G)
{
	for (int i=y0; i<=y1; i++)
		for (int j=x0; j<=x1; j++)
			if (i == -1 || i == 1 || j == -1 || j == 1)
			{
				if (innerhalb(x+j, y+i, G))
					if (G(x+j,y+i) != 0.0)
						return true;
			}
	return false;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  DeleteNotKonvex
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++




//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//     LinRand
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void
LinRand( GridWerte& G, GridWerte& Erg)
{
	Erg = G;
	Erg.xanz += 2;
	Erg.yanz += 2;
	Erg.xll -= Erg.dxy;
	Erg.yll -= Erg.dxy;
	Erg.getMem();

	int i;
	int j;
	long yyy = G.yanz;
	long xxx = G.xanz;

//	for (i=0; i<G.yanz; i++)
//		for (j=0; j<G.xanz; j++)
	for (i=0; i<yyy; i++)
		for (j=0; j<xxx; j++)
			Erg.Set_Value(j+1,i+1, G(j,i));

	// linke  Kante
	j = 0;
	for (i=1; i<=G.yanz; i++)
		Erg.Set_Value(j,i, 2.0*G(j,i-1)-G(j+1,i-1));
	
	// rechte Kante
	j = G.xanz-1;

	for (i=1; i<=G.yanz; i++)
		Erg.Set_Value(j+2,i, 2.0*G(j,i-1)-G(j-1,i-1));
	
	// untere Kante
	i = 0;
//	for (j=1; j<=G.xanz; j++)
	for (j=1; j<=xxx; j++)
		Erg.Set_Value(j,i, 2.0*G(j-1,i)-G(j-1,i+1));
	
	// obere Kante
	i = G.yanz-1;
//	for (j=1; j<=G.xanz; j++)
	for (j=1; j<=xxx; j++)
		Erg.Set_Value(j,i+2, 2.0*G(j-1,i)-G(j-1,i-1));

	double m1, m2;
	// Ecke ul
	m1 = 2*Erg(0,1)-Erg(0,2);
	m2 = 2*Erg(1,0)-Erg(2,0);
	Erg.Set_Value(0,0, (m1+m2)/2);
	
	// Ecke ur
	m1 = 2*Erg(Erg.xanz-1,1)-Erg(Erg.xanz-1,2);
	m2 = 2*Erg(Erg.xanz-2,0)-Erg(Erg.xanz-3,0);
	Erg.Set_Value(Erg.xanz-1,0, (m1+m2)/2);

	// Ecke ol
	m1 = 2*Erg(0,Erg.yanz-2)-Erg(0,Erg.yanz-3);
	m2 = 2*Erg(1,Erg.yanz-1)-Erg(2,Erg.yanz-1);
	Erg.Set_Value(0,Erg.yanz-1, (m1+m2)/2);

	// Ecke or
	m1 = 2*Erg(Erg.xanz-1,Erg.yanz-2)-Erg(Erg.xanz-1,Erg.yanz-3);
	m2 = 2*Erg(Erg.xanz-2,Erg.yanz-1)-Erg(Erg.xanz-3,Erg.yanz-1);
	
	Erg.Set_Value(Erg.xanz-1,Erg.yanz-1, (m1+m2)/2);
	 
	Erg.calcMinMax();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//        RandHinzu
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


void copyGrid(GridWerte &Dest,  GridWerte &Source, bool newmem)
{
	if (newmem)
	{
		Dest = Source;
		Dest.getMem();
	}

	for (int i=0; i<Source.yanz; i++)
	{
		for (int j=0; j<Source.xanz; j++)
		{
			double wert= Source(j,i);

			Dest.Set_Value(j,i, wert);
		}
	}
	

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//        Expositions-Abweichung
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void calcExpoAbweichung(GridWerte& Erg,  GridWerte& Expo)
{
	int		i;

	double w[9];
	// Standard-Expositionen zur mittleren Rasterzelle
	w[0] = double( 45.0/180.0*M_PI);  // -1 -1
	w[1] = double(  0.0/180.0*M_PI);  //  0 -1
	w[2] = double(315.0/180.0*M_PI);  //  1 -1
	
	w[3] = double( 90.0/180.0*M_PI);  // -1  0
	w[4] = double(  0.0/180.0*M_PI);  //  0  0 egal, wird nicht benutzt
	w[5] = double(270.0/180.0*M_PI);  //  1  0
	
	w[6] = double(135.0/180.0*M_PI);  // -1 +1
	w[7] = double(180.0/180.0*M_PI);  //  0 +1
	w[8] = double(225.0/180.0*M_PI);  //  1 +1

	Erg = Expo;
	Erg.getMem();
	for (i=0; i<Erg.yanz; i++)
	{
		for (int j=0; j<Erg.xanz; j++)
			Erg.Set_Value(j,i, 0.0f);
	}

	int zaehler;
	double winkel;
	for (i=0; i<Erg.yanz; i++)
	{
		for (int j=0; j<Erg.xanz; j++)
		{
			zaehler = 0;
			for (int ii=-1; ii<=1; ii++)
			{
				for (int jj=-1; jj<=1; jj++)
				{
					if ((ii != 0 || j != 0) && innerhalb(j+jj,i+ii, Erg))
					{
						winkel = fabs(w[(ii+1)*3+jj+1]-Expo(j+jj,i+ii));
						if (winkel > M_PI)
							winkel = 2*M_PI-winkel;
						Erg.Add_Value(j,i, winkel/M_PI);
						zaehler++;
					}
				}
			}
			if (zaehler != 0)
				Erg.Mul_Value(j,i, 1.0 / zaehler);
		}
	}

}

static double calcMittelwert( GridWerte& H, int x, int y)
{
	if (x != H.xanz-1 && y != H.yanz-1)
		return (H(x,y) + H(x,y+1) + H(x+1,y) + H(x+1,y+1))/4.0f;
	else if (x == H.xanz-1)
	{
		return (H(x,y) + H(x,y+1))/2.0f; 
	}
	else
	{
		return (H(x,y) + H(x+1,y))/2.0f; 
	}
}






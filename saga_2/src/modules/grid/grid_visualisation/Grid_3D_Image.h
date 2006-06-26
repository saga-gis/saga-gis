
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Grid_Visualisation                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Grid_3D_Image.h                    //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Grid_3D_Image_H
#define HEADER_INCLUDED__Grid_3D_Image_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_3D_Image : public CModule_Grid  
{
public:
	CGrid_3D_Image(void);
	virtual ~CGrid_3D_Image(void);


protected:

	virtual bool					On_Execute			(void);


private:

	typedef struct
	{
		bool						bOk;

		BYTE						r, g, b;

		int							x, y;

		double						z;
	}
	T3DPoint;


	int								m_Projection;

	double							m_ZMean, m_ZExagg, m_ZExagg_Min, m_ZRotate, m_XRotate, m_XScale, m_YScale, m_PanoramaBreak;

	CGrid							*m_pDEM, *m_pImage, *m_pRGB, *m_pRGB_Z;


	void							_Set_Grid			(void);
	void							_Set_Shapes			(CShapes *pInput);

	void							_Get_Line			(int y, T3DPoint *p);
	void							_Get_Line			(T3DPoint *a, T3DPoint *b, T3DPoint *c);

	void							_Get_Position		(double x, double y, double z, T3DPoint &p);
	void							_Get_Rotated		(double xAxis, double &x, double yAxis, double &y, double angle);

	bool							_Get_Panorama		(double &y, double &z);
	bool							_Get_Sinus			(double &y, double &z);
	bool							_Get_Circle			(double &y, double &z);
	bool							_Get_Hyperbel		(double &y, double &z);

	void							_Draw_Triangle		(T3DPoint p[3]);
	void							_Draw_Line			(int xa, int xb, int y, double za, double zb, double ra, double rb, double ga, double gb, double ba, double bb);
	void							_Draw_Pixel			(int x, int y, double z, BYTE r, BYTE g, BYTE b);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_3D_Image_H

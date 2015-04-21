/**********************************************************
 * Version $Id: 3d_view_projector.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_GDI                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  3d_view_projector.h                  //
//                                                       //
//                 Copyright (C) 2014 by                 //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
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
#ifndef HEADER_INCLUDED__3d_view_projector_H
#define HEADER_INCLUDED__3d_view_projector_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "sgdi_core.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SGDI_API_DLL_EXPORT CSG_3DView_Projector
{
public:
	CSG_3DView_Projector(void);

	void				Set_Center				(double x, double y, double z);
	void				Set_Scale				(double Scale);

	void				Set_Scaling				(double x, double y, double z);
	void				Set_xScaling			(double x);
	void				Set_yScaling			(double y);
	void				Set_zScaling			(double z);
	const TSG_Point_Z &	Get_Scaling				(void)	const	{	return( m_Scaling   );	}
	double				Get_xScaling			(void)	const	{	return( m_Scaling.x );	}
	double				Get_yScaling			(void)	const	{	return( m_Scaling.y );	}
	double				Get_zScaling			(void)	const	{	return( m_Scaling.z );	}

	void				Set_Rotation			(double x, double y, double z);
	void				Set_xRotation			(double x);
	void				Set_yRotation			(double y);
	void				Set_zRotation			(double z);
	const TSG_Point_Z &	Get_Rotation			(void)	const	{	return( m_Rotate   );	}
	double				Get_xRotation			(void)	const	{	return( m_Rotate.x );	}
	double				Get_yRotation			(void)	const	{	return( m_Rotate.y );	}
	double				Get_zRotation			(void)	const	{	return( m_Rotate.z );	}

	void				Set_Shift				(double x, double y, double z);
	void				Set_xShift				(double x);
	void				Set_yShift				(double y);
	void				Set_zShift				(double z);
	const TSG_Point_Z &	Get_Shift				(void)	const	{	return( m_Shift    );	}
	double				Get_xShift				(void)	const	{	return( m_Shift.x  );	}
	double				Get_yShift				(void)	const	{	return( m_Shift.y  );	}
	double				Get_zShift				(void)	const	{	return( m_Shift.z  );	}

	void				Set_Screen				(int Width, int Height);

	void				do_Central				(bool bOn = true);
	bool				is_Central				(void)	const	{	return( m_bCentral );	}
	void				Set_Central_Distance	(double Distance);
	double				Get_Central_Distance	(void)	const	{	return( m_dCentral );	}

	void				Get_Projection			(TSG_Point_Z &p);
	void				Get_Projection			(double &x, double &y, double &z);


private:

	bool				m_bCentral;

	int					m_Screen_NX, m_Screen_NY;

	double				m_dCentral, m_Scale;

	TSG_Point_Z			m_Center, m_Scaling, m_Rotate, m_Sin, m_Cos, m_Shift;


};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SGDI_API_DLL_EXPORT CSG_3DView_Canvas
{
public:
	CSG_3DView_Canvas(void);

	CSG_3DView_Projector &		Get_Projector			(void)	{	return( m_Projector );	}

	bool						Draw					(void);


protected:

	bool						m_bBox, m_bStereo;

	int							m_bgColor, m_Drape_Mode;

	double						m_dStereo;

	TSG_Point_Z					m_Data_Min, m_Data_Max;

	CSG_Grid					*m_pDrape;

	CSG_3DView_Projector		m_Projector;


	virtual bool				On_Before_Draw			(void)	{	return( true );	}
	virtual bool				On_Draw					(void)	= 0;

	virtual int					Get_Color				(double Value);
	virtual int					Dim_Color				(int Color, double dim);

	void						Set_Image				(BYTE *pRGB, int NX, int NY);

	void						Draw_Point				(int x, int y, double z, int color, int Size);
	void						Draw_Line				(double ax, double ay, double az, double bx, double by, double bz, int Color);
	void						Draw_Line				(double ax, double ay, double az, double bx, double by, double bz, int aColor, int bColor);
	void						Draw_Line				(const TSG_Point_Z &a, const TSG_Point_Z &b, int Color);
	void						Draw_Line				(const TSG_Point_Z &a, const TSG_Point_Z &b, int aColor, int bColor);

	typedef struct SSG_Triangle_Node
	{
		double	x, y, z, c, d;
	}
	TSG_Triangle_Node;

	void						Draw_Triangle			(TSG_Triangle_Node p[3], bool bValueAsColor, double Light_Dec, double Light_Azi);
	void						Draw_Triangle			(TSG_Triangle_Node p[3], bool bValueAsColor, double dim = 1.0);


private:

	int							m_Image_NX, m_Image_NY, m_Color_Mode;

	BYTE						*m_Image_pRGB;

	CSG_Matrix					m_Image_zMax;


	void						_Draw_Background		(void);
	void						_Draw_Box				(void);

	void						_Draw_Pixel				(int x, int y, double z, int color);
	void						_Draw_Triangle_Line		(int y, double a[], double b[], double dim, int mode);

	int							_Dim_Color				(int Color, double dim);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__3d_view_projector_H

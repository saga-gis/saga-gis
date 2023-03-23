
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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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

	void				Set_Scale				(double Scale);
	double				Get_Scale				(void)	const	{	return( m_Scale     );	}

	void				Set_Center				(double x, double y, double z);
	const TSG_Point_3D &	Get_Center				(void)	const	{	return( m_Center    );	}
	double				Get_xCenter				(void)	const	{	return( m_Center.x  );	}
	double				Get_yCenter				(void)	const	{	return( m_Center.y  );	}
	double				Get_zCenter				(void)	const	{	return( m_Center.z  );	}

	void				Set_Scaling				(double x, double y, double z);
	void				Set_xScaling			(double x);
	void				Set_yScaling			(double y);
	void				Set_zScaling			(double z);
	void				Inc_xScaling			(double x)		{	Set_xScaling(m_Scaling.x + x);	}
	void				Inc_yScaling			(double y)		{	Set_yScaling(m_Scaling.y + y);	}
	void				Inc_zScaling			(double z)		{	Set_zScaling(m_Scaling.z + z);	}
	const TSG_Point_3D &	Get_Scaling				(void)	const	{	return( m_Scaling   );	}
	double				Get_xScaling			(void)	const	{	return( m_Scaling.x );	}
	double				Get_yScaling			(void)	const	{	return( m_Scaling.y );	}
	double				Get_zScaling			(void)	const	{	return( m_Scaling.z );	}

	void				Set_Rotation			(double x, double y, double z, bool bDegree = false);
	void				Set_xRotation			(double x, bool bDegree = false);
	void				Set_yRotation			(double y, bool bDegree = false);
	void				Set_zRotation			(double z, bool bDegree = false);
	void				Inc_xRotation			(double x, bool bDegree = false);
	void				Inc_yRotation			(double y, bool bDegree = false);
	void				Inc_zRotation			(double z, bool bDegree = false);
	const TSG_Point_3D &	Get_Rotation			(void)					const	{	return( m_Rotate );	}
	double				Get_xRotation			(bool bDegree = false)	const	{	return( bDegree ? M_RAD_TO_DEG * m_Rotate.x : m_Rotate.x );	}
	double				Get_yRotation			(bool bDegree = false)	const	{	return( bDegree ? M_RAD_TO_DEG * m_Rotate.y : m_Rotate.y );	}
	double				Get_zRotation			(bool bDegree = false)	const	{	return( bDegree ? M_RAD_TO_DEG * m_Rotate.z : m_Rotate.z );	}

	void				Set_Shift				(double x, double y, double z);
	void				Set_xShift				(double x);
	void				Set_yShift				(double y);
	void				Set_zShift				(double z);
	void				Inc_xShift				(double x)		{	Set_xShift(m_Shift.x + x);	}
	void				Inc_yShift				(double y)		{	Set_yShift(m_Shift.y + y);	}
	void				Inc_zShift				(double z)		{	Set_zShift(m_Shift.z + z);	}
	const TSG_Point_3D &	Get_Shift				(void)	const	{	return( m_Shift    );	}
	double				Get_xShift				(void)	const	{	return( m_Shift.x  );	}
	double				Get_yShift				(void)	const	{	return( m_Shift.y  );	}
	double				Get_zShift				(void)	const	{	return( m_Shift.z  );	}

	void				Set_Screen				(int Width, int Height);

	void				do_Central				(bool bOn = true);
	bool				is_Central				(void)	const	{	return( m_bCentral );	}
	void				Set_Central_Distance	(double Distance);
	void				Inc_Central_Distance	(double Distance);
	double				Get_Central_Distance	(void)	const	{	return( m_dCentral );	}

	void				Get_Projection			(double &x, double &y, double &z)	const;
	void				Get_Projection			(      TSG_Point_3D &p)	const	{	return( Get_Projection(p.x, p.y, p.z) );	}
	void				Get_Projection			(      CSG_Point_3D &p)	const	{	return( Get_Projection(p.x, p.y, p.z) );	}
	TSG_Point_3D			Get_Projection			(const TSG_Point_3D &p)	const	{	CSG_Point_3D _p(p); Get_Projection(_p.x, _p.y, _p.z); return( _p );	}


private:

	bool				m_bCentral;

	int					m_Screen_NX, m_Screen_NY;

	double				m_dCentral, m_Scale;

	TSG_Point_3D		m_Center, m_Scaling, m_Rotate, m_Sin, m_Cos, m_Shift;


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

	int							m_bgColor, m_North, m_Labels, m_Label_Res;

	double						m_dStereo, m_BoxBuffer, m_North_Size, m_Label_Scale;

	TSG_Grid_Resampling			m_Drape_Mode;

	TSG_Point_3D					m_Data_Min, m_Data_Max;

	CSG_Grid					*m_pDrape;

	CSG_3DView_Projector		m_Projector;


	virtual bool				On_Before_Draw			(void)	{	return( true );	}
	virtual bool				On_Draw					(void)	= 0;

	virtual int					Get_Color				(double Value);
	virtual int					Dim_Color				(int Color, double Dim);

	void						Set_Image				(BYTE *pRGB, int NX, int NY);

	void						Draw_Point				(int x, int y, double z, int Color, int Size);
	void						Draw_Line				(double ax, double ay, double az, double bx, double by, double bz, int Color);
	void						Draw_Line				(double ax, double ay, double az, double bx, double by, double bz, int aColor, int bColor);
	void						Draw_Line				(const TSG_Point_3D &a, const TSG_Point_3D &b, int Color);
	void						Draw_Line				(const TSG_Point_3D &a, const TSG_Point_3D &b, int aColor, int bColor);

	typedef struct SSG_Triangle_Node
	{
		double	x, y, z, c, d;
	}
	TSG_Triangle_Node;

	void						Draw_Triangle			(TSG_Triangle_Node p[3], bool bValueAsColor, const CSG_Vector &LightSource, int Shading = 0, double zScale = 1000.);
	void						Draw_Triangle			(TSG_Triangle_Node p[3], bool bValueAsColor, double Dim = 1.);


private:

	enum
	{
		LABEL_ALIGN_TOP, LABEL_ALIGN_LEFT, LABEL_ALIGN_BOTTOM, LABEL_ALIGN_RIGHT
	};

	int							m_Image_NX, m_Image_NY, m_Color_Mode;

	BYTE						*m_Image_pRGB;

	CSG_Matrix					m_Image_zMax;


	void						_Draw_Background		(void);

	void						_Draw_Get_Box			(TSG_Point_3D Box[8], bool bProjected);
	int							_Draw_Get_Box_Front		(void);
	void						_Draw_Box				(void);

	void						_Draw_North				(void);

	void						_Draw_Labels			(int Front);
	void						_Draw_Labels			(double Min, double Max,         const TSG_Point_3D &P, double Rx, double Ry, double Rz                             , int Resolution = 100, double Scale = 1.);
	void						_Draw_Labels			(int Axis, const TSG_Point_3D &A, const TSG_Point_3D &B, double Rx, double Ry, double Rz, int Align = LABEL_ALIGN_TOP, int Resolution = 100, double Scale = 1.);
	void						_Draw_Label				(const CSG_String &Text,         const TSG_Point_3D &P, double Rx, double Ry, double Rz, int Align = LABEL_ALIGN_TOP, int Resolution = 100, double Scale = 1.);

	void						_Draw_Image				(class wxImage &Image, const CSG_Vector &Move, const CSG_Matrix &Rotate, int BGColor = -1, int xOffset = 0, int yOffset = 0);

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

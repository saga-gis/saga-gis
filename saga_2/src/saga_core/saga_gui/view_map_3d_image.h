
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 VIEW_Map_3D_Image.h                   //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Map_3D_Image_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Map_3D_Image_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/image.h>

#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum
{
	mode3d_Normal			= 0,
	mode3d_Triangle,
	mode3d_Stereo
}
TRender_3DMode;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Map_3D_Image
{
public:
	CVIEW_Map_3D_Image(class CVIEW_Map_3D *pParent, class CWKSP_Map *pMap);
	virtual ~CVIEW_Map_3D_Image(void);

	bool							m_bCentral, m_bStereo, m_bInterpol, m_Src_bUpdate;

	int								m_img_nx, m_img_ny, m_Resolution, m_nxPoints, m_nyPoints, m_BkColor, m_Figure;

	double							m_xRotate, m_yRotate, m_zRotate,
									m_xShift, m_yShift, m_zShift,
									m_Range, m_Exaggeration, m_Central, m_Stereo,
									m_Figure_Weight;

	CSG_Grid							*m_pDEM;


	void							Set_Source				(int Resolution = -1);

	void							Set_Image				(int NX, int NY);
	void							Set_Image				(void);

	wxImage &						Get_Image				(void)	{	return( m_img );	}

	void							Save					(void);
	void							Save					(const char *file, int type);


private:

	typedef struct
	{
		BYTE						r, g, b, Flags;

		int							x, y;

		float						z, zDEM;
	}
	TPoint;


	float							**m_img_z, m_Missing;

	double							r_sin_x, r_sin_y, r_sin_z,
									r_cos_x, r_cos_y, r_cos_z,
									r_kx, r_ky, r_m, r_ext, r_fig,
									m_xyRatio;

	TPoint							**m_Points;

	wxImage							m_img;

	class CWKSP_Map					*m_pMap;

	class CVIEW_Map_3D				*m_pParent;


	void							_Rotate_Matrix			(double xRotate, double yRotate, double zRotate);
	bool							_Rotate_Point			(double x, double y, double z, double &px, double &py, double &pz);

	void							_Draw_Image				(void);
	void							_Draw_Triangle			(TPoint p[3]);
	void							_Draw_Line				(int xa, int xb, int y, double za, double zb, BYTE r, BYTE g, BYTE b);
	void							_Draw_Triangle_i		(TPoint p[3]);
	void							_Draw_Line_i			(int xa, int xb, int y, double za, double zb, double ra, double rb, double ga, double gb, double ba, double bb);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Map_3D_Image_H

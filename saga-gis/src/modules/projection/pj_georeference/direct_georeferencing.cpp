/**********************************************************
 * Version $Id: direct_georeferencing.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    georeferencing                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                direct_georeferencing.cpp              //
//                                                       //
//                 Copyright (C) 2012 by                 //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "direct_georeferencing.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDirect_Georeferencing::CDirect_Georeferencing(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Direct Georeferencing of Airborne Photographs"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"Direct georeferencing of aerial photographs uses extrinsic "
		"(position, attitude) and intrinsic (focal length, physical "
		"pixel size) camera parameters. Orthorectification routine supports "
		"additional data from a Digital Elevation Model (DEM).\n"
		"\nReferences:\n"
		"Baumker, M. / Heimes, F.J. (2001): "
		"New Calibration and Computing Method for Direct Georeferencing of Image and Scanner Data Using the Position and Angular Data of an Hybrid Inertial Navigation System. "
		"OEEPE Workshop, Integrated Sensor Orientation, Hannover 2001. "
		"<a target=\"_blank\" href=\"http://www.hochschule-bochum.de/fileadmin/media/fb_v/veroeffentlichungen/baeumker/baheimesoeepe.pdf\">online</a>.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "INPUT"		, _TL("Unreferenced Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "OUTPUT"		, _TL("Referenced Grids"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	Parameters.Add_Shapes(
		NULL	, "EXTENT"		, _TL("Extent"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "NODE_POS"	, _TL("Position"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "X"			, _TL("X"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.0
	);

	Parameters.Add_Value(
		pNode	, "Y"			, _TL("Y"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.0
	);

	Parameters.Add_Value(
		pNode	, "Z"			, _TL("Flying Height"),
		_TL(""),
		PARAMETER_TYPE_Double	, 1000.0
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "NODE_DIR"	, _TL("Orientation"),
		_TL("")
	);

	Parameters.Add_Choice(
		pNode	, "ORIENTATION"	, _TL("Orientation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("BLUH"),
			_TL("PATB")
		), 0
	);

	Parameters.Add_Value(
		pNode	, "OMEGA"		, _TL("Omega [degree]"),
		_TL("rotation around the X axis (roll)"),
		PARAMETER_TYPE_Double	, 0.0
	);

	Parameters.Add_Value(
		pNode	, "PHI"			, _TL("Phi [degree]"),
		_TL("rotation around the Y axis (pitch)"),
		PARAMETER_TYPE_Double	, 0.0
	);

	Parameters.Add_Value(
		pNode	, "KAPPA"		, _TL("Kappa [degree]"),
		_TL("rotation around the Z axis (heading)"),
		PARAMETER_TYPE_Double	, 0.0
	);

	Parameters.Add_Value(
		pNode	, "KAPPA_OFF"	, _TL("Kappa Offset [degree]"),
		_TL("origin adjustment for Z axis (heading)"),
		PARAMETER_TYPE_Double	, 90.0
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "NODE_CAMERA"	, _TL("Camera"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "CFL"			, _TL("Focal Length [mm]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 80, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "PXSIZE"		, _TL("CCD Physical Pixel Size [micron]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 5.2, 0.0, true
	);

	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "ZREF"		, _TL("Reference Height"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.0
	);

	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, false
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "INTERPOLATION"	, _TL("Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Nearest Neigbhor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);

	Parameters.Add_Choice(
		NULL	, "DATA_TYPE"	, _TL("Data Storage Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			_TL("1 byte unsigned integer"),
			_TL("1 byte signed integer"),
			_TL("2 byte unsigned integer"),
			_TL("2 byte signed integer"),
			_TL("4 byte unsigned integer"),
			_TL("4 byte signed integer"),
			_TL("4 byte floating point"),
			_TL("8 byte floating point"),
			_TL("same as original")
		), 8
	);

	Parameters.Add_Choice(
		NULL	, "TARGET_TYPE"	, _TL("Target"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("user defined grid system"),
			_TL("existing grid system")
		), 0
	);

	m_Grid_Target.Create(Add_Parameters("TARGET", _TL("Target Grid System"), _TL("")), false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDirect_Georeferencing::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( m_Grid_Target.On_Parameter_Changed(pParameters, pParameter) ? 1 : 0 );
}

//---------------------------------------------------------
int CDirect_Georeferencing::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( m_Grid_Target.On_Parameters_Enable(pParameters, pParameter) ? 1 : 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDirect_Georeferencing::On_Execute(void)
{
	//-----------------------------------------------------
	if( !Set_Transformation() )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pDEM			= Parameters("DEM"          )->asGrid();
	double		zRef			= Parameters("ZREF"         )->asDouble();
	int			Interpolation	= Parameters("INTERPOLATION")->asInt();

	//-----------------------------------------------------
	TSG_Point	p[4];

	p[0]	= Image_to_World(       0,        0, zRef);
	p[1]	= Image_to_World(Get_NX(),        0, zRef);
	p[2]	= Image_to_World(Get_NX(), Get_NY(), zRef);
	p[3]	= Image_to_World(       0, Get_NY(), zRef);

	CSG_Rect	r(p[0], p[1]);
	r.Union(p[2]);
	r.Union(p[3]);

	//-----------------------------------------------------
	CSG_Shapes	*pShapes	= Parameters("EXTENT")->asShapes();

	if( pShapes )
	{
		pShapes->Create(SHAPE_TYPE_Polygon, _TL("Extent"));
		pShapes->Add_Field(_TL("OID"), SG_DATATYPE_Int);

		CSG_Shape	*pExtent	= pShapes->Add_Shape();

		pExtent->Add_Point(p[0]);
		pExtent->Add_Point(p[1]);
		pExtent->Add_Point(p[2]);
		pExtent->Add_Point(p[3]);
	}

	//-----------------------------------------------------
	double	Cellsize	= SG_Get_Distance(p[0], p[1]) / Get_NX();

	CSG_Grid_System	System(Cellsize, r);

	m_Grid_Target.Set_User_Defined(Get_Parameters("TARGET"), r, Get_NX());
//	m_Grid_Target.Set_User_Defined(Get_Parameters("TARGET"), r, (int)(1 + r.Get_YRange() / Cellsize));

	if( !Dlg_Parameters("TARGET") )
	{
		return( false );
	}

	System	= m_Grid_Target.Get_System();

	if( !System.is_Valid() )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pInput		= Parameters("INPUT" )->asGridList();
	CSG_Parameter_Grid_List	*pOutput	= Parameters("OUTPUT")->asGridList();

	pOutput->Del_Items();

	if( pInput->Get_Count() <= 0 )
	{
		return( false );
	}
	else
	{
		TSG_Data_Type	Type;

		switch( Parameters("DATA_TYPE")->asInt() )
		{
		case 0:		Type	= SG_DATATYPE_Byte;			break;
		case 1:		Type	= SG_DATATYPE_Char;			break;
		case 2:		Type	= SG_DATATYPE_Word;			break;
		case 3:		Type	= SG_DATATYPE_Short;		break;
		case 4:		Type	= SG_DATATYPE_DWord;		break;
		case 5:		Type	= SG_DATATYPE_Int;			break;
		case 6: 	Type	= SG_DATATYPE_Float;		break;
		case 7:		Type	= SG_DATATYPE_Double;		break;
		default:	Type	= SG_DATATYPE_Undefined;	break;
		}

		for(int i=0; i<pInput->Get_Count(); i++)
		{
			CSG_Grid	*pGrid	= SG_Create_Grid(System, Type != SG_DATATYPE_Undefined ? Type : pInput->asGrid(i)->Get_Type());

			if( !pGrid || !pGrid->is_Valid() )
			{
				if( pGrid )
				{
					delete(pGrid);
				}

				return( false );
			}

			pOutput->Add_Item(pGrid);

			pGrid->Set_Name(pInput->asGrid(i)->Get_Name());
		}
	}

	//-----------------------------------------------------
	for(int y=0; y<System.Get_NY() && Set_Progress(y, System.Get_NY()); y++)
	{
		double	py	= System.Get_YMin() + y * System.Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<System.Get_NX(); x++)
		{
			double	pz, px	= System.Get_XMin() + x * System.Get_Cellsize();

			if( !pDEM || !pDEM->Get_Value(px, py, pz) )
			{
				pz	= zRef;
			}

			TSG_Point	p	= World_to_Image(px, py, pz);

			for(int i=0; i<pInput->Get_Count(); i++)
			{
				if( pInput->asGrid(i)->Get_Value(p.x, p.y, pz, Interpolation) )
				{
					pOutput->asGrid(i)->Set_Value(x, y, pz);
				}
				else
				{
					pOutput->asGrid(i)->Set_NoData(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDirect_Georeferencing::Set_Transformation(void)
{
	//-----------------------------------------------------
	m_O.Create(2);
	m_O[0]	= Get_NX() / 2.0;
	m_O[1]	= Get_NY() / 2.0;

	m_f		= Parameters("CFL"   )->asDouble() / 1000;		// [mm]     -> [m]
	m_s		= Parameters("PXSIZE")->asDouble() / 1000000;	// [micron] -> [m]

	//-----------------------------------------------------
	m_T.Create(3);

	m_T[0]	= Parameters("X")->asDouble();
	m_T[1]	= Parameters("Y")->asDouble();
	m_T[2]	= Parameters("Z")->asDouble();

	//-----------------------------------------------------
	double		a;
	CSG_Matrix	Rx(3, 3), Ry(3, 3), Rz(3, 3);

	a	= Parameters("OMEGA" )->asDouble() * M_DEG_TO_RAD;
	Rx[0][0] =       1; Rx[0][1] =       0; Rx[0][2] =       0;
	Rx[1][0] =       0; Rx[1][1] =  cos(a); Rx[1][2] = -sin(a);
	Rx[2][0] =       0; Rx[2][1] =  sin(a); Rx[2][2] =  cos(a);

	a	= Parameters("PHI"   )->asDouble() * M_DEG_TO_RAD;
	Ry[0][0] =  cos(a); Ry[0][1] =       0; Ry[0][2] =  sin(a);
	Ry[1][0] =       0; Ry[1][1] =       1; Ry[1][2] =       0;
	Ry[2][0] = -sin(a); Ry[2][1] =       0; Ry[2][2] =  cos(a);

	a	= Parameters("KAPPA" )->asDouble() * M_DEG_TO_RAD + Parameters("KAPPA_OFF" )->asDouble() * M_DEG_TO_RAD;
	Rz[0][0] =  cos(a); Rz[0][1] = -sin(a); Rz[0][2] =       0;
	Rz[1][0] =  sin(a); Rz[1][1] =  cos(a); Rz[1][2] =       0;
	Rz[2][0] =       0; Rz[2][1] =       0; Rz[2][2] =       1;

	switch( Parameters("ORIENTATION")->asInt() )
	{
	case 0:	default:	m_R	= Rz * Rx * Ry;	break;	// BLUH
	case 1:				m_R	= Rx * Ry * Rz;	break;	// PATB
	}

	m_Rinv	= m_R.Get_Inverse();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline TSG_Point CDirect_Georeferencing::World_to_Image(double x_w, double y_w, double z_w)
{
	TSG_Point	p;
	CSG_Vector	Pw(3), Pc;
	
	Pw[0]	= x_w;
	Pw[1]	= y_w;
	Pw[2]	= z_w;

	Pc		= m_Rinv * (Pw - m_T);

	p.x		= m_O[0] - (m_f / m_s) * (Pc[0] / Pc[2]);
	p.y		= m_O[1] - (m_f / m_s) * (Pc[1] / Pc[2]);

	p.x		= Get_XMin() + p.x * Get_Cellsize();
	p.y		= Get_YMin() + p.y * Get_Cellsize();

	return( p );
}

//---------------------------------------------------------
inline TSG_Point CDirect_Georeferencing::Image_to_World(double x_i, double y_i, double z_w)
{
	double		k;
	TSG_Point	p;
	CSG_Vector	Pc(3), Pw;
	
	Pc[0]	= (m_O[0] - x_i) * m_s;
	Pc[1]	= (m_O[1] - y_i) * m_s;
	Pc[2]	= m_f;

	Pw		= m_R * Pc;

	k		= (z_w - m_T[2]) / Pw[2];

	p.x		= m_T[0] + k * Pw[0];
	p.y		= m_T[1] + k * Pw[1];

	return( p );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

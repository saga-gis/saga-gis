
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                                                       //
//                       io_gdal                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   gdal_import.cpp                     //
//                                                       //
//            Copyright (C) 2007 O. Conrad               //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Bundesstr. 55                          //
//                D-20146 Hamburg                        //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "gdal_import.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGDAL_Import::CGDAL_Import(void)
{
	Set_Name	(_TL("GDAL: Import Raster"));

	Set_Author	(SG_T("(c) 2007 by O.Conrad (A.Ringeler)"));

	CSG_String	Description;

	Description	= _TW(
		"The \"GDAL Raster Import\" module imports grid data from various file formats using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
		"For more information have a look at the GDAL homepage:\n"
		"  <a target=\"_blank\" href=\"http://www.gdal.org/\">"
		"  http://www.gdal.org</a>\n"
		"\n"
		"Following raster formats are currently supported:\n"
		"<table border=\"1\"><tr><th>ID</th><th>Name</th></tr>\n"
	);

	for(int i=0; i<g_GDAL_Driver.Get_Count(); i++)
    {
		Description	+= CSG_String::Format(SG_T("<tr><td>%s</td><td>%s</td></tr>\n"),
			SG_STR_MBTOSG(g_GDAL_Driver.Get_Description(i)),
			SG_STR_MBTOSG(g_GDAL_Driver.Get_Name(i))
		);
    }

	Description	+= SG_T("</table>");

	Set_Description(Description);

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL, "GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, false
	);

	Parameters.Add_FilePath(
		NULL, "FILES"	, _TL("Files"),
		_TL(""),
		NULL, NULL, false, false, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import::On_Execute(void)
{
	CSG_Strings		Files;
	CGDAL_System	System;

	//-----------------------------------------------------
	if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_pGrids	= Parameters("GRIDS")	->asGridList();
	m_pGrids	->Del_Items();

	for(int i=0; i<Files.Get_Count(); i++)
	{
		Message_Add(CSG_String::Format(SG_T("%s: %s"), _TL("loading"), Files[i].c_str()));

		if( System.Create(Files[i], IO_READ) == false )
		{
			Message_Add(_TL("failed: could not find a suitable import driver"));
		}
		else
		{
			if( System.Get_Count() <= 0 )
			{
				Load_Sub(System, SG_File_Get_Name(Files[i], false));
			}
			else
			{
				Load(System, SG_File_Get_Name(Files[i], false));
			}
		}
	}

	return( m_pGrids->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import::Load_Sub(CGDAL_System &System, const CSG_String &Name)
{
	if( System.is_Reading() )
	{
		char	**pMetaData	= System.Get_DataSet()->GetMetadata("SUBDATASETS");

		if( CSLCount(pMetaData) > 0 )
		{
			int				i, n;
			CSG_String		s, sID, sName, sDesc;
			CSG_Parameters	P;

			for(i=0; pMetaData[i]!=NULL; i++)
			{
				Message_Add(CSG_String::Format(SG_T("  %s\n"), pMetaData[i]), false);

				s		= pMetaData[i];

				if( s.Contains(SG_T("SUBDATASET_")) && s.Contains(SG_T("_NAME=")) )
				{
					sID		= s.AfterFirst('_').BeforeFirst('_');
					sName	= s.AfterFirst('=');
					sDesc	= _TL("no description available");

					if( pMetaData[i + 1] != NULL )
					{
						s		= pMetaData[i + 1];

						if( s.Contains(SG_T("SUBDATASET_")) && s.Contains(SG_T("_DESC")) )
						{
							sDesc	= s.AfterFirst ('=');
						}
					}

					P.Add_Value(NULL, sName, sDesc, SG_T(""), PARAMETER_TYPE_Bool, false);
				}
			}

			if( Dlg_Parameters(&P, _TL("Select from Subdatasets...")) )
			{
				for(i=0, n=0; i<P.Get_Count() && Process_Get_Okay(false); i++)
				{
					if( P(i)->asBool() && System.Create(P(i)->Get_Identifier(), IO_READ) && Load(System, P(i)->Get_Name()) )
					{
						n++;
					}
				}

				return( n > 0 );
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import::Load(CGDAL_System &System, const CSG_String &Name)
{
	//-----------------------------------------------------
	if( !System.is_Reading() )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Vector	A;
	CSG_Matrix	B;

	System.Get_Transform(A, B);

	//-----------------------------------------------------
	Message_Add(CSG_String::Format(
		SG_T("\n%s: %s/%s\n"),
		_TL("Driver"),
		System.Get_Driver()->GetDescription(), 
		System.Get_Driver()->GetMetadataItem(GDAL_DMD_LONGNAME)
	), false);

	Message_Add(CSG_String::Format(
		SG_T("%s: x %d, y %d\n%s: %d\n%s x' = %.6f + x * %.6f + y * %.6f\n%s y' = %.6f + x * %.6f + y * %.6f"),
		_TL("Cells")			, System.Get_NX(), System.Get_NY(),
		_TL("Bands")			, System.Get_Count(),
		_TL("Transformation")	, A[0], B[0][0], B[0][1],
		_TL("Transformation")	, A[1], B[1][0], B[1][1]
	), false);

	//-----------------------------------------------------
	int			i, n;
	CSG_Grid	*pGrid;

	for(i=0, n=0; i<System.Get_Count() && Process_Get_Okay(); i++)
	{
		if( (pGrid = System.Read_Band(i)) != NULL )
		{
			n++;

			if( System.Needs_Transform() )
			{
				Set_Transformation(&pGrid, A, B);
			}

			pGrid->Set_Name(System.Get_Count() > 1
				? CSG_String::Format(SG_T("%s [%s]"), Name.c_str(), pGrid->Get_Name()).c_str()
				: Name.c_str()
			);

			pGrid->Get_Projection().Create(System.Get_Projection(), SG_PROJ_FMT_WKT);
			m_pGrids->Add_Item(pGrid);

			DataObject_Add			(pGrid);
			DataObject_Set_Colors	(pGrid, CSG_Colors(100, SG_COLORS_BLACK_WHITE, false));
		}
    }

	//-----------------------------------------------------
	return( n > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGDAL_Import::Set_Transformation(CSG_Grid **ppGrid, const CSG_Vector &A, const CSG_Matrix &B)
{
	//-----------------------------------------------------
	int			x, y;
	double		z;
	TSG_Rect	r;
	CSG_Vector	vImage(2), vWorld(2);
	CSG_Matrix	BInv(B.Get_Inverse());
	CSG_Grid	*pImage, *pWorld;

	//-----------------------------------------------------
	pImage		= *ppGrid;

	//-----------------------------------------------------
	vImage[0]	= pImage->Get_XMin();	vImage[1]	= pImage->Get_YMin();	vWorld	= B * vImage + A;
	r.xMin	= r.xMax	= vWorld[0];
	r.yMin	= r.yMax	= vWorld[1];

	vImage[0]	= pImage->Get_XMin();	vImage[1]	= pImage->Get_YMax();	vWorld	= B * vImage + A;
	if( r.xMin > vWorld[0] )	r.xMin	= vWorld[0];	else if( r.xMax < vWorld[0] )	r.xMax	= vWorld[0];
	if( r.yMin > vWorld[1] )	r.yMin	= vWorld[1];	else if( r.yMax < vWorld[1] )	r.yMax	= vWorld[1];

	vImage[0]	= pImage->Get_XMax();	vImage[1]	= pImage->Get_YMax();	vWorld	= B * vImage + A;
	if( r.xMin > vWorld[0] )	r.xMin	= vWorld[0];	else if( r.xMax < vWorld[0] )	r.xMax	= vWorld[0];
	if( r.yMin > vWorld[1] )	r.yMin	= vWorld[1];	else if( r.yMax < vWorld[1] )	r.yMax	= vWorld[1];

	vImage[0]	= pImage->Get_XMax();	vImage[1]	= pImage->Get_YMin();	vWorld	= B * vImage + A;
	if( r.xMin > vWorld[0] )	r.xMin	= vWorld[0];	else if( r.xMax < vWorld[0] )	r.xMax	= vWorld[0];
	if( r.yMin > vWorld[1] )	r.yMin	= vWorld[1];	else if( r.yMax < vWorld[1] )	r.yMax	= vWorld[1];

	z	= fabs(B[0][0]) < fabs(B[1][1]) ? fabs(B[0][0]) : fabs(B[1][1]);	// guess a suitable cellsize; could be improved...
	x	= 1 + (int)((r.xMax - r.xMin) / z);
	y	= 1 + (int)((r.yMax - r.yMin) / z);

	//-----------------------------------------------------
	pWorld		= *ppGrid	= SG_Create_Grid(pImage->Get_Type(), x, y, z, r.xMin, r.yMin);

	for(y=0, vWorld[1]=pWorld->Get_YMin(); y<pWorld->Get_NY() && Set_Progress(y, pWorld->Get_NY()); y++, vWorld[1]+=pWorld->Get_Cellsize())
	{
		for(x=0, vWorld[0]=pWorld->Get_XMin(); x<pWorld->Get_NX(); x++, vWorld[0]+=pWorld->Get_Cellsize())
		{
			vImage	= BInv * (vWorld - A);

			if( pImage->Get_Value(vImage[0], vImage[1], z, GRID_INTERPOLATION_NearestNeighbour, false, true) )
			{
				pWorld->Set_Value(x, y, z);
			}
			else
			{
				pWorld->Set_NoData(x, y);
			}
		}
	}

	delete(pImage);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

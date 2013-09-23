/**********************************************************
 * Version $Id$
 *********************************************************/

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
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name	(_TL("GDAL: Import Raster"));

	Set_Author	(SG_T("O.Conrad (c) 2007 (A.Ringeler)"));

	CSG_String	Description;

	Description	= _TW(
		"The \"GDAL Raster Import\" module imports grid data from various file formats using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
		"For more information have a look at the GDAL homepage:\n"
		"  <a target=\"_blank\" href=\"http://www.gdal.org/\">"
		"  http://www.gdal.org</a>\n"
	);

	Description	+= CSG_String::Format(SG_T("\nGDAL %s:%s\n\n"), _TL("Version"), SG_Get_GDAL_Drivers().Get_Version().c_str());

	Description	+= _TW(
		"Following raster formats are currently supported:\n"
		"<table border=\"1\"><tr><th>ID</th><th>Name</th></tr>\n"
	);

	for(int i=0; i<SG_Get_GDAL_Drivers().Get_Count(); i++)
    {
		Description	+= CSG_String::Format(SG_T("<tr><td>%s</td><td>%s</td></tr>\n"),
			SG_Get_GDAL_Drivers().Get_Description(i).c_str(),
			SG_Get_GDAL_Drivers().Get_Name       (i).c_str()
		);
    }

	Description	+= SG_T("</table>");

	Set_Description(Description);

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	Parameters.Add_FilePath(
		NULL	, "FILES"		, _TL("Files"),
		_TL(""),
		NULL, NULL, false, false, true
	);

	//-----------------------------------------------------
	if( SG_UI_Get_Window_Main() )
	{
		Parameters.Add_Value(
			NULL	, "SELECT"		, _TL("Select from Multiple Bands"),
			_TL(""),
			PARAMETER_TYPE_Bool, true
		);
	}

	//-----------------------------------------------------
	pNode	= Parameters.Add_Value(
		NULL	, "TRANSFORM"	, _TL("Transformation"),
		_TL("align grid to coordinate system"),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Choice(
		pNode	, "INTERPOL"	, _TL("Interpolation"),
		_TL("interpolation method to use if grid needs to be aligned to coordinate system"),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);

	//-----------------------------------------------------
	Add_Parameters("SELECTION", _TL("Select from Multiple Bands"), _TL(""));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGDAL_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("TRANSFORM")) )
	{
		pParameters->Get_Parameter("INTERPOL")->Set_Enabled(pParameter->asBool());
	}

	if( !SG_STR_CMP(pParameters->Get_Identifier(), SG_T("SELECTION"))
	&&  !SG_STR_CMP(pParameter ->Get_Identifier(), SG_T("ALL")) && pParameters->Get_Parameter("BANDS") )
	{
		pParameters->Get_Parameter("BANDS")->Set_Enabled(!pParameter->asBool());
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import::On_Execute(void)
{
	CSG_Strings			Files;
	CSG_GDAL_DataSet	DataSet;

	//-----------------------------------------------------
	if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_pGrids	= Parameters("GRIDS")->asGridList();
	m_pGrids	->Del_Items();

	for(int i=0; i<Files.Get_Count(); i++)
	{
		Message_Add(CSG_String::Format(SG_T("\n%s: %s"), _TL("loading"), Files[i].c_str()), false);

		if( DataSet.Open_Read(Files[i]) == false )
		{
			Message_Add(_TL("failed: could not find a suitable import driver"));
		}
		else
		{
			Load(DataSet, SG_File_Get_Name(Files[i], false));
		}
	}

	return( m_pGrids->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import::Load_Sub(CSG_GDAL_DataSet &DataSet)
{
	CSG_MetaData	MetaData;

	if( !DataSet.Get_MetaData(MetaData, "SUBDATASETS") )
	{
		return( false );
	}

	//-----------------------------------------------------
	int				i, n;
	CSG_Parameters	P;

	for(i=0, n=0; i==n; i++)
	{
		CSG_MetaData	*pName	= MetaData.Get_Child(CSG_String::Format(SG_T("SUBDATASET_%d_NAME"), i + 1));
		CSG_MetaData	*pDesc	= MetaData.Get_Child(CSG_String::Format(SG_T("SUBDATASET_%d_DESC"), i + 1));

		if( pName )
		{
			n++;

			Message_Add(CSG_String::Format(SG_T("\n%s"), pName->Get_Content().c_str()), false);

			P.Add_Value(NULL, pName->Get_Content(), pDesc ? pDesc->Get_Content().c_str() : _TL("unnamed"), SG_T(""), PARAMETER_TYPE_Bool, SG_UI_Get_Window_Main() == NULL);
		}
	}

	if( SG_UI_Get_Window_Main() && !Dlg_Parameters(&P, _TL("Select from Subdatasets...")) )	// with gui
	{
		return( false );
	}

	//-----------------------------------------------------
	for(i=0, n=0; i<P.Get_Count() && Process_Get_Okay(false); i++)
	{
		if( P(i)->asBool() && DataSet.Open_Read(P(i)->Get_Identifier()) && Load(DataSet, P(i)->Get_Name()) )
		{
			n++;
		}
	}

	//---------------------------------------------
	return( n > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import::Load(CSG_GDAL_DataSet &DataSet, const CSG_String &Name)
{
	if( !DataSet.is_Reading() )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( DataSet.Get_Count() <= 0 )
	{
		return( Load_Sub(DataSet) );
	}

	//-----------------------------------------------------
	CSG_Vector	A;
	CSG_Matrix	B;

	DataSet.Get_Transform(A, B);

	//-----------------------------------------------------
	Message_Add( 
		SG_T("Driver: ") 
		+ DataSet.Get_Description()
		+ SG_T("/")
		+ DataSet.Get_Name(),
		 false);

	if( DataSet.Get_Count() > 1 )
	{
		Message_Add(CSG_String::Format(SG_T("%d %s\n"), DataSet.Get_Count(), _TL("Bands")), false);
	}

	Message_Add(CSG_String::Format(
		SG_T("%s: x %d, y %d\n%s: %d\n%s x' = %.6f + x * %.6f + y * %.6f\n%s y' = %.6f + x * %.6f + y * %.6f"),
		_TL("Cells")			, DataSet.Get_NX(), DataSet.Get_NY(),
		_TL("Bands")			, DataSet.Get_Count(),
		_TL("Transformation")	, A[0], B[0][0], B[0][1],
		_TL("Transformation")	, A[1], B[1][0], B[1][1]
	), false);

	//-----------------------------------------------------
	int			i, n;
	CSG_Table	Bands;

	Bands.Add_Field("NAME", SG_DATATYPE_String);

	for(i=0; i<DataSet.Get_Count(); i++)
	{
		Bands.Add_Record()->Set_Value(0, DataSet.Get_Name(i));
	}

	Bands.Set_Index(0, TABLE_INDEX_Ascending);

	//-----------------------------------------------------
	if( Parameters("SELECT") && Parameters("SELECT")->asBool() && DataSet.Get_Count() > 1 )
	{
		CSG_Parameters	*pSelection	= Get_Parameters("SELECTION");
		pSelection->Add_Value(NULL, "ALL", _TL("Load all bands"), _TL(""), PARAMETER_TYPE_Bool, false);
		CSG_Parameter	*pNode	= pSelection->Add_Node(NULL, "BANDS", _TL("Bands"), _TL(""));

		for(i=0; i<Bands.Get_Count(); i++)
		{
			CSG_Table_Record	*pBand	= Bands.Get_Record_byIndex(i);

			pSelection->Add_Value(pNode, SG_Get_String(i, 0), pBand->asString(0), _TL(""), PARAMETER_TYPE_Bool, false);
		}

		if( Dlg_Parameters("SELECTION") )
		{
			for(i=0; i<Bands.Get_Count(); i++)
			{
				if( pSelection->Get_Parameter(0)->asBool() || pSelection->Get_Parameter(i + 2)->asBool() )
				{
					Bands.Select(Bands.Get_Record_byIndex(i)->Get_Index(), true);
				}
			}
		}

		pSelection->Del_Parameters();

		if( Bands.Get_Selection_Count() <= 0 )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	CSG_Grid_System	Transform;

	if( Parameters("TRANSFORM")->asBool() && DataSet.Needs_Transform() )
	{
		double		s;
		CSG_Vector	v(2);
		CSG_Rect	r;

		v[0]	= DataSet.Get_xMin();	v[1]	= DataSet.Get_yMin();	v	= B * v + A;	r.Assign(v[0], v[1], v[0], v[1]);
		v[0]	= DataSet.Get_xMin();	v[1]	= DataSet.Get_yMax();	v	= B * v + A;	r.Union(CSG_Point(v[0], v[1]));
		v[0]	= DataSet.Get_xMax();	v[1]	= DataSet.Get_yMax();	v	= B * v + A;	r.Union(CSG_Point(v[0], v[1]));
		v[0]	= DataSet.Get_xMax();	v[1]	= DataSet.Get_yMin();	v	= B * v + A;	r.Union(CSG_Point(v[0], v[1]));

		v[0]	= 0;	v[1] = 1;	v = B * v;	s	= fabs(v.Get_Length());
		v[0]	= 1;	v[1] = 0;	v = B * v;

		if( s != fabs(v.Get_Length()) )
		{
			if( s > fabs(v.Get_Length()) )
			{
				s	= fabs(v.Get_Length());
			}

			Message_Add(CSG_String::Format(SG_T("\n%s: %s\n\t%s: %f\n"), _TL("warning"), _TL("top-to-bottom and left-to-right cell sizes differ."), _TL("using cellsize"), s), false);
		}

		Transform.Assign(s, r);
	}

	//-----------------------------------------------------
	for(i=0, n=0; i<DataSet.Get_Count() && Process_Get_Okay(); i++)
	{
		CSG_Table_Record	*pBand	= Bands.Get_Record_byIndex(i);

		if( !Bands.Get_Selection_Count() || pBand->is_Selected() )
		{
			Process_Set_Text(CSG_String::Format(SG_T("%s [%d/%d]"), _TL("loading band"), i + 1, DataSet.Get_Count()));

			CSG_Grid	*pGrid	= DataSet.Read(pBand->Get_Index());

			if( pGrid != NULL )
			{
				n++;

				if( Transform.is_Valid() )
				{
					Process_Set_Text(CSG_String::Format(SG_T("%s [%d/%d]"), _TL("band transformation"), i + 1, DataSet.Get_Count()));

					Set_Transformation(&pGrid, Transform, A, B);
				}

				pGrid->Set_Name(DataSet.Get_Count() > 1
					? CSG_String::Format(SG_T("%s [%s]"), Name.c_str(), pGrid->Get_Name()).c_str()
					: Name.c_str()
				);

				m_pGrids->Add_Item(pGrid);

				DataObject_Add			(pGrid);
				DataObject_Set_Colors	(pGrid, CSG_Colors(100, SG_COLORS_BLACK_WHITE, false));
			}
		}
    }

	//-----------------------------------------------------
	return( n > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGDAL_Import::Set_Transformation(CSG_Grid **ppGrid, const CSG_Grid_System &System, const CSG_Vector &A, const CSG_Matrix &B)
{
	//-----------------------------------------------------
	TSG_Grid_Interpolation	Interpolation;

	switch( Parameters("INTERPOL")->asInt() )
	{
	default:
	case 0:	Interpolation	= GRID_INTERPOLATION_NearestNeighbour;	break;
	case 1:	Interpolation	= GRID_INTERPOLATION_Bilinear;			break;
	case 2:	Interpolation	= GRID_INTERPOLATION_InverseDistance;	break;
	case 3:	Interpolation	= GRID_INTERPOLATION_BicubicSpline;		break;
	case 4:	Interpolation	= GRID_INTERPOLATION_BSpline;			break;
	}

	//-----------------------------------------------------
	CSG_Matrix	BInv(B.Get_Inverse());

	CSG_Grid	*pImage	= *ppGrid;
	CSG_Grid	*pWorld	= *ppGrid	= SG_Create_Grid(System, pImage->Get_Type());

	pWorld->Set_Name              (pImage->Get_Name       ());
	pWorld->Set_Description       (pImage->Get_Description());
	pWorld->Set_Unit              (pImage->Get_Unit       ());
	pWorld->Set_ZFactor           (pImage->Get_ZFactor    ());
	pWorld->Set_NoData_Value_Range(pImage->Get_NoData_Value(), pImage->Get_NoData_hiValue());
	pWorld->Get_MetaData()	     = pImage->Get_MetaData   ();
	pWorld->Get_Projection()     = pImage->Get_Projection ();

	//-----------------------------------------------------
//	#pragma omp parallel for
//	for(int y=0; y<pWorld->Get_NY(); y++)
//	{
//		Process_Get_Okay();

	for(int y=0; y<pWorld->Get_NY() && Set_Progress(y, pWorld->Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<pWorld->Get_NX(); x++)
		{
			double		z;
			CSG_Vector	vWorld(2), vImage;

			vWorld[0]	= pWorld->Get_XMin() + x * pWorld->Get_Cellsize();
			vWorld[1]	= pWorld->Get_YMin() + y * pWorld->Get_Cellsize();

			vImage	= BInv * (vWorld - A);

			if( pImage->Get_Value(vImage[0], vImage[1], z, Interpolation, false, false, true) )
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
bool	SG_GDAL_Import	(const CSG_String &File_Name)
{
	CGDAL_Import	Import;

	if(	!Import.Get_Parameters()->Set_Parameter(SG_T("FILES"), File_Name, PARAMETER_TYPE_FilePath) )
	{
		return( false );
	}

	if(	!Import.Execute() )
	{
		return( false );
	}

	CSG_Parameter_Grid_List	*pGrids	= Import.Get_Parameters()->Get_Parameter(SG_T("GRIDS"))->asGridList();

	for(int i=0; i<pGrids->Get_Count(); i++)
	{
		SG_UI_DataObject_Add(pGrids->asGrid(i), SG_UI_DATAOBJECT_UPDATE_ONLY);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

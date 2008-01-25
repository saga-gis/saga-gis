
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//            geostatistics_kriging_variogram            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   kriging_base.cpp                    //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
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
#include "variogram_dialog.h"

#include "kriging_base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKriging_Base::CKriging_Base(void)
{
	CSG_Parameter	*pNode;
	CSG_Parameters	*pParameters;

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field(
		pNode	, "ZFIELD"		, _TL("Attribute"),
		_TL("")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "GRID"		, _TL("Grid"),
		_TL("")
	);

	Parameters.Add_Grid_Output(
		NULL	, "VARIANCE"	, _TL("Variance"),
		_TL("")
	);

	Parameters.Add_Value(
		NULL	, "BVARIANCE"	, _TL("Create Variance Grid"),
		_TL(""),
		PARAMETER_TYPE_Bool		, true
	);

	Parameters.Add_Choice(
		NULL	, "TARGET"		, _TL("Target Grid"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("user defined"),
			_TL("grid system"),
			_TL("grid")
		), 0
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "VARIOGRAM"	, _TL("Semi-Variogram"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "DISTLAG"		, _TL("Distance Increment"),
		_TL(""),
		PARAMETER_TYPE_Double	, 10.0, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "DISTMAX"		, _TL("Maximum Distance"),
		_TL(""),
		PARAMETER_TYPE_Double	, -1.0
	);

	Parameters.Add_Value(
		pNode	, "NSKIP"		, _TL("Skip Number"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);

	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "BLOCK"		, _TL("Block Kriging"),
		_TL(""),
		PARAMETER_TYPE_Bool		, false
	);

	Parameters.Add_Value(
		NULL	, "DBLOCK"		, _TL("Block Size"),
		_TL(""),
		PARAMETER_TYPE_Double	, 100.0, 0.0, true
	);

	//-----------------------------------------------------
	pParameters	= Add_Parameters(SG_T("USER")	, _TL("User defined grid")	, _TL(""));

	pParameters->Add_Value(
		NULL	, "CELL_SIZE"	, _TL("Grid Size"),
		_TL(""),
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);

	pNode	= pParameters->Add_Value(
		NULL	, "FIT_EXTENT"	, _TL("Fit Extent"),
		_TL("Automatically fits the grid to the shapes layers extent."),
		PARAMETER_TYPE_Bool		, true
	);

	pParameters->Add_Range(
		pNode	, "X_EXTENT"	, _TL("X-Extent"),
		_TL("")
	);

	pParameters->Add_Range(
		pNode	, "Y_EXTENT"	, _TL("Y-Extent"),
		_TL("")
	);

	//-----------------------------------------------------
	pParameters	= Add_Parameters(SG_T("SYSTEM")	, _TL("Choose Grid System")	, _TL(""));

	pParameters->Add_Grid_System(
		NULL	, "SYSTEM"		, _TL("Grid System"),
		_TL("")
	);

	//-----------------------------------------------------
	pParameters	= Add_Parameters(SG_T("GRID")	, _TL("Choose Grid")		, _TL(""));

	pNode	= pParameters->Add_Grid_System(
		NULL	, "SYSTEM"		, _TL("Grid System"),
		_TL("")
	);

	pParameters->Add_Grid(
		pNode	, "GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT	, false
	);

	pParameters->Add_Grid(
		pNode	, "VARIANCE"	, _TL("Variance"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, false
	);
}

//---------------------------------------------------------
CKriging_Base::~CKriging_Base(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Base::On_Execute(void)
{
	bool	bResult	= false;

	if( _Initialise() && _Get_Variances() )
	{
		if( m_Variogram.Set_Formula(SG_T("a + b * x")) && m_Variogram.Get_Trend() && On_Initialise() )
		{
			CVariogram_Dialog	dlg(&m_Variogram);

			if( dlg.ShowModal() == wxID_OK )
			{
				bResult	= _Interpolate();
			}
		}
	}

	_Finalise();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Base::_Initialise(void)
{
	//-----------------------------------------------------
	m_Block		= Parameters("DBLOCK")	->asDouble() / 2.0;
	m_bBlock	= Parameters("BLOCK")	->asBool() && m_Block > 0.0;

	//-----------------------------------------------------
	m_pPoints	= Parameters("POINTS")	->asShapes();
	m_zField	= Parameters("ZFIELD")	->asInt();

	if( m_pPoints->Get_Count() <= 1 )
	{
		SG_UI_Msg_Add(_TL("not enough points for interpolation"), true);

		return( false );
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CKriging_Base::_Initialise_Grids(void)
{
	//-----------------------------------------------------
	m_pGrid		= NULL;
	m_pVariance	= NULL;

	//-----------------------------------------------------
	switch( Parameters("TARGET")->asInt() )
	{
	case 0:	// user defined...
		if( Dlg_Parameters("USER") )
		{
			TSG_Rect		Extent	= m_pPoints->Get_Extent();
			CSG_Parameters	*P		= Get_Parameters("USER");

			if( !P->Get_Parameter("FIT_EXTENT")->asBool() )
			{
				Extent.xMin	= P->Get_Parameter("X_EXTENT")->asRange()->Get_LoVal();
				Extent.yMin	= P->Get_Parameter("Y_EXTENT")->asRange()->Get_LoVal();
				Extent.xMax	= P->Get_Parameter("X_EXTENT")->asRange()->Get_HiVal();
				Extent.yMax	= P->Get_Parameter("Y_EXTENT")->asRange()->Get_HiVal();
			}

			double	d	= P->Get_Parameter("CELL_SIZE")->asDouble();
			int		nx	= 1 + (int)((Extent.xMax - Extent.xMin) / d);
			int		ny	= 1 + (int)((Extent.yMax - Extent.yMin) / d);

			if( nx > 1 && ny > 1 )
			{
				m_pGrid	= SG_Create_Grid(GRID_TYPE_Float, nx, ny, d, Extent.xMin, Extent.yMin);
			}
		}
		break;

	case 1:	// grid system...
		if( Dlg_Parameters("SYSTEM") )
		{
			m_pGrid		= SG_Create_Grid(*Get_Parameters("SYSTEM")->Get_Parameter("SYSTEM")->asGrid_System(), GRID_TYPE_Float);
		}
		break;

	case 2:	// grid...
		if( Dlg_Parameters("GRID") )
		{
			m_pGrid		= Get_Parameters("GRID")->Get_Parameter("GRID")		->asGrid();
			m_pVariance	= Get_Parameters("GRID")->Get_Parameter("VARIANCE")	->asGrid();
		}
		break;
	}

	//-----------------------------------------------------
	if( m_pGrid )
	{
		if( !m_pVariance && Parameters("BVARIANCE")->asBool() )
		{
			m_pVariance	= SG_Create_Grid(m_pGrid, GRID_TYPE_Float);
		}

		m_pGrid->Set_Name(CSG_String::Format(SG_T("%s (%s)"), m_pPoints->Get_Name(), Get_Name()));
		Parameters("GRID")->Set_Value(m_pGrid);

		if( m_pVariance )
		{
			m_pVariance->Set_Name(CSG_String::Format(SG_T("%s (%s - %s)"), m_pPoints->Get_Name(), Get_Name(), _TL("Variance")));
			Parameters("VARIANCE")->Set_Value(m_pVariance);
		}

		if( Parameters("TARGET")->asInt() == 2 )
		{
			Get_Parameters("GRID")->Get_Parameter("VARIANCE")->Set_Value(m_pVariance);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CKriging_Base::_Finalise(void)
{
	m_Points	.Clear();
	m_Search	.Destroy();
	m_G			.Destroy();
	m_W			.Destroy();
	m_Variogram	.Clr_Data();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Base::_Interpolate(void)
{
	if( _Initialise_Grids() )
	{
		int		ix, iy;
		double	x, y, z, v;

		for(iy=0, y=m_pGrid->Get_YMin(); iy<m_pGrid->Get_NY() && Set_Progress(iy, m_pGrid->Get_NY()); iy++, y+=m_pGrid->Get_Cellsize())
		{
			for(ix=0, x=m_pGrid->Get_XMin(); ix<m_pGrid->Get_NX(); ix++, x+=m_pGrid->Get_Cellsize())
			{
				if( Get_Value(x, y, z, v) )
				{
					m_pGrid->Set_Value(ix, iy, z);

					if( m_pVariance )
					{
						m_pVariance->Set_Value(ix, iy, v);
					}
				}
				else
				{
					m_pGrid->Set_NoData(ix, iy);

					if( m_pVariance )
					{
						m_pVariance->Set_NoData(ix, iy);
					}
				}
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CKriging_Base::Get_Weight(double d)
{
	if( d > 0.0 )
	{
		d	= m_Variogram.Get_Value(d);

		return( d > 0.0 ? d : 0.000001 );
	}

	return( 0.000001 );
}

//---------------------------------------------------------
double CKriging_Base::Get_Weight(double dx, double dy)
{
	return( Get_Weight(sqrt(dx*dx + dy*dy)) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	DIF_FIELD_DISTANCE		= 0,
	DIF_FIELD_DIFFERENCE
};

//---------------------------------------------------------
enum
{
	VAR_FIELD_DISTANCE		= 0,
	VAR_FIELD_VARIANCE
};

//---------------------------------------------------------
bool CKriging_Base::_Get_Variances(void)
{
	int					iDif, nVar, nVarS, nSkip;
	double				iDist, dz, zVar, zVarS, Dist, Dist_Step, Dist_Max;
	CSG_Table_Record	*pRec_Dif;
	CSG_Table			Differences;

	//-----------------------------------------------------
	nSkip		= Parameters("NSKIP")	->asInt();
	Dist_Max	= Parameters("DISTMAX")	->asDouble();
	Dist_Step	= Parameters("DISTLAG")	->asDouble();

	//-----------------------------------------------------
	if( _Get_Differences(&Differences, m_zField, nSkip, Dist_Max) )
	{
		Differences.Set_Index(DIF_FIELD_DISTANCE, TABLE_INDEX_Up);

		m_Variogram	.Clr_Data();

		//-----------------------------------------------------
		iDist		= 0.0;
		zVar		= 0.0;
		nVar		= 0;
		zVarS		= 0.0;
		nVarS		= 0;

		//-----------------------------------------------------
		for(iDif=0; iDif<Differences.Get_Record_Count() && Set_Progress(iDif, Differences.Get_Record_Count()); iDif++)
		{
			pRec_Dif	= Differences.Get_Record_byIndex(iDif);

			if( (Dist = pRec_Dif->asDouble(DIF_FIELD_DISTANCE)) > iDist )
			{
				if( nVar > 0 )
				{
					zVarS	+= zVar;
					nVarS	+= nVar;

					m_Variogram.Add_Data(iDist, 0.5 * zVarS / (double)nVarS);
				}

				zVar		= 0.0;
				nVar		= 0;

				do	iDist	+= Dist_Step;	while( Dist > iDist );
			}

			dz		= pRec_Dif->asDouble(DIF_FIELD_DIFFERENCE);
			zVar	+= dz*dz;
			nVar++;
		}

		//-----------------------------------------------------
		Differences.Set_Index(DIF_FIELD_DISTANCE, TABLE_INDEX_None);

		return( Process_Get_Okay(false) );
	}

	return( false );
}

//---------------------------------------------------------
bool CKriging_Base::_Get_Differences(CSG_Table *pTable, int zField, int nSkip, double maxDist)
{
	int					iPoint, jPoint;
	double				d, dx, dy, z;
	CSG_Shape			*pPoint;
	CSG_Table_Record	*pRecord;
	TSG_Point			Pt_i, Pt_j;

	//-----------------------------------------------------
	pTable->Destroy();
	pTable->Add_Field(_TL("Distance")	, TABLE_FIELDTYPE_Double);	// DIF_FIELD_DISTANCE
	pTable->Add_Field(_TL("Difference")	, TABLE_FIELDTYPE_Double);	// DIF_FIELD_DIFFERENCE

	//-----------------------------------------------------
	for(iPoint=0; iPoint<m_pPoints->Get_Count()-nSkip && Set_Progress(iPoint, m_pPoints->Get_Count()-nSkip); iPoint+=nSkip)
	{
		pPoint	= m_pPoints->Get_Shape(iPoint);
		Pt_i	= pPoint->Get_Point(0);
		z		= pPoint->Get_Record()->asDouble(zField);

		for(jPoint=iPoint+nSkip; jPoint<m_pPoints->Get_Count(); jPoint+=nSkip)
		{
			pPoint	= m_pPoints->Get_Shape(jPoint);
			Pt_j	= pPoint->Get_Point(0);
			dx		= Pt_j.x - Pt_i.x;
			dy		= Pt_j.y - Pt_i.y;

			if( (d = sqrt(dx*dx + dy*dy)) < maxDist || maxDist < 0.0 )
			{
				pRecord	= pTable->Add_Record();
				pRecord->Set_Value(DIF_FIELD_DISTANCE	, d);
				pRecord->Set_Value(DIF_FIELD_DIFFERENCE	, pPoint->Get_Record()->asDouble(zField) - z);
			}
		}
	}

	return( Process_Get_Okay(false) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

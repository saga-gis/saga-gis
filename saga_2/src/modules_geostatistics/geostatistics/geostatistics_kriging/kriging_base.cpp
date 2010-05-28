
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
	Parameters.Add_Choice(
		NULL	, "TARGET"		, _TL("Target Grid"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("user defined"),
			_TL("grid")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "TQUALITY"	, _TL("Type of Quality Measure"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("standard deviation"),
			_TL("variance")
		), 0
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "VARIOGRAM"	, _TL("Variogram"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "DISTMAX"		, _TL("Maximum Distance"),
		_TL(""),
		PARAMETER_TYPE_Double	, -1.0
	);

	Parameters.Add_Value(
		pNode	, "DISTCOUNT"	, _TL("Initial Number of Distance Classes"),
		_TL(""),
		PARAMETER_TYPE_Int		, 100, 1, true
	);

	Parameters.Add_Value(
		pNode	, "NSKIP"		, _TL("Skip Number"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Value(
		NULL	, "BLOCK"		, _TL("Block Kriging"),
		_TL(""),
		PARAMETER_TYPE_Bool		, false
	);

	Parameters.Add_Value(
		pNode	, "DBLOCK"		, _TL("Block Size"),
		_TL(""),
		PARAMETER_TYPE_Double	, 100.0, 0.0, true
	);

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	pParameters	= Add_Parameters(SG_T("FORMULA"), _TL("Formula"), _TL(""));

	pParameters->Add_String(
		NULL	, "STRING"		, _TL("Formula String"),
		_TL(""),
		SG_T("a + b * x")
	);

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	pParameters = Add_Parameters("USER", _TL("User Defined Grid")	, _TL(""));

	pParameters->Add_Value(
		NULL	, "BVARIANCE"	, _TL("Create Quality Grid"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Grid_Target.Add_Parameters_User(pParameters);

	//-----------------------------------------------------
	pParameters = Add_Parameters("GRID", _TL("Choose Grid")			, _TL(""));

	m_Grid_Target.Add_Parameters_Grid(pParameters);

	//-----------------------------------------------------
	m_Grid_Target.Add_Grid_Parameter(SG_T("VARIANCE"), _TL("Quality Measure"), true);

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	m_Variances.Add_Field(SG_T("DISTANCE")	, SG_DATATYPE_Double);
	m_Variances.Add_Field(SG_T("VAR_CUM")	, SG_DATATYPE_Double);
	m_Variances.Add_Field(SG_T("VAR_CLS")	, SG_DATATYPE_Double);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CKriging_Base::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( m_Grid_Target.On_User_Changed(pParameters, pParameter) ? 1 : 0 );
}


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
		m_Variogram.Set_Formula	(Get_Parameters("FORMULA")->Get_Parameter("STRING")->asString());

		if( SG_UI_Get_Window_Main() )
		{
			CVariogram_Dialog	dlg(&m_Variogram, &m_Variances);

			bResult	= dlg.ShowModal() == wxID_OK;
		}
		else
		{
			m_Variogram.Clr_Data();

			for(int i=0; i<m_Variances.Get_Count(); i++)
			{
				m_Variogram.Add_Data(m_Variances[i][0], m_Variances[i][1]);
			}

			bResult	= m_Variogram.Get_Trend();
		}

		if( bResult && m_Variogram.is_Okay() && On_Initialise() )
		{
			bResult	= _Interpolate();

			Get_Parameters("FORMULA")->Get_Parameter("STRING")->Set_Value(m_Variogram.Get_Formula(SG_TREND_STRING_Formula));
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
	m_bStdDev	= Parameters("TQUALITY")->asInt() == 0;

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

	switch( Parameters("TARGET")->asInt() )
	{
	case 0:	// user defined...
		if( m_Grid_Target.Init_User(m_pPoints->Get_Extent()) && Dlg_Parameters("USER") )
		{
			m_pGrid		= m_Grid_Target.Get_User();

			if( Get_Parameters("USER")->Get_Parameter("BVARIANCE")->asBool() )
			{
				m_pVariance	= m_Grid_Target.Get_User(SG_T("VARIANCE"));
			}
		}
		break;

	case 1:	// grid...
		if( Dlg_Parameters("GRID") )
		{
			m_pGrid		= m_Grid_Target.Get_Grid();
			m_pVariance	= m_Grid_Target.Get_Grid(SG_T("VARIANCE"));
		}
		break;
	}

	if( !m_pGrid )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_pGrid->Set_Name(CSG_String::Format(SG_T("%s (%s)"), m_pPoints->Get_Name(), Get_Name()));

	Parameters("GRID")->Set_Value(m_pGrid);

	if( m_pVariance )
	{
		m_pVariance->Set_Name(CSG_String::Format(SG_T("%s (%s - %s)"), m_pPoints->Get_Name(), Get_Name(), m_bStdDev ? _TL("Standard Deviation") : _TL("Variance")));

		Parameters("VARIANCE")->Set_Value(m_pVariance);
	}

	return( true );
}

//---------------------------------------------------------
bool CKriging_Base::_Finalise(void)
{
	m_Points	.Clear();
	m_Search	.Destroy();
	m_G			.Destroy();
	m_W			.Destroy();
	m_Variogram	.Clr_Data();
	m_Variances	.Del_Records();

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
						m_pVariance->Set_Value(ix, iy, m_bStdDev ? sqrt(v) : v);
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
	int			i, j, k, n, nDistances, nSkip;
	double		z, d, dx, dy, maxDistance, lagDistance;
	TSG_Point	Pt_i, Pt_j;
	CSG_Vector	Count, Variance;
	CSG_Shape	*pPoint;

	//-----------------------------------------------------
	nSkip		= Parameters("NSKIP")		->asInt();
	maxDistance	= Parameters("DISTMAX")		->asDouble();
	nDistances	= Parameters("DISTCOUNT")	->asInt();

	if( maxDistance <= 0.0 )
	{
		maxDistance	= SG_Get_Length(m_pPoints->Get_Extent().Get_XRange(), m_pPoints->Get_Extent().Get_XRange());
	}

	lagDistance	= maxDistance / nDistances;

	Count		.Create(nDistances);
	Variance	.Create(nDistances);

	//-----------------------------------------------------
	for(i=0, n=0; i<m_pPoints->Get_Count()-nSkip && Set_Progress(n, SG_Get_Square(m_pPoints->Get_Count()/nSkip)/2); i+=nSkip)
	{
		pPoint	= m_pPoints->Get_Shape(i);

		if( !pPoint->is_NoData(m_zField) )
		{
			Pt_i	= pPoint->Get_Point(0);
			z		= pPoint->asDouble(m_zField);

			for(j=i+nSkip; j<m_pPoints->Get_Count(); j+=nSkip, n++)
			{
				pPoint	= m_pPoints->Get_Shape(j);

				if( !pPoint->is_NoData(m_zField) )
				{
					Pt_j	= pPoint->Get_Point(0);
					dx		= Pt_j.x - Pt_i.x;
					dy		= Pt_j.y - Pt_i.y;
					d		= sqrt(dx*dx + dy*dy);
					k		= (int)(d / lagDistance);

					if( k < nDistances )
					{
						d	= pPoint->asDouble(m_zField) - z;

						Count	[k]	++;
						Variance[k]	+= d * d;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	for(i=0, z=0.0, n=0; i<nDistances && Process_Get_Okay(); i++)
	{
		if( Count[i] > 0 )
		{
			n	+= Count	[i];
			z	+= Variance	[i];

			CSG_Table_Record	*pRecord	= m_Variances.Add_Record();
			pRecord->Set_Value(0, (i + 1) * lagDistance);
			pRecord->Set_Value(1, 0.5 * Variance[i] / Count[i]);
			pRecord->Set_Value(2, 0.5 * z / n);
		}
	}

	return( Process_Get_Okay() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

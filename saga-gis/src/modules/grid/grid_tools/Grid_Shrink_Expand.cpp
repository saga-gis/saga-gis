/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Grid_Shrink_Expand.cpp                 //
//                                                       //
//                 Copyright (C) 2011 by                 //
//            Volker Wichmann and Olaf Conrad            //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_Shrink_Expand.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Shrink_Expand::CGrid_Shrink_Expand(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Shrink and Expand"));

	Set_Author		(SG_T("V.Wichmann & O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Regions with valid data in the input grid can be shrinked or expanded by a certain amount (radius). "
		"Shrinking just sets the border of regions with valid data to NoData, expanding sets NoData "
		"cells along the border of regions with valid data to a new valid value, computed by the method "
		"selected (min, max, mean, majority).\n\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL, "INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "RESULT"		, _TL("Result Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL, "OPERATION"		, _TL("Operation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Shrink"),
			_TL("Expand")
		), 1
	);

	Parameters.Add_Choice(
		NULL, "MODE"		, _TL("Search Mode"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Square"),
			_TL("Circle")
		), 1
	);

	Parameters.Add_Value(
		NULL, "RADIUS"		, _TL("Radius"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);

	CSG_String	choices;
		
	for (int i=0; i<METHOD_KEY_Count; i++)
		choices += CSG_String::Format(SG_T("%s|"), gSG_Method_Key_Names[i]);

	Parameters.Add_Choice(
		NULL, "METHOD_EXPAND"	, _TL("Method"),
		_TL(""),
		choices
		, 0
	);

}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Shrink_Expand::On_Execute(void)
{
	int						x, y, ix, iy;
	int						operation, methodExpand;
	CSG_Grid				*pResult;
	CSG_Simple_Statistics	Statistics;

	//-----------------------------------------------------
	m_pInput		= Parameters("INPUT")		->asGrid();
	pResult			= Parameters("RESULT")		->asGrid();
	m_Radius		= Parameters("RADIUS")		->asInt();
	operation		= Parameters("OPERATION")	->asInt();
	methodExpand	= Parameters("METHOD_EXPAND")->asInt();

	//-----------------------------------------------------
	m_Kernel.Create(SG_DATATYPE_Byte, 1 + 2 * m_Radius, 1 + 2 * m_Radius);
	m_Kernel.Set_NoData_Value(0.0);
	m_Kernel.Assign(1.0);
	m_Kernel.Set_Value(m_Radius, m_Radius, 0.0);

	if( Parameters("MODE")->asInt() == 1 )
	{
		for(y=-m_Radius, iy=0; y<=m_Radius; y++, iy++)
		{
			for(x=-m_Radius, ix=0; x<=m_Radius; x++, ix++)
			{
				if( x*x + y*y > m_Radius*m_Radius )
				{
					m_Kernel.Set_Value(ix, iy, 0.0);
				}
			}
		}
	}

	//-----------------------------------------------------
	if( !pResult || pResult == m_pInput )
	{
		pResult	= SG_Create_Grid(m_pInput);
	}
	else
	{
		if( operation == 0 )
			pResult->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_pInput->Get_Name(), _TL("Shrink")));
		else
			pResult->Set_Name(CSG_String::Format(SG_T("%s [%s %s]"), m_pInput->Get_Name(), _TL("Expand"), gSG_Method_Key_Names[Parameters("METHOD_EXPAND")->asInt()]));

		pResult->Set_NoData_Value(m_pInput->Get_NoData_Value());
	}

	pResult->Assign_NoData();

	//-----------------------------------------------------
	if( operation == 0 )	// shrink
	{
		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( m_pInput->is_NoData(x, y) )
					continue;
				else
				{
					bool	bShrink = false;

					for(int iy=0, jy=y-m_Radius; iy<m_Kernel.Get_NY(); iy++, jy++)
					{
						for(int ix=0, jx=x-m_Radius; ix<m_Kernel.Get_NX(); ix++, jx++)
						{
							if( m_Kernel.asByte(ix, iy) && m_pInput->is_InGrid(jx, jy, false) )
							{
								if( m_pInput->is_NoData(jx, jy) )
								{
									bShrink = true;
									break;
								}
							}
						}

						if( bShrink )
							break;
					}

					if( bShrink )
						pResult->Set_NoData(x, y);
					else
						pResult->Set_Value(x, y, m_pInput->asDouble(x, y));
				}
			}
		}
	}
	else	// expand
	{
		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( !m_pInput->is_NoData(x, y) )
				{
					pResult->Set_Value(x, y, m_pInput->asDouble(x, y));
					continue;
				}
				else
				{
					if(	methodExpand == METHOD_KEY_MIN || methodExpand == METHOD_KEY_MAX || methodExpand == METHOD_KEY_MEAN )
					{
						Statistics.Invalidate();

						for(int iy=0, jy=y-m_Radius; iy<m_Kernel.Get_NY(); iy++, jy++)
						{
							for(int ix=0, jx=x-m_Radius; ix<m_Kernel.Get_NX(); ix++, jx++)
							{
								if( m_Kernel.asByte(ix, iy) && m_pInput->is_InGrid(jx, jy, false) )
								{
									if( !m_pInput->is_NoData(jx, jy) )
										Statistics.Add_Value(m_pInput->asDouble(jx, jy));
								}
							}
						}

						if( Statistics.Get_Count() == 0 )
						{
							pResult->Set_NoData(x, y);
							continue;
						}
					}

			
					switch( methodExpand )
					{
					default:
						return( false );
					case METHOD_KEY_MIN:
						pResult->Set_Value(x, y, Statistics.Get_Minimum());
						break;
					case METHOD_KEY_MAX:
						pResult->Set_Value(x, y, Statistics.Get_Maximum());
						break;
					case METHOD_KEY_MEAN:
						pResult->Set_Value(x, y, Statistics.Get_Mean());
						break;
					case METHOD_KEY_MAJORITY:
						m_Majority.Create();
						pResult->Set_Value(x, y, Get_Majority(x, y));
						m_Majority.Destroy();
						break;
					}
				} // noData
			} // for x
		} // for y
	} // method expand


	//-----------------------------------------------------
	if( !Parameters("RESULT")->asGrid() || Parameters("RESULT")->asGrid() == m_pInput )
	{
		m_pInput->Assign(pResult);

		delete(pResult);

		DataObject_Update(m_pInput);
	}

	m_Kernel.Destroy();

	return( true );
}


//---------------------------------------------------------
int CGrid_Shrink_Expand::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("OPERATION")) )
	{
		pParameters->Get_Parameter("METHOD_EXPAND")->Set_Enabled(pParameter->asInt() > 0);
	}

	//-----------------------------------------------------
	return (1);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CGrid_Shrink_Expand::Get_Majority(int x, int y)
{
	m_Majority.Reset();

	for(int iy=0, jy=y-m_Radius; iy<m_Kernel.Get_NY(); iy++, jy++)
	{
		for(int ix=0, jx=x-m_Radius; ix<m_Kernel.Get_NX(); ix++, jx++)
		{
			if( m_Kernel.asByte(ix, iy) && m_pInput->is_InGrid(jx, jy) )
			{
				m_Majority.Add_Value(m_pInput->asDouble(jx, jy));
			}
		}
	}

	int		Count;
	double	Value;

	if( m_Majority.Get_Majority(Value, Count) && Count > 0 )
		return( Value );
	else
		return( m_pInput->Get_NoData_Value() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

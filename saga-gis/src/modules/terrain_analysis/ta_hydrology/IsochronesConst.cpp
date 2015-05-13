/**********************************************************
 * Version $Id: IsochronesConst.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/*******************************************************************************
    IsochronesConst.cpp
    Copyright (C) Victor Olaya
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/ 

#include "Helper.h"
#include "IsochronesConst.h"

//-----------------------------------------------------
CIsochronesConst::CIsochronesConst(void){

	Set_Name(_TL("Isochrones Constant Speed"));
	Set_Author(_TL("V.Olaya (c) 2004, V.Wichmann (c) 2015"));
	Set_Description	(_TW("Isochrones calculation with constant speed based on a user "
						"provided Time of Concentration. For each selected pour point, "
						"the longest watercourse length and the average slope of the "
						"watercourse are reported. These can be used to estimate the "
						"Time of Concentration with one of the empirical equations "
						"available.\n\n"));

	Parameters.Add_Grid(NULL, 
						"DEM", 
						_TL("Elevation Grid"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"TIME", 
						_TL("Time Out [min]"), 
						_TL(""), 
						PARAMETER_OUTPUT, 
						true, 
						SG_DATATYPE_Double);

	Parameters.Add_Value(
		NULL, "TIME_OF_CONCENTRATION", _TL("Time of Concentration [min]"),
		_TL("Time of Concentration [min] used to estimate flow speed."),
		PARAMETER_TYPE_Double, 60.0, 0.001, true
	);
		

}//constructor


//-----------------------------------------------------
CIsochronesConst::~CIsochronesConst(void){
	On_Execute_Finish();
}


//-----------------------------------------------------
void CIsochronesConst::_CalculateDistance(int x, int y)
{
	CSG_Grid_Stack	Stack;

	Stack.Push(x, y);

	//-----------------------------------------------------
	while( Stack.Get_Size() > 0 && Process_Get_Okay() )
	{
		Stack.Pop(x, y);

		//-------------------------------------------------
		for(int i=0; i<8; i++)
		{
			int	ix	= Get_xFrom(i, x);
			int	iy	= Get_yFrom(i, y);

			if( m_pDEM->is_InGrid(ix, iy) && i == m_Direction.asInt(ix, iy) )
			{
				m_pTime->Set_Value(ix, iy, m_pTime->asDouble(x, y) + Get_Length(i));
				Stack.Push(ix, iy);
			}
		}
	}

	return;
}


//-----------------------------------------------------
bool CIsochronesConst::On_Execute(void)
{	
	m_pDEM = Parameters("DEM")->asGrid(); 
	m_pTime = Parameters("TIME")->asGrid();
	m_pTime->Assign(0.0);

	m_dConcTime = Parameters("TIME_OF_CONCENTRATION")->asDouble();

	m_Direction.Create(*Get_System(), SG_DATATYPE_Char);
	m_Direction.Set_NoData_Value(-1);

	Init_FlowDirectionsD8(m_pDEM, &m_Direction);

	return( true );
}


//-----------------------------------------------------
bool CIsochronesConst::On_Execute_Finish(void)
{
	m_Direction.Destroy();

	return( true );
}


//-----------------------------------------------------
bool CIsochronesConst::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{	
	int iX, iY;		
	int iHighX, iHighY;

	if(	Mode != MODULE_INTERACTIVE_LDOWN || !Get_Grid_Pos(iX, iY) )
	{
		return( false );
	}

	m_pTime->Assign(0.0);
	
	_CalculateDistance(iX, iY);
    
	double	dMax = m_pTime->Get_ZMax();
	bool	bMaxFound = false;

    for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		if( bMaxFound )
			break;

		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double dValue = m_pTime->asDouble(x,y);
            
			if (dValue == dMax)
			{
				#pragma omp critical
				{
					iHighX = x;
					iHighY = y;
					bMaxFound = true;
				}
			}
        }// for
    }// for
	
	double dMaxDist = m_pTime->Get_ZMax();
	double dH1 = m_pDEM->asDouble(iX, iY);
	double dH2 = m_pDEM->asDouble(iHighX, iHighY);
	double dSpeed = dMaxDist / m_dConcTime;

	SG_UI_Msg_Add(SG_T("--------------------------------------------------------------------------------"), true);
	SG_UI_Msg_Add(CSG_String::Format(_TL("Longest watercourse length: %.2f m"), dMaxDist), true);
	SG_UI_Msg_Add(CSG_String::Format(_TL("Average slope of watercourse: %.2f m/m"), (dH2 - dH1) / dMaxDist), true);
	SG_UI_Msg_Add(CSG_String::Format(_TL("Average velocity in watercourse: %.2f m/min"), dSpeed), true);
	SG_UI_Msg_Add(SG_T("--------------------------------------------------------------------------------"), true);
	
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			m_pTime->Set_Value(x, y, m_pTime->asDouble(x, y) / dSpeed);
		}
	}

	m_pTime->Set_NoData_Value(0.0);

	DataObject_Update(m_pTime, SG_UI_DATAOBJECT_SHOW_LAST_MAP);

	return (true);

}//method

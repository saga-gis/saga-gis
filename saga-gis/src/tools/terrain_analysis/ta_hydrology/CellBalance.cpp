/**********************************************************
 * Version $Id: CellBalance.cpp 1616 2013-02-27 16:23:56Z oconrad $
 *********************************************************/
/*******************************************************************************
    CellBalance.cpp
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "CellBalance.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCellBalance::CCellBalance(void)
{
	Set_Name		(_TL("Cell Balance"));

	Set_Author		("V.Olaya (c) 2004, O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Cell Balance"
	));

	Add_Reference(
		"Olaya, V.", "2004", "Hidrologia computacional y modelos digitales del terreno", "Alqua. 536 pp."
	);

	Parameters.Add_Grid(NULL,
		"DEM"		, _TL("Elevation"),
		_TL(""), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_or_Const(NULL,
		"WEIGHTS"	, _TL("Weights"),
		_TL(""),
		1.0, 0.0, true
	);

	Parameters.Add_Grid(NULL,
		"BALANCE"	, _TL("Cell Balance"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(NULL,
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Deterministic 8"),
			_TL("Multiple Flow Direction")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCellBalance::On_Execute(void)
{
	m_pDEM		= Parameters("DEM"    )->asGrid();
	m_pBalance	= Parameters("BALANCE")->asGrid();

	int	Method	= Parameters("METHOD" )->asInt();

	CSG_Grid	*pWeight	= Parameters("WEIGHTS")->asGrid  ();
	double		  Weight	= Parameters("WEIGHTS")->asDouble();

	m_pBalance->Assign(0.0);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pDEM->is_NoData(x, y) )
			{
				m_pBalance->Set_NoData(x, y);
			}
			else
			{
				double	w	= pWeight && !pWeight->is_NoData(x, y) ? pWeight->asDouble(x, y) : Weight;

				if( w > 0.0 )
				{
					m_pBalance->Add_Value(x, y, -w);

					switch( Method )
					{
					case  0:	Set_D8 (x, y, w);	break;
					default:	Set_MFD(x, y, w);	break;
					}
				}
			}
        }
    }

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CCellBalance::Set_D8(int x, int y, double Weight)
{
	int	Dir	= m_pDEM->Get_Gradient_NeighborDir(x, y);

	if( Dir >= 0 )
	{
		x	+= Get_xTo(Dir);
		y	+= Get_yTo(Dir);

		if( m_pDEM->is_InGrid(x, y) )
		{
			m_pBalance->Add_Value(x, y, Weight);
		}
	}
}

//---------------------------------------------------------
void CCellBalance::Set_MFD(int x, int y, double Weight)
{
	const double	MFD_Converge	= 1.1;

	int		i;

	double	d, dz[8], dzSum = 0.0, z = m_pDEM->asDouble(x, y);

	for(i=0; i<8; i++)
	{
		int	ix	= Get_xTo(i, x);
		int	iy	= Get_yTo(i, y);

		if( m_pDEM->is_InGrid(ix, iy) && (d = z - m_pDEM->asDouble(ix, iy)) > 0.0 )
		{
			dz[i]	= pow(d / Get_Length(i), MFD_Converge);
			dzSum	+= dz[i];
		}
		else
		{
			dz[i]	= 0.0;
		}
	}

	if( dzSum > 0.0 )
	{
		d	= Weight / dzSum;

		for(i=0; i<8; i++)
		{
			if( dz[i] > 0.0 )
			{
				int	ix	= Get_xTo(i, x);
				int	iy	= Get_yTo(i, y);

				m_pBalance->Add_Value(ix, iy, dz[i] * d);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

/*/---------------------------------------------------------
#include "Helper.h"

double CCellBalance::getCellBalance(int iX, int iY)
{		
	int iNextX, iNextY;
	double dIn=0, dOut=0, dBalance;

	for (int i = -1; i<2; i++){
		for (int j = -1; j<2; j++){
			if (!(i == 0) || !(j == 0)) {
				getNextCell(m_pDEM, iX + i, iY + j, iNextX, iNextY);
                if (iNextY == iY && iNextX == iX) {
                    dIn+=m_pWeight->asDouble(iX+i,iY+j);                                        
				}// if				
			}//if				
		}//for
	}//for
	
	dOut = m_pWeight->asDouble(iX,iY);
	dBalance = dIn-dOut;
	
	return dBalance;
	
}//function

bool CCellBalance::On_Execute(void)
{
	m_pDEM = Parameters("DEM")->asGrid(); 
	m_pWeight = Parameters("WEIGHT")->asGrid(); 
	m_pBalance = Parameters("BALANCE")->asGrid();
	m_pBalance->Assign((double)0);

    for(int y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(int x=0; x<Get_NX(); x++){			
            m_pBalance->Set_Value(x,y,getCellBalance(x,y));
        }// for
    }// for

	return( true );

}//method /**/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

	Set_Author		(_TL("(c) 2004 by V.Olaya, (c) 2006 by O.Conrad"));

	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. Cell Balance Calculation\r\n"
		"References:\r\n 1. Olaya, V. Hidrologia computacional y modelos digitales del terreno. Alqua. 536 pp. 2004"
	));

	Parameters.Add_Grid(
		NULL, "DEM"		, _TL("Elevation"),
		_TL(""), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "WEIGHTS"	, _TL("Parameter"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL, "WEIGHT"	, _TL("Default Weight"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0
	);

	Parameters.Add_Grid(
		NULL, "BALANCE"	, _TL("Cell Balance"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL, "METHOD"	, _TL("Method"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Deterministic 8"),
			_TL("Multiple Flow Direction")
		)
	);
}

//---------------------------------------------------------
CCellBalance::~CCellBalance(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCellBalance::On_Execute(void)
{
	int		x, y, Method;
	double	Weight;
	CSG_Grid	*pWeights;

	m_pDEM		= Parameters("DEM")		->asGrid(); 
	pWeights	= Parameters("WEIGHTS")	->asGrid(); 
	m_pBalance	= Parameters("BALANCE")	->asGrid();
	Weight		= Parameters("WEIGHT")	->asDouble();
	Method		= Parameters("METHOD")	->asInt();

	m_pBalance->Assign(0.0);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pDEM->is_NoData(x, y) )
			{
				m_pBalance->Set_NoData	(x, y);
			}
			else
			{
				if( pWeights )
				{
					Weight	= pWeights->is_NoData(x, y) ? 0.0 : pWeights->asDouble(x, y);
				}

				m_pBalance->Add_Value	(x, y, -Weight);

				switch( Method )
				{
				case 0:	Set_D8	(x, y, Weight);	break;
				case 1:	Set_MFD	(x, y, Weight);	break;
				}
			}
        }
    }

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CCellBalance::Set_D8(int x, int y, double Weight)
{
	int		Dir;

	if( (Dir = m_pDEM->Get_Gradient_NeighborDir(x, y)) >= 0 )
	{
		x	+= Get_xTo(Dir);
		y	+= Get_yTo(Dir);

		if( is_InGrid(x, y) )
		{
			m_pBalance->Add_Value(x, y, Weight);
		}
	}
}

//---------------------------------------------------------
void CCellBalance::Set_MFD(int x, int y, double Weight)
{
	const double	MFD_Converge	= 1.1;

	int		i, ix, iy;
	double	z, d, dzSum, dz[8];

	z		= m_pDEM->asDouble(x, y);
	dzSum	= 0.0;

	for(i=0; i<8; i++)
	{
		ix		= Get_xTo(i, x);
		iy		= Get_yTo(i, y);

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
		d		= Weight / dzSum;

		for(i=0; i<8; i++)
		{
			if( dz[i] > 0.0 )
			{
				ix		= Get_xTo(i, x);
				iy		= Get_yTo(i, y);

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


///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   IsochronesVar.cpp                   //
//                                                       //
//                 Copyright (C) 2004 by                 //
//                     Victor Olaya                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
///////////////////////////////////////////////////////////

//-----------------------------------------------------
#include "IsochronesVar.h"
#include "Helper.h"

//-----------------------------------------------------
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

//---------------------------------------------------------
#define ADD_REFERENCES	{\
    Add_Reference("Al-Smadi, Mohammad", "1998",\
		"Incorporating spatial and temporal variation of watershed response in a gis-based hydrologic model",\
		"Faculty of the Virginia Polythecnic Institute and State University. MSc Thesis.",\
		SG_T("scholar.lib.vt.edu/theses/available/etd-121698-112858/unrestricted/smadi.pdf"),\
		SG_T("online")\
	);\
\
	Add_Reference("Martínez Álvarez, V.; Dal-Ré Tenreiro, R.; García García, A. I.; Ayuga Téllez, F.", "",\
		"Modelación distribuida de la escorrentía superficial en pequeñas cuencas mediante SIG",\
		"Evaluación experimental."\
	);\
\
	Add_Reference("Olaya, V.", "2004",\
		"Hidrologia computacional y modelos digitales del terreno",\
		"Alqua. 536 pp."\
	);\
}


//-----------------------------------------------------
CIsochronesVar_Tool_Interactive::CIsochronesVar_Tool_Interactive(void)
{

	Set_Name		(_TL("Isochrones Variable Speed"));

	Set_Author		("V.Olaya (c) 2004, V.Wichmann (c) 2015");

	Set_Description	(_TW(
		"Calculation of isochrones with variable speed.\n"
		"In case a cell in an optional input grid is NoData, the corresponding parameter value will "
		"be used instead of skipping this cell.\n"
        "This is the interactive tool version, where a left mouse click will trigger the calculation "
        "for the selected cell.\n\n"
	));

    ADD_REFERENCES

    Add_Tool_Parameters(Parameters, true);

}


//-----------------------------------------------------
bool CIsochronesVar_Tool_Interactive::On_Execute(void)
{
    m_Calculator.Initialise(    Parameters("DEM")->asGrid(),
	                            Parameters("TIME")->asGrid(),
	                            Parameters("SPEED")->asGrid(),
	                            Parameters("MANNING")->asGrid(),
	                            Parameters("SLOPE")->asGrid(),
	                            Parameters("FLOWACC")->asGrid(),
	                            Parameters("CN")->asGrid(),
	                            Parameters("THRSMIXED")->asDouble(),
	                            Parameters("THRSCHANNEL")->asDouble(),
	                            Parameters("AVGCN")->asDouble(),
	                            Parameters("AVGMANNING")->asDouble(),
	                            Parameters("AVGRAINFALL")->asDouble(),
	                            Parameters("CHANSLOPE")->asDouble(),
	                            Parameters("MINSPEED")->asDouble()
    );

	
	return( true );
}


//-----------------------------------------------------
bool CIsochronesVar_Tool_Interactive::On_Execute_Finish()
{
	m_Calculator.Finalise();

	return( true );
}


//-----------------------------------------------------
bool CIsochronesVar_Tool_Interactive::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	int iX, iY;

	if(	Mode != TOOL_INTERACTIVE_LDOWN || !Get_Grid_Pos(iX, iY) )
	{
		return( false );
	}
	

    m_Calculator.Calculate(iX, iY);
	

	DataObject_Update(Parameters("TIME")->asGrid(), SG_UI_DATAOBJECT_SHOW_MAP_ACTIVE);

	return( true );
}


//-----------------------------------------------------
CIsochronesVar_Tool::CIsochronesVar_Tool(void)
{

    Set_Name		(_TL("Isochrones Variable Speed"));

    Set_Author		("V.Olaya (c) 2004, V.Wichmann (c) 2015");

    Set_Description	(_TW(
        "Calculation of isochrones with variable speed.\n"
        "In case a cell in an optional input grid is NoData, the corresponding parameter value will "
        "be used instead of skipping this cell.\n"
        "This is the non-interactive tool version, where the target point can be specified either as "
        "point shapefile (the first point in the file will be used) or by target coordinates.\n\n"
    ));

    ADD_REFERENCES

    Add_Tool_Parameters(Parameters, false);
}


//-----------------------------------------------------
bool CIsochronesVar_Tool::On_Execute(void)
{
    m_Calculator.Initialise(    Parameters("DEM")->asGrid(),
                                Parameters("TIME")->asGrid(),
                                Parameters("SPEED")->asGrid(),
                                Parameters("MANNING")->asGrid(),
                                Parameters("SLOPE")->asGrid(),
                                Parameters("FLOWACC")->asGrid(),
                                Parameters("CN")->asGrid(),
                                Parameters("THRSMIXED")->asDouble(),
                                Parameters("THRSCHANNEL")->asDouble(),
                                Parameters("AVGCN")->asDouble(),
                                Parameters("AVGMANNING")->asDouble(),
                                Parameters("AVGRAINFALL")->asDouble(),
                                Parameters("CHANSLOPE")->asDouble(),
                                Parameters("MINSPEED")->asDouble()
    );

    int iX, iY;

    CSG_Shapes *pPoint = Parameters("TARGET_PT")->asShapes();

    if (pPoint != NULL)
    {
        if (pPoint->Get_Count() < 1)
        {
            SG_UI_Msg_Add_Error(_TL("Input shapefile is empty!"));

            m_Calculator.Finalise();

            return (false);
        }

        TSG_Point p = pPoint->Get_Shape(0)->Get_Point();

        iX = Parameters("DEM")->asGrid()->Get_System().Get_xWorld_to_Grid(p.x);
        iY = Parameters("DEM")->asGrid()->Get_System().Get_yWorld_to_Grid(p.y);
    }
    else
    {
        iX = Parameters("DEM")->asGrid()->Get_System().Get_xWorld_to_Grid(Parameters("TARGET_PT_X")->asDouble());
        iY = Parameters("DEM")->asGrid()->Get_System().Get_yWorld_to_Grid(Parameters("TARGET_PT_Y")->asDouble());
    }

    if (!Parameters("DEM")->asGrid()->Get_System().is_InGrid(iX, iY))
    {
        SG_UI_Msg_Add_Error(_TL("Target point is outside grid system!"));

        m_Calculator.Finalise();

        return (false);
    }
    
    
    //-----------------------------------------------------
    m_Calculator.Calculate(iX, iY);

    m_Calculator.Finalise();


    return( true );
}


//-----------------------------------------------------
void Add_Tool_Parameters(CSG_Parameters &Parameters, bool bInteractive)
{
    Parameters.Add_Grid("",
        "DEM", _TL("Elevation"),
        _TL(""),
        PARAMETER_INPUT
    );

    Parameters.Add_Grid("",
        "SLOPE", _TL("Slope"),
        _TL(""),
        PARAMETER_INPUT
    );

    Parameters.Add_Grid("",
        "FLOWACC", _TL("Catchment Area"),
        _TL(""),
        PARAMETER_INPUT
    );

    Parameters.Add_Grid("",
        "CN", _TL("Curve Number"),
        _TL(""),
        PARAMETER_INPUT_OPTIONAL
    );

    Parameters.Add_Grid("",
        "MANNING", _TL("Manning's N"),
        _TL(""),
        PARAMETER_INPUT_OPTIONAL
    );

    Parameters.Add_Grid("",
        "TIME", _TL("Time Out(h)"),
        _TL(""),
        PARAMETER_OUTPUT, true, SG_DATATYPE_Double
    );

    Parameters.Add_Grid("",
        "SPEED", _TL("Speed (m/s)"),
        _TL(""),
        PARAMETER_OUTPUT, true, SG_DATATYPE_Double
    );

    //-------------------------------------------------
    Parameters.Add_Double("",
        "AVGMANNING", _TL("Avg. Manning's N"),
        _TL(""),
        0.15
    );

    Parameters.Add_Double("",
        "AVGCN", _TL("Avg. Curve Number"),
        _TL(""),
        75
    );

    Parameters.Add_Double("",
        "THRSMIXED", _TL("Mixed Flow Threshold (ha)"),
        _TL(""),
        18
    );

    Parameters.Add_Double("",
        "THRSCHANNEL", _TL("Channel Definition Threshold (ha)"),
        _TL(""),
        360
    );

    Parameters.Add_Double("",
        "AVGRAINFALL", _TL("Avg. Rainfall Intensity (mm/h)"),
        _TL(""),
        1
    );

    Parameters.Add_Double("",
        "CHANSLOPE", _TL("Channel side slope(m/m)"),
        _TL(""),
        0.5
    );

    Parameters.Add_Double("",
        "MINSPEED", _TL("Min. Flow Speed (m/s)"),
        _TL(""),
        0.05
    );

    //-------------------------------------------------
    if (!bInteractive)
    {
        Parameters.Add_Double("",
            "TARGET_PT_X", _TL("Target X Coordinate"),
            _TL("The x-coordinate of the target point in world coordinates [map units]"),
            0.
        );

        Parameters.Add_Double("",
            "TARGET_PT_Y", _TL("Target Y Coordinate"),
            _TL("The y-coordinate of the target point in world coordinates [map units]"),
            0.
        );

        Parameters.Add_Shapes("",
            "TARGET_PT", _TL("Target Point"),
            _TL("A point shapefile with the target point."),
            PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Point
        );
    }
}


//---------------------------------------------------------
CIsochronesVar::CIsochronesVar(void)
{
    m_pDEM              = NULL;
    m_pTime             = NULL;
    m_pSpeed            = NULL;
    m_pManning          = NULL;
    m_pSlope            = NULL;
    m_pCatchArea        = NULL;
    m_pCN               = NULL;
}

//---------------------------------------------------------
CIsochronesVar::~CIsochronesVar(void)
{
    Finalise();
}


//-----------------------------------------------------
void CIsochronesVar::Initialise(CSG_Grid *pDEM, CSG_Grid *pTime, CSG_Grid *pSpeed, CSG_Grid *pManning, CSG_Grid *pSlope, CSG_Grid *pCatchArea, CSG_Grid *pCN,
                                double dMixedThresh, double dChannelThresh, double dCN, double dManning, double dRainfall, double dChannelSlope, double dMinSpeed)
{
    m_pDEM              = pDEM;
    m_pTime             = pTime;
    m_pSpeed            = pSpeed;
    m_pManning          = pManning;
    m_pSlope            = pSlope;
    m_pCatchArea        = pCatchArea;
    m_pCN               = pCN;
    m_dMixedThresh      = dMixedThresh * 10000;
    m_dChannelThresh    = dChannelThresh * 10000;
    m_dCN               = dCN;
    m_dManning          = dManning;
    m_dRainfall         = dRainfall;
    m_dChannelSlope     = dChannelSlope;
    m_dMinSpeed         = dMinSpeed;

    m_Direction.Create(m_pDEM->Get_System(), SG_DATATYPE_Char);
    m_Direction.Set_NoData_Value(-1);

    Init_FlowDirectionsD8(m_pDEM, &m_Direction);
}

//-----------------------------------------------------
void CIsochronesVar::Finalise(void)
{
    m_pDEM              = NULL;
    m_pTime             = NULL;
    m_pSpeed            = NULL;
    m_pManning          = NULL;
    m_pSlope            = NULL;
    m_pCatchArea        = NULL;
    m_pCN               = NULL;

    m_Direction.Destroy();
}

//-----------------------------------------------------
void CIsochronesVar::Calculate(int x, int y)
{
    m_pTime->Assign(0.0);

	_CalculateTime(x, y);

	for(int y=0; y<m_pDEM->Get_System().Get_NY() && SG_UI_Process_Set_Progress(y, m_pDEM->Get_System().Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<m_pDEM->Get_System().Get_NX(); x++)
		{
			m_pTime->Set_Value(x, y, m_pTime->asDouble(x,y) / 3600.0);
        }
    }

	ZeroToNoData();

}

//-----------------------------------------------------
void CIsochronesVar::_CalculateTime(int x, int y)
{
	CSG_Grid_Stack	Stack;
	double	dDist = 1;
    double	dD = 0;
    double	dSlope;
    double	dSpeed;
    double	dQ = 0;
    double	dH;
    double	dSup;
    double	dInf;
    double	dAcc;
    double	dArea = 0;
    double	dPerim;
    double	dDif;
    double	dManning;
	double	dCN;
    double	dI = 0;
	int		iIter = 0;
	int		iDir;


	Stack.Push(x, y);

	//-----------------------------------------------------
	while( Stack.Get_Size() > 0 && SG_UI_Process_Get_Okay() )
	{
		Stack.Pop(x, y);

		if (m_pCN != NULL && !m_pCN->is_NoData(x, y))
		{
			dCN = m_pCN->asDouble(x, y);
		}
		else
		{
			dCN = m_dCN;
		}

		dI = Runoff(m_dRainfall, dCN);
		dI /= 3600.0;	// in mm/s
        dI /= 1000.0;	// m/s of runoff;

		iDir  = m_Direction.asInt(x, y);
		dDist = m_pDEM->Get_System().Get_Length(iDir);	// length to previous, i.e. successor cell

		dSlope = m_pSlope->asDouble(x, y);
		dSlope = fabs(tan(dSlope));
		dSlope = max(0.001, dSlope);

		dAcc = m_pCatchArea->asDouble(x, y);

        if (dAcc < m_dMixedThresh)
		{
            dD = sqrt(2.0 * dAcc / 3.14159);
			
			if (m_pManning != NULL && !m_pManning->is_NoData(x, y))
			{
				dManning = m_pManning->asDouble(x, y);
			}
			else
			{
				dManning = m_dManning;
			}

            dSpeed = max(m_dMinSpeed, pow(dI * dD, 0.4) * pow(dSlope, 0.3) / pow(dManning, 0.6));
        }
        else
		{
			if (dAcc < m_dChannelThresh)
			{
				dManning = 0.06;
			}
			else
			{
				dManning= 0.05;
			}

            dQ		= dI * dAcc; // Q m3/s
            dSup	= 60;
            dInf	= 0;
            dH		= 2;
            dArea	= dH * dH / m_dChannelSlope;
            dPerim	= 2.0 * (dH / m_dChannelSlope + sqrt(dH * dH + pow(dH / m_dChannelSlope, 2.0)));
            dDif	= (sqrt(dSlope) * pow(dArea, 5.0 / 3.0) / pow(dPerim, 2.0 / 3.0) / dManning) - dQ;
            iIter	= 0;

			do
			{
                if (dDif > 0)
				{
                    dSup	= dH;
                    dH		= (dInf + dH) / 2.0;
                }
                else if (dDif < 0)
				{
                    dInf	= dH;
                    dH		= (dSup + dH) / 2.0;
                }

                dArea	= dH * dH / m_dChannelSlope;
                dPerim	= 2.0 * (dH / m_dChannelSlope + sqrt(dH * dH + pow(dH / m_dChannelSlope, 2.0)));
                dDif	= (sqrt(dSlope) * pow(dArea, 5.0 / 3.0) / pow(dPerim, 2.0 / 3.0) / dManning) - dQ;
				iIter++;
            }
			while (fabs(dDif) > 0.1);

            dSpeed = max(m_dMinSpeed, dQ / dArea);
        }

		int ix = m_pDEM->Get_System().Get_xTo(iDir, x);
		int iy = m_pDEM->Get_System().Get_yTo(iDir, y);

		m_pTime->Set_Value(x, y, m_pTime->asDouble(ix, iy) + dDist / dSpeed);
		m_pSpeed->Set_Value(x, y, dSpeed);

		//-------------------------------------------------
		for(int i=0; i<8; i++)
		{
			ix	= m_pDEM->Get_System().Get_xFrom(i, x);
			iy	= m_pDEM->Get_System().Get_yFrom(i, y);

			if( m_pDEM->is_InGrid(ix, iy) && i == m_Direction.asInt(ix, iy) )
			{
				Stack.Push(ix, iy);
			}
		}
	}

	return;
}


//-----------------------------------------------------
double CIsochronesVar::Runoff(double dRainfall, double dCN)
{
    double dS;
    double dRunoff;

    dS = (25400.0 / dCN) - 254;

    if (dRainfall < (0.2 * dS)) {
        return 0.1; //can´t be 0, because that would mean speed = 0
    }// if

    dRunoff = pow(dRainfall - 0.2 * dS, 2)
            / (dRainfall + 0.8 * dS);

    return dRunoff;

}


//-----------------------------------------------------
void CIsochronesVar::ZeroToNoData(void){


    for(int y=0; y<m_pDEM->Get_System().Get_NY() && SG_UI_Process_Set_Progress(y, m_pDEM->Get_System().Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<m_pDEM->Get_System().Get_NX(); x++)
		{
            if (m_pTime->asDouble(x, y) == 0)
			{
				m_pTime->Set_NoData(x, y);
			}

            if (m_pSpeed->asDouble(x, y) == 0)
			{
				m_pSpeed->Set_NoData(x, y);
			}
        }// for
    }// for

}


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
//                    IsochronesVar.h                    //
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


#include <saga_api/saga_api.h>

//-----------------------------------------------------
void        Add_Tool_Parameters(CSG_Parameters &Parameters, bool bInteractive);


//-----------------------------------------------------
class CIsochronesVar
{
public:

	CIsochronesVar(void);
	virtual ~CIsochronesVar(void);

    void        Initialise(CSG_Grid *pDEM, CSG_Grid *pTime, CSG_Grid *pSpeed, CSG_Grid *pManning, CSG_Grid *pSlope, CSG_Grid *pCatchArea, CSG_Grid *pCN,
                           double dMixedThresh, double dChannelThresh, double dCN, double dManning, double dRainfall, double dChannelSlope, double dMinSpeed);
    
    void        Finalise(void);

    void        Calculate(int x, int y);

private:

	CSG_Grid    *m_pDEM;
	CSG_Grid    *m_pTime;
	CSG_Grid    *m_pSpeed;
	CSG_Grid    *m_pManning;
	CSG_Grid    *m_pCN;
	CSG_Grid    *m_pCatchArea;
	CSG_Grid    *m_pSlope;
	CSG_Grid    m_Direction;
	double      m_dManning;
	double      m_dCN;
	double      m_dRainfall;
	double      m_dMixedThresh;
	double      m_dChannelThresh;
	double      m_dChannelSlope;
	double      m_dMinSpeed;
	void        ZeroToNoData(void);
	double      Runoff(double, double);

	void        _CalculateTime(int x, int y);

};


//-----------------------------------------------------
class CIsochronesVar_Tool_Interactive : public CSG_Tool_Grid_Interactive
{
public:
    
    CIsochronesVar_Tool_Interactive(void);
    
    virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("Dynamics" ));	}

protected:

    virtual bool On_Execute(void);
    virtual bool On_Execute_Finish();
    virtual bool On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode);

private:

    CIsochronesVar  m_Calculator;

};


//-----------------------------------------------------
class CIsochronesVar_Tool : public CSG_Tool_Grid
{
public:

    CIsochronesVar_Tool(void);

    virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("Dynamics" ));	}

protected:

    virtual bool On_Execute(void);

private:

    CIsochronesVar  m_Calculator;

};
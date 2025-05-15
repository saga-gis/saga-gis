
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      ta_lighting                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  clouds_and_shadows.h                 //
//                                                       //
//                  Copyrights (c) 2023                  //
//                  Justus Spitzmüller                   //
//                     Olaf Conrad                       //
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
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#pragma once


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "saga_api/grid.h"
#include <saga_api/saga_api.h>
#include <algorithm>
#include <vector>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

inline int CompareCoords(const TSG_Point_Int& a, const TSG_Point_Int& b) {
    if (a.y < b.y) return -1;
    if (a.y > b.y) return 1;
    if (a.x < b.x) return -1;
    if (a.x > b.x) return 1;
    return 0; // gleich
}

class CSG_Cloud_Stack : public CSG_Grid_Stack
{
public:
	bool					Overlap( CSG_Cloud_Stack &Other )
	{
		if( !m_Rect.Intersect(Other.m_Rect) )
		{
			return( false );
		}

		Sort();
		Other.Sort();

		TSG_Point_Int* Stack_This = (TSG_Point_Int*) Get_Array();
		TSG_Point_Int* Stack_Other = (TSG_Point_Int*) Other.Get_Array();

		int i = 0;
		int j = 0;

		while (i < Get_Size() && j < Other.Get_Size()) 
		{
        	int Compare = CompareCoords(Stack_This[i], Stack_Other[j]);
			if( Compare == 0)
			{
            	return( true ); 
        	} 
			else if( Compare < 0)
			{
            	++i;
        	} 
			else
			{
            	++j;
        	}
		}

		return( false );
	}

	CSG_Cloud_Stack& 			Merge( CSG_Cloud_Stack &Other )
	{
		if( !Overlap(Other) )
		{
			return( *this );
		}

		Sort();
		Other.Sort();
		
		TSG_Point_Int* Stack_This = (TSG_Point_Int*) Get_Array();
		TSG_Point_Int* Stack_Other = (TSG_Point_Int*) Other.Get_Array();

		CSG_Array 		Output(sizeof(TSG_Point_Int), Get_Size() + Other.Get_Size());

		int i = 0;
		int j = 0;
		
		while (i < Get_Size() && j < Other.Get_Size()) 
		{
        	int Compare = CompareCoords(Stack_This[i], Stack_Other[j]);
			if( Compare < 0)
			{
				Output.Inc_Array();
				*((TSG_Point_Int*)Output.Get_Entry(Output.Get_Size() - 1)) = Stack_This[i++];
			}
			else if (Compare > 0)
			{
				Output.Inc_Array();
				*((TSG_Point_Int*)Output.Get_Entry(Output.Get_Size() - 1)) = Stack_Other[j++];
			}
			else 
			{
				Output.Inc_Array();
				*((TSG_Point_Int*)Output.Get_Entry(Output.Get_Size() - 1)) = Stack_This[i];
				++i;
				++j;
			}
		}

		while (i < Get_Size())
		{
			Output.Inc_Array();
			*((TSG_Point_Int*)Output.Get_Entry(Output.Get_Size() - 1)) = Stack_This[i++];
		}

		// Rest von Other
		while (j < Other.Get_Size())
		{
			Output.Inc_Array();
			*((TSG_Point_Int*)Output.Get_Entry(Output.Get_Size() - 1)) = Stack_Other[j++];
		}

		Clear();

		TSG_Point_Int* Final = (TSG_Point_Int*) Output.Get_Array();
		for( int i=0; i<Output.Get_Size(); i++ )
		{
			TSG_Point_Int P = Final[i];
			Push( P.x, P.y );
		}

		m_isSorted = true;
		
		return( *this );
	}
	
	virtual bool			Push			(int  x, int  y)
	{
		if( Get_Size() == 0 )
		{
			m_Rect.Create(x,y,x,y);
		}

		m_Rect.Union(x,y);

		m_isSorted = false;

		return( CSG_Grid_Stack::Push(x,y) );
	}

	virtual bool			Pop				(int &x, int &y)
	{
		m_isSorted = false;

		return( CSG_Grid_Stack::Pop(x, y) );
	}


	void 					Sort(void)
	{
		if( m_isSorted )
			return;

		TSG_Point_Int* Array = (TSG_Point_Int*) Get_Array();
		std::sort( Array, Array + Get_Size(), 
			[](const TSG_Point_Int& A, const TSG_Point_Int& B)
			{
				return( CompareCoords(A, B) < 0 );
    		}
		);

		m_isSorted = true;

	}

	/*
	CSG_Cloud_Stack & operator=(const CSG_Cloud_Stack &Other)
	{
		if( this == &Other )
			return *this;
		
		Clear();

		TSG_Point_Int *pOther = (TSG_Point_Int*)Other.Get_Array();

		for( int i = 0; i < Other.Get_Size(); i++ )
		{
			Push(pOther[i].x, pOther[i].y);
		}

		m_Rect     = Other.m_Rect;
		m_isSorted = Other.m_isSorted;
		return *this;
	}
	*/


  
private: 

	CSG_Rect_Int 	m_Rect;
	
	bool 			m_isSorted = false;

};

typedef enum Sensor
{
	TM = 0, ETM = 1, OLI_TIRS = 2, MSI = 3
} Sensor;

typedef enum SpectralBand
{
	RED = 0, GREEN, BLUE, NIR, SWIR1, SWIR2, CIR, TIR, QARAD_G, QARAD_R, SAA, SZA, VAA, VZA
} SpectralBand;

typedef enum Algorithm
{
	FMASK_1_6 = 0, FMASK_3_2
} Algorithm;

//---------------------------------------------------------
class CFmask : public CSG_Tool
{
public:
	CFmask(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("A:Imagery|Feature Extraction") );	}


protected:

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	TSG_Data_Type 				m_Bool_Type = SG_DATATYPE_Bit;

	bool						m_bCelsius { false };
	int 						m_Algorithm;
	int 						m_Sensor;

	size_t						m_Bin_Count;
	double 						m_Temp_Off;
	double 						m_Temp_Min;
	double 						m_Temp_Max;

	double 						m_Snow_Temp_Threshold;
	double 						m_dx;
	double 						m_dy;
	double 						m_dz;

	double 						m_Similarity_Threshold = 0.3;
	double 						m_Decrease_Threshold;

	CSG_Grid					*m_pBand[14];
	CSG_Grid					*m_pResults[10];

	CSG_Grid_System 			*m_pSystem;
	
	CSG_Array_Int 				m_xKernel_Dilation_Cloud;
	CSG_Array_Int 				m_yKernel_Dilation_Cloud;
	CSG_Array_Int 				m_xKernel_Improve_Cloud;
	CSG_Array_Int 				m_yKernel_Improve_Cloud;
	CSG_Array_Int 				m_xKernel_Fill_Shadow;
	CSG_Array_Int 				m_yKernel_Fill_Shadow;
	
	int 						m_Min_Cloud_Size;
	int 						m_Improve_Cloud_Count;
	double 						m_Shadow_Radius;

	bool 						Get_Sun_Position		(CSG_Grid *pGrid, double &Azimuth, double &Height);
	bool 						Get_Sensor				(CSG_Grid *pGrid, Sensor &Sensor);

	double 						Get_Brightness			(int x, int y, int Band, bool &Eval );
	bool 						Is_Saturated			(int x, int y, SpectralBand Band);

	bool 						Get_Flood_Fill			(double Boundary, int Band_Input, int Band_Output );


	bool 						Initialize				(void);
	bool 						Set_Fmask_Pass_One		( double &T_Water, double &T_Low, double &T_High, double &Lower_Level_NIR, double &Lower_Level_Swir);
	bool 						Set_Fmask_Pass_Two		( const double T_Water, const double T_Low, const double T_High, double &Land_threshold, double &Water_threshold );
	
	bool 						Set_Cloud_Mask			( const double T_Low, const double Land_threshold, const double Water_threshold );
	bool 						Set_Final_Fmask			( void );

	bool 						Set_Shadow_Mask			( const double T_Low, const double T_High );

	bool 						Get_Segmentation		( std::vector<CSG_Cloud_Stack> *Array, double T_Low, double T_High, int xStart, int xEnd, int yStart, int yEnd);
	
	bool 						Get_3D_Shadow			( CSG_Cloud_Stack *pInputStack, CSG_Cloud_Stack *pOutputStack, const double T_Cloud_base );

	bool 						Get_Shadow_Match		( CSG_Grid_Stack *pCloud_Shape, const double H_Cloud_base_min, const double H_Cloud_base_max);
	
	bool 						Create_Kernel			( CSG_Array_Int *X_Kernel, CSG_Array_Int *Y_Kernel, int Cells, bool Round );
	

};



///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////
/*
//---------------------------------------------------------
class CDetect_CloudShadows : public CSG_Tool_Grid
{
public:
	CDetect_CloudShadows(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("A:Imagery|Feature Extraction") );	}


protected:

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	CSG_Table					*m_pInfo { NULL };

	CSG_Grid					m_Correction;


	CSG_Grid *					Get_Target				(void);

	bool						Get_Candidates			(CSG_Grid &Candidates);

	double						Get_Correction			(double dx, double dy, double dz, CSG_Grid *pDEM, int x, int y);
	bool						Get_Correction			(double dx, double dy, double dz);
	bool						Get_Correction			(double dx, double dy, int &x, int &y);

	bool						Get_Cloud				(CSG_Grid_Stack &Cloud, const CSG_Grid *pClouds);
	bool						Get_Cloud				(CSG_Grid_Stack &Cloud,       CSG_Grid *pClouds, int x, int y, CSG_Grid *pID);

	bool						Find_Shadow				(CSG_Grid *pShadows, CSG_Grid &Candidates, const CSG_Grid_Stack &Cloud, double zRange[2], double dx, double dy, double dz);

};
*/

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

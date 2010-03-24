
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    ta_preprocessor                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Flat_Detection.h                    //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Flat_Detection_H
#define HEADER_INCLUDED__Flat_Detection_H

///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Grid_Stack
{
public:
	CSG_Grid_Stack(void) : m_Size(0), m_Buffer(0), m_Stack(NULL)	{}
	virtual ~CSG_Grid_Stack(void)					{	Destroy();			}

	int						Get_Size		(void)	{	return( m_Size );	}
	void					Clear			(void)	{	m_Size	= 0;		}

	//-----------------------------------------------------
	void					Destroy			(void)
	{
		if( m_Stack )
		{
			SG_Free(m_Stack);
		}

		m_Size		= 0;
		m_Buffer	= 0;
		m_Stack		= NULL;
	}

	//-----------------------------------------------------
	void					Push			(int  x, int  y)
	{
		if( m_Size < m_Buffer || _Grow() )
		{
			m_Stack[m_Size].x	= x;
			m_Stack[m_Size].y	= y;

			m_Size++;
		}
	}

	//-----------------------------------------------------
	void					Pop				(int &x, int &y)
	{
		if( m_Size > 0 )
		{
			m_Size--;

			x	= m_Stack[m_Size].x;
			y	= m_Stack[m_Size].y;
		}
	}


private:

	typedef struct 
	{
		int					x, y;
	}
	TPoint;


	int						m_Size, m_Buffer;

	TPoint					*m_Stack;


	//-----------------------------------------------------
	bool					_Grow			(void)
	{
		TPoint	*Stack	= (TPoint *)SG_Realloc(m_Stack, (m_Buffer + 256) * sizeof(TPoint));

		if( Stack )
		{
			m_Stack		= Stack;
			m_Buffer	+= 256;
		}

		return( false );
	}

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CFlat_Detection : public CSG_Module_Grid  
{
public:
	CFlat_Detection(void);


protected:

	virtual bool			On_Execute		(void);


private:

	int						m_Flat_Output, m_nFlats;

	double					m_zFlat;

	CSG_Grid_Stack			m_Stack;

	CSG_Grid				*m_pDEM, *m_pNoFlats, *m_pFlats, m_Flats;


	bool					is_Flat			(int x, int y);

	void					Set_Flat_Cell	(int x, int y);
	void					Set_Flat		(int x, int y);

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Flat_Detection_H

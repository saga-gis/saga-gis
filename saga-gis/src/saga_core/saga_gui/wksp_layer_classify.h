
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 WKSP_Layer_Classify.h                 //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Layer_Classify_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Layer_Classify_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/string.h>

#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	CLASSIFY_SINGLE	= 0,
	CLASSIFY_LUT,
	CLASSIFY_DISCRETE,
	CLASSIFY_GRADUATED,
	CLASSIFY_OVERLAY,
	CLASSIFY_RGB,
	CLASSIFY_SHADE
};

//---------------------------------------------------------
enum
{
	LUT_COLOR		= 0,
	LUT_TITLE,
	LUT_DESCRIPTION,
	LUT_MIN,
	LUT_MAX
};

//---------------------------------------------------------
enum
{
	METRIC_MODE_NORMAL	= 0,
	METRIC_MODE_LOGUP,
	METRIC_MODE_LOGDOWN
};

//---------------------------------------------------------
enum
{
	SHADE_MODE_DSC_GREY	= 0,
	SHADE_MODE_ASC_GREY,
	SHADE_MODE_DSC_CYAN,
	SHADE_MODE_ASC_CYAN,
	SHADE_MODE_DSC_MAGENTA,
	SHADE_MODE_ASC_MAGENTA,
	SHADE_MODE_DSC_YELLOW,
	SHADE_MODE_ASC_YELLOW
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Scaler
{
public:
	typedef enum EInterval
	{
		LINEAR = 0, INCREASING, DECREASING
	}
	TInterval;

	//-----------------------------------------------------
	CSG_Scaler(void);

								CSG_Scaler			(double Minimum, double Maximum, double Interval = 0.);
	bool						Create				(double Minimum, double Maximum, double Interval = 0.);

	//-----------------------------------------------------
	bool						Set_Linear			(CSG_Table *pTable, int Field, double Interval, double Minimum, double Maximum);
	bool						Set_StdDev			(CSG_Table *pTable, int Field, double Interval, double StdDev, bool bKeepInRange);
	bool						Set_Percentile		(CSG_Table *pTable, int Field, double Interval, double Minimum, double Maximum);

	bool						Set_Linear			(CSG_Grid  *pGrid            , double Interval, double Minimum, double Maximum);
	bool						Set_StdDev			(CSG_Grid  *pGrid            , double Interval, double StdDev, bool bKeepInRange);
	bool						Set_Percentile		(CSG_Grid  *pGrid            , double Interval, double Minimum, double Maximum);

	bool						Set_Linear			(CSG_Grids *pGrids           , double Interval, double Minimum, double Maximum);
	bool						Set_StdDev			(CSG_Grids *pGrids           , double Interval, double StdDev, bool bKeepInRange);
	bool						Set_Percentile		(CSG_Grids *pGrids           , double Interval, double Minimum, double Maximum);

	//-----------------------------------------------------
	double						Get_Minimum			(void)	const	{	return( m_Minimum           );	}
	double						Get_Maximum			(void)	const	{	return( m_Minimum + m_Range );	}
	double						Get_Range			(void)	const	{	return(             m_Range );	}
	bool						Set_Range			(double Minimum, double Maximum);

	TInterval					Get_Interval_Mode	(void)	const	{	return( m_Interval          );	}
	double						Get_Interval		(void)	const	{	return( m_LogRange          );	}
	bool						Set_Interval		(double Interval);

	//-----------------------------------------------------
	double						to_Relative			(double Value)	const
	{
		if( m_Range > 0. )
		{
			Value	= (Value - m_Minimum) / m_Range;

			switch( m_Interval )
			{
			default:
				return( Value );

			case INCREASING:
				return( Value <= 0. ? 0. :      (log(1. + m_LogRange * (     Value)) / log(1. + m_LogRange)) );

			case DECREASING:
				return( Value >= 1. ? 1. : 1. - (log(1. + m_LogRange * (1. - Value)) / log(1. + m_LogRange)) );
			}
		}

		return( 0. );
	}

	//-----------------------------------------------------
	double						from_Relative		(double Value)	const
	{
		if( m_Range > 0. )
		{
			switch( m_Interval )
			{
			default:
				break;

			case INCREASING:
				Value	=      ((exp(log(1. + m_LogRange) * (     Value)) - 1.) / m_LogRange);
				break;

			case DECREASING:
				Value	= 1. - ((exp(log(1. + m_LogRange) * (1. - Value)) - 1.) / m_LogRange);
				break;
			}

			return( m_Minimum + (m_Range * Value) );
		}

		return( m_Minimum );
	}


protected:

	TInterval		m_Interval;

	double			m_Minimum, m_Range, m_LogRange;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Layer_Classify
{
public: ///////////////////////////////////////////////////
	CWKSP_Layer_Classify(void);
	virtual ~CWKSP_Layer_Classify(void);

	bool						Initialise				(class CWKSP_Layer *pLayer, CSG_Table *pLUT, CSG_Colors *pColors);

	void						Set_Mode				(int Mode)	{	m_Mode			= Mode;	}
	int							Get_Mode				(void)		{	return( m_Mode );		}

	void						Set_Shade_Mode			(int Mode)	{	m_Shade_Mode	= Mode;	}
	int							Get_Shade_Mode			(void)		{	return( m_Shade_Mode );	}

	void						Set_Unique_Color		(int Color);
	int							Get_Unique_Color		(void)		{	return( m_UNI_Color );	}


	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	bool						Set_Class_Count			(int Count);

	int							Get_Class_Count			(void)	const
	{
		switch( m_Mode )
		{
		case CLASSIFY_SINGLE   : default:
			return( 1 );

		case CLASSIFY_LUT      :
			return( m_pLUT->Get_Count() );

		case CLASSIFY_GRADUATED:
		case CLASSIFY_SHADE    :
		case CLASSIFY_OVERLAY  :
			return( m_Count );

		case CLASSIFY_DISCRETE :
			return( m_pColors->Get_Count() );
		}
	}

	//-----------------------------------------------------
	int							Get_Class				(double Value)
	{
		switch( m_Mode )
		{
		case CLASSIFY_SINGLE   : default:
			return( 0 );

		case CLASSIFY_LUT      :
			return( _LUT_Get_Class(Value) );

		case CLASSIFY_GRADUATED:
		case CLASSIFY_DISCRETE :
		case CLASSIFY_SHADE    :
		case CLASSIFY_OVERLAY  :
			return( _METRIC_Get_Class(Value) );
		}
	}

	int							Get_Class				(const CSG_String &Value)
	{
		if( m_Mode == CLASSIFY_LUT )
		{
			return( _LUT_Get_Class(Value) );
		}

		return( Get_Class(Value.asDouble()) );
	}

	//-----------------------------------------------------
	double						Get_Class_Value_Minimum	(int iClass);
	double						Get_Class_Value_Center	(int iClass);
	double						Get_Class_Value_Maximum	(int iClass);
	wxString					Get_Class_Name			(int iClass);
	wxString					Get_Class_Name_byValue	(double          Value);
	wxString					Get_Class_Name_byValue	(const wxString &Value);


	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	CSG_Colors					Get_Class_Colors		(void)	const;

	//-----------------------------------------------------
	bool						Get_Class_Color			(int iClass, int &Color)	const
	{
		switch( m_Mode )
		{
		case CLASSIFY_SINGLE   : default:
			Color	= m_UNI_Color;
			break;

		case CLASSIFY_LUT      :
			if( iClass < 0 || iClass >= m_pLUT->Get_Count() )
			{
				Color	= m_UNI_Color;

				return( false );
			}

			Color	= m_pLUT->Get_Record(iClass)->asInt(LUT_COLOR);
			break;


		case CLASSIFY_DISCRETE :
			Color	= m_pColors->Get_Color(iClass < 0 ? 0 : iClass >= m_pColors->Get_Count() ? m_pColors->Get_Count() - 1 : iClass);
			break;

		case CLASSIFY_GRADUATED:
		case CLASSIFY_SHADE    :
		case CLASSIFY_OVERLAY  :
			Get_Class_Color_byValue(Get_RelativeToMetric(iClass / (double)m_Count), Color);
			break;
		}

		return( true );
	}

	//-----------------------------------------------------
	int							Get_Class_Color			(int iClass)	const
	{
		int		Color;

		return( Get_Class_Color(iClass, Color) ? Color : 0 );
	}

	//-----------------------------------------------------
	bool						Get_Class_Color_byValue	(double Value, int &Color)	const
	{
		switch( m_Mode )
		{
		case CLASSIFY_SINGLE   : default:
			{
				return( Get_Class_Color(0, Color) );
			}

		case CLASSIFY_LUT      :
			{
				return( Get_Class_Color(_LUT_Get_Class(Value), Color) );
			}

		case CLASSIFY_DISCRETE :
			{
				return( Get_Class_Color(_METRIC_Get_Class(Value), Color) );
			}

		case CLASSIFY_GRADUATED:
			{
				double	iClass	= Get_MetricToRelative(Value) * (m_pColors->Get_Count() - 1);

				if( iClass < 0 )
				{
					Color	= m_pColors->Get_Color(0);
				}
				else if( iClass >= m_pColors->Get_Count() - 1 )
				{
					Color	= m_pColors->Get_Color(m_pColors->Get_Count() - 1);
				}
				else
				{
					int		a	= m_pColors->Get_Color(    (int)iClass);
					int		b	= m_pColors->Get_Color(1 + (int)iClass);
					double	d	= iClass - (int)iClass;

					Color	= SG_GET_RGB(
						SG_GET_R(a) + d * (SG_GET_R(b) - SG_GET_R(a)),
						SG_GET_G(a) + d * (SG_GET_G(b) - SG_GET_G(a)),
						SG_GET_B(a) + d * (SG_GET_B(b) - SG_GET_B(a))
					);
				}

				return( true );
			}

		case CLASSIFY_SHADE    :
			{
				int	iClass	= (int)(255.0 * Get_MetricToRelative(Value));

				if( iClass < 0 ) iClass = 0; else if( iClass > 255 ) iClass = 255;

				switch( m_Shade_Mode )
				{
				default:
				case SHADE_MODE_DSC_GREY   : Color = SG_GET_RGB(255 - iClass, 255 - iClass, 255 - iClass);	break;
				case SHADE_MODE_DSC_CYAN   : Color = SG_GET_RGB(255 - iClass, 255         , 255         );	break;
				case SHADE_MODE_DSC_MAGENTA: Color = SG_GET_RGB(255         , 255 - iClass, 255         );	break;
				case SHADE_MODE_DSC_YELLOW : Color = SG_GET_RGB(255         , 255         , 255 - iClass);	break;
				case SHADE_MODE_ASC_GREY   : Color = SG_GET_RGB(      iClass,       iClass,       iClass);	break;
				case SHADE_MODE_ASC_CYAN   : Color = SG_GET_RGB(      iClass, 255         , 255         );	break;
				case SHADE_MODE_ASC_MAGENTA: Color = SG_GET_RGB(255         ,       iClass, 255         );	break;
				case SHADE_MODE_ASC_YELLOW : Color = SG_GET_RGB(255         , 255         ,       iClass);	break;
				}

				return( true );
			}

		case CLASSIFY_OVERLAY  :
			{
				int	iClass	= (int)(255.0 * Get_MetricToRelative(Value));
			
				if( iClass < 0 ) iClass = 0; else if( iClass > 255 ) iClass = 255;

				Color	= SG_GET_RGB(iClass, iClass, iClass);

				return( true );
			}

		case CLASSIFY_RGB      :
			{
				Color	= (int)Value;
			
				return( true );
			}
		}
	}

	bool						Get_Class_Color_byValue	(const CSG_String &Value, int &Color)	const
	{
		if( m_Mode == CLASSIFY_LUT )
		{
			return( Get_Class_Color(_LUT_Get_Class(Value), Color) );
		}
		
		return( Get_Class_Color(Value.asDouble(), Color) );
	}

	//-----------------------------------------------------
	int							Get_Class_Color_byValue	(double Value)	const
	{
		int		Color;

		return( Get_Class_Color_byValue(Value, Color) ? Color : 0 );
	}

	int							Get_Class_Color_byValue	(const CSG_String &Value)	const
	{
		int		Color;

		return( Get_Class_Color_byValue(Value, Color) ? Color : 0 );
	}


	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	void						Set_Metric				(int Mode, double LogFactor, double zMin, double zMax);
	int							Get_Metric_Mode			(void)	const	{	return( m_zMode           );	}
	CSG_Colors *				Get_Metric_Colors		(void)	const	{	return( m_pColors         );	}
	double						Get_Metric_Minimum		(void)	const	{	return( m_zMin            );	}
	double						Get_Metric_Maximum		(void)	const	{	return( m_zMin + m_zRange );	}
	double						Get_Metric_Range		(void)	const	{	return(          m_zRange );	}
	double						Get_Metric_LogFactor	(void)	const	{	return( m_zLogRange       );	}

	void						Metric2EqualElements	(void);

	//-----------------------------------------------------
	double						Get_MetricToRelative	(double Value)	const
	{
		if( m_zRange > 0.0 )
		{
			Value	= (Value - m_zMin) / m_zRange;

			switch( m_zMode )
			{
			default:
				return( Value );

			case METRIC_MODE_LOGUP:
				return( Value > 0.0 ?       (log(1.0 + m_zLogRange * (      Value)) / m_zLogMax) : 0.0 );

			case METRIC_MODE_LOGDOWN:
				return( Value < 1.0 ? 1.0 - (log(1.0 + m_zLogRange * (1.0 - Value)) / m_zLogMax) : 1.0 );
			}
		}

		return( 0.0 );
	}

	//-----------------------------------------------------
	double						Get_RelativeToMetric	(double Value)	const
	{
		switch( m_zMode )
		{
		case METRIC_MODE_LOGUP:
			Value	=       ((exp(m_zLogMax * (      Value)) - 1.0) / m_zLogRange);
			break;

		case METRIC_MODE_LOGDOWN:
			Value	= 1.0 - ((exp(m_zLogMax * (1.0 - Value)) - 1.0) / m_zLogRange);
			break;
		}

		return( m_zMin + (m_zRange * Value) );
	}


	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	bool							Histogram_Update	(void);

	const CSG_Histogram &			Histogram_Get		(void)	const	{	return( m_Histogram );	}

	const CSG_Simple_Statistics &	Statistics_Get		(void)	const	{	return( m_Statistics );	}


protected: ////////////////////////////////////////////////

	int							m_Mode, m_zMode, m_Shade_Mode, m_Count, m_UNI_Color;

	double						m_zMin, m_zRange, m_zLogRange, m_zLogMax;

	CSG_Histogram				m_Histogram;

	CSG_Simple_Statistics		m_Statistics;

	CSG_Colors					*m_pColors;

	CSG_Table					*m_pLUT;

	class CWKSP_Layer			*m_pLayer;


	//-----------------------------------------------------
	int							_LUT_Cmp_Class			(double            Value, int iClass)	const;
	int							_LUT_Get_Class			(double            Value            )	const;

	int							_LUT_Cmp_Class			(const CSG_String &Value, int iClass)	const;
	int							_LUT_Get_Class			(const CSG_String &Value            )	const;

	int							_METRIC_Get_Class		(double Value)	const
	{
		if( Value < m_zMin )
		{
			return( -1 );
		}

		if( Value > m_zMin + m_zRange )
		{
			return( Get_Class_Count() );
		}

		int	Class	= (int)(Get_MetricToRelative(Value) * Get_Class_Count());

		return( Class < 0 ? 0 : Class < Get_Class_Count() ? Class : Get_Class_Count() - 1 );
	}


	//-----------------------------------------------------
	bool						_Histogram_Update		(CSG_Grid  *pGrid );
	bool						_Histogram_Update		(CSG_Grids *pGrids);
	bool						_Histogram_Update		(CSG_Shapes *pShapes, int Attribute, int Normalize = -1, double Scale = 1.);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Layer_Classify_H

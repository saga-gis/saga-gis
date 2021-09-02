
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
//                  VIEW_Table_Data.h                    //
//                                                       //
//          Copyright (C) 2021 by Olaf Conrad            //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Table_Data_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Table_Data_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/grid.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Table_Data : public wxGridTableBase
{
public:
	CVIEW_Table_Data(CSG_Table *pTable);
	~CVIEW_Table_Data(void);

	//-----------------------------------------------------
	virtual int			GetNumberRows		(void)
	{
		return( m_bSelection ? m_pTable->Get_Selection_Count() : m_pTable->Get_Record_Count() );
	}

	virtual int			GetNumberCols		(void)
	{
		return( m_bRowLabels ? m_pTable->Get_Field_Count() - 1 : m_pTable->Get_Field_Count() );
	}

	//-----------------------------------------------------
	CSG_Table_Record *	Get_Record			(int iRecord)
	{
		return( m_bSelection
			? m_pTable->Get_Selection((size_t)iRecord)
			: m_pTable->Get_Record_byIndex(   iRecord)
		);
	}

	CSG_Table_Record *	Get_Record			(int iRecord, int iField)
	{
		if( iField >= 0 && iField < m_pTable->Get_Field_Count() )
		{
			return( m_bSelection
				? m_pTable->Get_Selection((size_t)iRecord)
				: m_pTable->Get_Record_byIndex(   iRecord)
			);
		}

		return( NULL );
	}

	//-----------------------------------------------------
	virtual wxString	GetRowLabelValue	(int iRecord)
	{
		if( m_bRowLabels )
		{
			return( Get_Record(iRecord)->asString(0) );
		}

		return( wxGridTableBase::GetRowLabelValue(iRecord) );
	}

	virtual wxString	GetColLabelValue	(int iField)
	{
		if( m_bRowLabels ) { iField++; }

		if( iField >= 0 && iField < m_pTable->Get_Field_Count() )
		{
			return( m_pTable->Get_Field_Name(iField) );
		}

		return( wxEmptyString );
	}

	//-----------------------------------------------------
	virtual bool		InsertRows			(size_t Position = 0, size_t Number = 1);
	virtual bool		AppendRows			(                     size_t Number = 1);
	virtual bool		DeleteRows			(size_t Position = 0, size_t Number = 1);
	virtual bool		InsertCols			(size_t Position = 0, size_t Number = 1);
	virtual bool		AppendCols			(                     size_t Number = 1);
	virtual bool		DeleteCols			(size_t Position = 0, size_t Number = 1);

	//-----------------------------------------------------
	virtual wxString	GetTypeName			(int iRecord, int iField)
	{
		if( iField >= 0 && iField < m_pTable->Get_Field_Count() )
		{
			switch( m_pTable->Get_Field_Type(iField) )
			{
			case SG_DATATYPE_Bit   : return( wxGRID_VALUE_BOOL   );

			case SG_DATATYPE_Byte  :
			case SG_DATATYPE_Char  :
			case SG_DATATYPE_Word  :
			case SG_DATATYPE_Short :
			case SG_DATATYPE_DWord :
			case SG_DATATYPE_Int   :
			case SG_DATATYPE_ULong :
			case SG_DATATYPE_Long  : return( wxGRID_VALUE_NUMBER );

			case SG_DATATYPE_Float :
			case SG_DATATYPE_Double: return( wxGRID_VALUE_FLOAT  );

			case SG_DATATYPE_Color : return( wxGRID_VALUE_STRING );

			#if !(wxMAJOR_VERSION == 3 && wxMINOR_VERSION <= 1 && wxRELEASE_NUMBER < 5)
			case SG_DATATYPE_Date  : return( wxGRID_VALUE_DATE   );
			#endif

			default                : break;
			}
		}

		return( wxGRID_VALUE_STRING );
	}

	//-----------------------------------------------------
	virtual bool		IsEmptyCell			(int iRecord, int iField)
	{
		CSG_Table_Record *pRecord = Get_Record(iRecord, iField);

		return( !pRecord || pRecord->is_NoData(iField) );
	}

	//-----------------------------------------------------
	virtual wxString	GetValue			(int iRecord, int iField)
	{
		if( m_bRowLabels ) { iField++; } CSG_Table_Record *pRecord = Get_Record(iRecord, iField);

		if( pRecord && !pRecord->is_NoData(iField) )
		{
			switch( m_pTable->Get_Field_Type(iField) )
			{
			default:
				{
					return( pRecord->asString(iField) );
				}

			case SG_DATATYPE_Color:
				{
					int c = pRecord->asInt(iField);

					return( wxString::Format("#%02X%02X%02X", SG_GET_R(c), SG_GET_G(c), SG_GET_B(c)) );
				}
			}
		}

		return( wxEmptyString );
	}

	//-----------------------------------------------------
	virtual long		GetValueAsLong		(int iRecord, int iField)
	{
		if( m_bRowLabels ) { iField++; } CSG_Table_Record *pRecord = Get_Record(iRecord, iField);

		return( pRecord ? pRecord->asInt(iField) : 0l );
	}

	//-----------------------------------------------------
	virtual double		GetValueAsDouble	(int iRecord, int iField)
	{
		if( m_bRowLabels ) { iField++; } CSG_Table_Record *pRecord = Get_Record(iRecord, iField);

		return( pRecord ? pRecord->asDouble(iField) : 0. );
	}

	//-----------------------------------------------------
	virtual bool		GetValueAsBool		(int iRecord, int iField)
	{
		if( m_bRowLabels ) { iField++; } CSG_Table_Record *pRecord = Get_Record(iRecord, iField);

		return( pRecord ? pRecord->asInt(iField) != 0 : false );
	}

	//-----------------------------------------------------
	virtual void		SetValue			(int iRecord, int iField, const wxString &Value);
	virtual void		SetValueAsLong		(int iRecord, int iField, long            Value);
	virtual void		SetValueAsDouble	(int iRecord, int iField, double          Value);
	virtual void		SetValueAsBool		(int iRecord, int iField, bool            Value);

	//-----------------------------------------------------
	bool				On_Changed			(int iRecord, int iField);
	bool				On_Sort				(void);

	//-----------------------------------------------------
	bool				Sort				(int iField, int Direction);

	bool				To_Clipboard		(void);

	bool				is_DataSource		(wxString &Source);

	//-----------------------------------------------------
	bool				m_bSelection, m_bRowLabels;

	CSG_Table			*m_pTable;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Table_Data_H

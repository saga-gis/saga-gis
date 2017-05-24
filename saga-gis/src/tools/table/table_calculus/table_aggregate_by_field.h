
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    table_calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               table_aggregate_by_field.h              //
//                                                       //
//                 Copyright (C) 2017 by                 //
//                      Olaf Conrad                      //
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
#ifndef HEADER_INCLUDED__table_aggregate_by_field_H
#define HEADER_INCLUDED__table_aggregate_by_field_H


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Aggregate_by_Field : public CSG_Tool
{
public:
	CTable_Aggregate_by_Field(void);


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	bool						m_bSUM, m_bAVG, m_bMIN, m_bMAX, m_bRNG, m_bDEV, m_bVAR, m_bLST, m_bNUM;

	int							m_Stat_Offset;

	CSG_Strings					m_List;

	CSG_Parameter_Table_Fields	*m_Stat_pFields;

	CSG_Simple_Statistics		*m_Statistics;


	bool						Get_Aggregated			(CSG_Table_Record *pAggregate);

	bool						Statistics_Initialize	(CSG_Table *pAggregated, CSG_Table *pTable);
	CSG_String					Statistics_Get_Name		(const CSG_String &Type, const CSG_String &Name);
	bool						Statistics_Add			(CSG_Table_Record *pAggregate, CSG_Table_Record *pRecord, bool bReset);

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__table_aggregate_by_field_H

/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    Table_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   MLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                    Andre Ringeler                     //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     aringel@gwdg.de                        //
//                                                       //
//    contact:    Andre Ringeler                         //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//			The Module Link Library Interface			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include "MLB_Interface.h"


//---------------------------------------------------------
// 2. Place general module library informations here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Calculus") );

	case MLB_INFO_Category:
		return( _TL("Table") );

	case MLB_INFO_Author:
		return( _TL("SAGA User Group Associaton (c) 2002-2014") );

	case MLB_INFO_Description:
		return( _TL("Tools for table based analyses and calculations.") );

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Table|Calculus") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "Fit.h"
#include "Table_Calculator.h"
#include "table_running_average.h"
#include "table_cluster_analysis.h"
#include "table_pca.h"
#include "table_fill_record_gaps.h"
#include "table_field_analyzer.h"
#include "table_mRMR.h"
#include "table_field_statistics.h"
#include "table_record_statistics.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case  0:	return( new CFit );

	case  1:	return( new CTable_Calculator );
	case  2:	return( new CTable_Calculator_Shapes );

	case  5:	return( new CTable_Running_Average );
	case  6:	return( new CTable_Cluster_Analysis(false) );
	case 14:	return( new CTable_Cluster_Analysis(true) );
	case  7:	return( new CTable_PCA );

	case  8:	return( new CTable_Fill_Record_Gaps );

	case 11:	return( new CTable_Field_Extreme );

	case 12:	return( new CTable_mRMR );

	case 15:	return( new CTable_Field_Statistics() );
	case 16:	return( new CTable_Record_Statistics() );
	case 17:	return( new CTable_Record_Statistics_Shapes() );

	//-----------------------------------------------------
	case 19:	return( NULL );
	default:	return( MLB_INTERFACE_SKIP_MODULE );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//{{AFX_SAGA

	MLB_INTERFACE

//}}AFX_SAGA

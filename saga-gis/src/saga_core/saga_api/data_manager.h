
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    data_manager.h                     //
//                                                       //
//          Copyright (C) 2013 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
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
#ifndef HEADER_INCLUDED__SAGA_API__data_manager_H
#define HEADER_INCLUDED__SAGA_API__data_manager_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** \file data_manager.h
* Classes for data object management and declaration of the
* SAGA API's default data manager.
* @see SG_Get_Data_Manager
* @see CSG_Data_Manager
* @see CSG_Data_Object
*/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "parameters.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Data_Collection
{
	friend class CSG_Data_Manager;

public:

	size_t							Count				(void)		const	{	return( m_Objects.Get_Size() );	}

	CSG_Data_Object &				operator []			(size_t i)	const	{	return(              *(CSG_Data_Object *)m_Objects[i]        );	}
	CSG_Data_Object *				Get					(size_t i)	const	{	return( i < Count() ? (CSG_Data_Object *)m_Objects[i] : NULL );	}

	bool							Delete				(CSG_Data_Object *pObject, bool bDetach = false);
	bool							Delete				(size_t i                , bool bDetach = false);
	bool							Delete				(                          bool bDetach = false, bool bUnsaved = false);

	CSG_Data_Object *				Find				(const CSG_String &File, bool bNative = true)	const;

	bool							Exists				(CSG_Data_Object *pObject)	const;


protected:

	CSG_Data_Collection(TSG_Data_Object_Type Type);
	virtual ~CSG_Data_Collection(void);

	bool							Add					(CSG_Data_Object *pObject);


	TSG_Data_Object_Type			m_Type;

	CSG_Array_Pointer				m_Objects;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Data_Manager
{
public:
	CSG_Data_Manager(void);
	virtual ~CSG_Data_Manager(void);

	CSG_Data_Collection &			Table				(void)		const	{	return( *m_pTable      );	}
	CSG_Data_Collection &			TIN					(void)		const	{	return( *m_pTIN        );	}
	CSG_Data_Collection &			PointCloud			(void)		const	{	return( *m_pPointCloud );	}
	CSG_Data_Collection &			Shapes				(void)		const	{	return( *m_pShapes     );	}
	CSG_Data_Collection &			Grid				(void)		const	{	return( *m_pGrid       );	}
	CSG_Data_Collection &			Grids				(void)		const	{	return( *m_pGrids      );	}

	CSG_Table &						Table				(size_t i)	const	{	return( *m_pTable     ->Get(i)->asTable     () );	}
	CSG_TIN &						TIN					(size_t i)	const	{	return( *m_pTIN       ->Get(i)->asTIN       () );	}
	CSG_PointCloud &				PointCloud			(size_t i)	const	{	return( *m_pPointCloud->Get(i)->asPointCloud() );	}
	CSG_Shapes &					Shapes				(size_t i)	const	{	return( *m_pShapes    ->Get(i)->asShapes    () );	}
	CSG_Grid &						Grid				(size_t i)	const	{	return( *m_pGrid      ->Get(i)->asGrid      () );	}
	CSG_Grids &						Grids				(size_t i)	const	{	return( *m_pGrids     ->Get(i)->asGrids     () );	}

	CSG_Data_Object *				Add					(CSG_Data_Object *pObject);

	CSG_Data_Object *				Add					(const CSG_String &File, TSG_Data_Object_Type Type = SG_DATAOBJECT_TYPE_Undefined);
	CSG_Data_Object *				Add					(const char       *File, TSG_Data_Object_Type Type = SG_DATAOBJECT_TYPE_Undefined);
	CSG_Data_Object *				Add					(const wchar_t    *File, TSG_Data_Object_Type Type = SG_DATAOBJECT_TYPE_Undefined);

	CSG_Table *						Add_Table			(void);
	CSG_Table *						Add_Table			(const CSG_String &File);
	CSG_Table *						Add_Table			(const char       *File);
	CSG_Table *						Add_Table			(const wchar_t    *File);

	CSG_Shapes *					Add_Shapes			(void);
	CSG_Shapes *					Add_Shapes			(const CSG_String &File);
	CSG_Shapes *					Add_Shapes			(const char       *File);
	CSG_Shapes *					Add_Shapes			(const wchar_t    *File);

	CSG_PointCloud *				Add_PointCloud		(void);
	CSG_PointCloud *				Add_PointCloud		(const CSG_String &File);
	CSG_PointCloud *				Add_PointCloud		(const char       *File);
	CSG_PointCloud *				Add_PointCloud		(const wchar_t    *File);

	CSG_TIN *						Add_TIN				(void);
	CSG_TIN *						Add_TIN				(const CSG_String &File);
	CSG_TIN *						Add_TIN				(const char       *File);
	CSG_TIN *						Add_TIN				(const wchar_t    *File);

	CSG_Grid *						Add_Grid			(void);
	CSG_Grid *						Add_Grid			(const CSG_String &File);
	CSG_Grid *						Add_Grid			(const char       *File);
	CSG_Grid *						Add_Grid			(const wchar_t    *File);

	CSG_Grids *						Add_Grids			(void);
	CSG_Grids *						Add_Grids			(const CSG_String &File);
	CSG_Grids *						Add_Grids			(const char       *File);
	CSG_Grids *						Add_Grids			(const wchar_t    *File);

	bool							Delete				(CSG_Data_Object *pObject, bool bDetach = false);
	bool							Delete				(                          bool bDetach = false, bool bUnsaved = false);

	CSG_Data_Object *				Find				(const CSG_String &File, bool bNative = true)	const;

	bool							Exists				(CSG_Data_Object *pObject)	const;

	CSG_String						Get_Summary			(void)	const;


private:

	CSG_Data_Collection				*m_pTable, *m_pShapes, *m_pPointCloud, *m_pTIN, *m_pGrid, *m_pGrids;
	

	CSG_Data_Collection *			_Get_Collection		(CSG_Data_Object *pObject)	const;

	CSG_Data_Object *				_Add_External		(const CSG_String &File);

};

//---------------------------------------------------------
SAGA_API_DLL_EXPORT CSG_Data_Manager &	SG_Get_Data_Manager	(void);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__data_manager_H

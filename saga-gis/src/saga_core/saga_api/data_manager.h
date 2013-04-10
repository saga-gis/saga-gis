/**********************************************************
 * Version $Id: data_manager.h 1428 2012-06-06 14:33:41Z oconrad $
 *********************************************************/

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
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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

	CSG_Data_Object *				Get					(size_t i)	const	{	return( i < Count() ? ((CSG_Data_Object **)m_Objects.Get_Array())[i] : NULL );	}
	CSG_Data_Object *				Get					(const CSG_String &File)	const;

	virtual bool					Exists				(CSG_Data_Object *pObject)	const;


protected:

	CSG_Data_Collection(class CSG_Data_Manager *pManager, TSG_Data_Object_Type Type);
	virtual ~CSG_Data_Collection(void);

	virtual bool					Add					(CSG_Data_Object *pObject);
	bool							Delete				(CSG_Data_Object *pObject, bool bDetachOnly = false);
	bool							Delete				(size_t i                , bool bDetachOnly = false);
	bool							Delete_All			(                          bool bDetachOnly = false);


	class CSG_Data_Manager			*m_pManager;

	TSG_Data_Object_Type			m_Type;

	CSG_Array						m_Objects;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Shapes_Collection : public CSG_Data_Collection
{
	friend class CSG_Data_Manager;

public:

	virtual bool					Exists				(CSG_Data_Object *pObject)	const;

	TSG_Shape_Type					Get_Shape_Type		(void)	const	{	return( m_Shape_Type );	}


protected:

	CSG_Shapes_Collection(class CSG_Data_Manager *pManager, TSG_Shape_Type Type);

	virtual bool					Add					(CSG_Data_Object *pObject);


	TSG_Shape_Type					m_Shape_Type;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Grid_Collection : public CSG_Data_Collection
{
	friend class CSG_Data_Manager;

public:

	virtual bool					Exists				(CSG_Data_Object *pObject)	const;

	const CSG_Grid_System &			Get_System			(void)	const	{	return( m_System );	}

	bool							is_Equal			(const CSG_Grid_System &System)	const
	{
		return( Count() > 0 && m_System.is_Equal(System) );
	}


protected:

	CSG_Grid_Collection(class CSG_Data_Manager *pManager);

	virtual bool					Add					(CSG_Data_Object *pObject);


	CSG_Grid_System					m_System;

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

	CSG_Data_Collection *				Get_Table			(void)		const	{	return( m_pTable       );	}
	CSG_Data_Collection *				Get_TIN				(void)		const	{	return( m_pTIN         );	}
	CSG_Data_Collection *				Get_Point_Cloud		(void)		const	{	return( m_pPoint_Cloud );	}
	CSG_Shapes_Collection *				Get_Point			(void)		const	{	return( m_pPoint       );	}
	CSG_Shapes_Collection *				Get_Points			(void)		const	{	return( m_pPoints      );	}
	CSG_Shapes_Collection *				Get_Line			(void)		const	{	return( m_pLine        );	}
	CSG_Shapes_Collection *				Get_Polygon			(void)		const	{	return( m_pPolygon     );	}

	size_t								Grid_System_Count	(void)		const	{	return( m_Grid_Systems.Get_Size() );	}
	CSG_Grid_Collection *				Get_Grid_System		(size_t i)	const	{	return( i < Grid_System_Count() ? ((CSG_Grid_Collection **)m_Grid_Systems.Get_Array())[i] : NULL );	}
	CSG_Grid_Collection *				Get_Grid_System		(const CSG_Grid_System &System)	const;

	bool								Add					(CSG_Data_Object *pObject);
	bool								Add					(const CSG_String &File, TSG_Data_Object_Type Type = DATAOBJECT_TYPE_Undefined);

	bool								Delete				(CSG_Data_Collection *pCollection, bool bDetachOnly = false);
	bool								Delete				(CSG_Data_Object     *pObject    , bool bDetachOnly = false);
	bool								Delete				(const CSG_Grid_System &System   , bool bDetachOnly = false);
	bool								Delete_All			(                                  bool bDetachOnly = false);

	bool								Exists				(CSG_Data_Object *pObject)		const;
	bool								Exists				(const CSG_Grid_System &System)	const;

	CSG_Data_Object *					Find				(const CSG_String &File)		const;

	CSG_String							Get_Summary			(void)	const;


private:

	CSG_Array							m_Grid_Systems;

	CSG_Data_Collection					*m_pTable, *m_pTIN, *m_pPoint_Cloud;
	
	CSG_Shapes_Collection				*m_pPoint, *m_pPoints, *m_pLine, *m_pPolygon;


	CSG_Data_Collection *				_Get_Collection		(CSG_Data_Object *pObject)		const;

	bool								_Add_External		(const CSG_String &File);

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

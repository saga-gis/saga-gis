
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

	CSG_Data_Object *				Get					(size_t i)	const	{	return( i < Count() ? (CSG_Data_Object *)m_Objects[i] : NULL );	}
	CSG_Data_Object *				Get					(const CSG_String &File, bool bNative = true)	const;

	virtual bool					Exists				(CSG_Data_Object *pObject)	const;


protected:

	CSG_Data_Collection(class CSG_Data_Manager *pManager, TSG_Data_Object_Type Type);
	virtual ~CSG_Data_Collection(void);

	virtual bool					Add					(CSG_Data_Object *pObject);
	bool							Delete				(CSG_Data_Object *pObject, bool bDetachOnly = false);
	bool							Delete				(size_t i                , bool bDetachOnly = false);
	bool							Delete_All			(                          bool bDetachOnly = false);
	bool							Delete_Unsaved		(                          bool bDetachOnly = false);


	class CSG_Data_Manager			*m_pManager;

	TSG_Data_Object_Type			m_Type;

	CSG_Array_Pointer				m_Objects;

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
	CSG_Data_Collection *				asTIN				(void)		const	{	return( m_pTIN         );	}
	CSG_Data_Collection *				Get_Point_Cloud		(void)		const	{	return( m_pPoint_Cloud );	}
	CSG_Data_Collection *				Get_Shapes			(void)		const	{	return( m_pShapes      );	}

	size_t								Grid_System_Count	(void)		const	{	return( m_Grid_Systems.Get_Size() );	}
	CSG_Grid_Collection *				Get_Grid_System		(size_t i)	const	{	return( i < Grid_System_Count() ? (CSG_Grid_Collection *)m_Grid_Systems[i] : NULL );	}
	CSG_Grid_Collection *				Get_Grid_System		(const CSG_Grid_System &System)	const;

	bool								Add					(CSG_Data_Object *pObject);
	CSG_Data_Object *					Add					(const CSG_String &File, TSG_Data_Object_Type Type = SG_DATAOBJECT_TYPE_Undefined);
	CSG_Data_Object *					Add					(const char       *File, TSG_Data_Object_Type Type = SG_DATAOBJECT_TYPE_Undefined)	{	return( Add(CSG_String(File), Type) );	}
	CSG_Data_Object *					Add					(const wchar_t    *File, TSG_Data_Object_Type Type = SG_DATAOBJECT_TYPE_Undefined)	{	return( Add(CSG_String(File), Type) );	}

	CSG_Table *							Add_Table			(const CSG_String &File)	{	return( (CSG_Table      *)Add(File, SG_DATAOBJECT_TYPE_Table     ) );	}
	CSG_Table *							Add_Table			(const char       *File)	{	return( (CSG_Table      *)Add(File, SG_DATAOBJECT_TYPE_Table     ) );	}
	CSG_Table *							Add_Table			(const wchar_t    *File)	{	return( (CSG_Table      *)Add(File, SG_DATAOBJECT_TYPE_Table     ) );	}
	CSG_TIN *							Add_TIN				(const CSG_String &File)	{	return( (CSG_TIN        *)Add(File, SG_DATAOBJECT_TYPE_TIN       ) );	}
	CSG_TIN *							Add_TIN				(const char       *File)	{	return( (CSG_TIN        *)Add(File, SG_DATAOBJECT_TYPE_TIN       ) );	}
	CSG_TIN *							Add_TIN				(const wchar_t    *File)	{	return( (CSG_TIN        *)Add(File, SG_DATAOBJECT_TYPE_TIN       ) );	}
	CSG_PointCloud *					Add_PointCloud		(const CSG_String &File)	{	return( (CSG_PointCloud *)Add(File, SG_DATAOBJECT_TYPE_PointCloud) );	}
	CSG_PointCloud *					Add_PointCloud		(const char       *File)	{	return( (CSG_PointCloud *)Add(File, SG_DATAOBJECT_TYPE_PointCloud) );	}
	CSG_PointCloud *					Add_PointCloud		(const wchar_t    *File)	{	return( (CSG_PointCloud *)Add(File, SG_DATAOBJECT_TYPE_PointCloud) );	}
	CSG_Shapes *						Add_Shapes			(const CSG_String &File)	{	return( (CSG_Shapes     *)Add(File, SG_DATAOBJECT_TYPE_Shapes    ) );	}
	CSG_Shapes *						Add_Shapes			(const char       *File)	{	return( (CSG_Shapes     *)Add(File, SG_DATAOBJECT_TYPE_Shapes    ) );	}
	CSG_Shapes *						Add_Shapes			(const wchar_t    *File)	{	return( (CSG_Shapes     *)Add(File, SG_DATAOBJECT_TYPE_Shapes    ) );	}
	CSG_Grid *							Add_Grid			(const CSG_String &File)	{	return( (CSG_Grid       *)Add(File, SG_DATAOBJECT_TYPE_Grid      ) );	}
	CSG_Grid *							Add_Grid			(const char       *File)	{	return( (CSG_Grid       *)Add(File, SG_DATAOBJECT_TYPE_Grid      ) );	}
	CSG_Grid *							Add_Grid			(const wchar_t    *File)	{	return( (CSG_Grid       *)Add(File, SG_DATAOBJECT_TYPE_Grid      ) );	}
	CSG_Grids *							Add_Grids			(const CSG_String &File)	{	return( (CSG_Grids      *)Add(File, SG_DATAOBJECT_TYPE_Grids     ) );	}
	CSG_Grids *							Add_Grids			(const char       *File)	{	return( (CSG_Grids      *)Add(File, SG_DATAOBJECT_TYPE_Grids     ) );	}
	CSG_Grids *							Add_Grids			(const wchar_t    *File)	{	return( (CSG_Grids      *)Add(File, SG_DATAOBJECT_TYPE_Grids     ) );	}

	CSG_Table *							Add_Table			(void);
	CSG_TIN *							Add_TIN				(void);
	CSG_PointCloud *					Add_PointCloud		(void);
	CSG_Shapes *						Add_Shapes			(TSG_Shape_Type Type = SHAPE_TYPE_Undefined);
	CSG_Grid *							Add_Grid			(const CSG_Grid_System &System, TSG_Data_Type Type = SG_DATATYPE_Undefined);
	CSG_Grid *							Add_Grid			(int NX, int NY, double Cellsize = 0.0, double xMin = 0.0, double yMin = 0.0, TSG_Data_Type Type = SG_DATATYPE_Undefined);

	bool								Delete				(CSG_Data_Collection *pCollection, bool bDetachOnly = false);
	bool								Delete				(CSG_Data_Object     *pObject    , bool bDetachOnly = false);
	bool								Delete				(const CSG_Grid_System &System   , bool bDetachOnly = false);
	bool								Delete_All			(                                  bool bDetachOnly = false);
	bool								Delete_Unsaved		(                                  bool bDetachOnly = false);

	bool								Exists				(CSG_Data_Object *pObject)		const;
	bool								Exists				(const CSG_Grid_System &System)	const;

	CSG_Data_Object *					Find				(const CSG_String &File, bool bNative = true)	const;

	CSG_String							Get_Summary			(void)	const;


private:

	CSG_Array_Pointer					m_Grid_Systems;

	CSG_Data_Collection					*m_pTable, *m_pTIN, *m_pPoint_Cloud, *m_pShapes;
	

	CSG_Data_Collection *				_Get_Collection		(CSG_Data_Object *pObject)		const;

	CSG_Data_Object *					_Add_External		(const CSG_String &File);

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

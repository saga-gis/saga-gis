
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
//                     pointcloud.h                      //
//                                                       //
//          Copyright (C) 2009 by Olaf Conrad            //
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
#ifndef HEADER_INCLUDED__SAGA_API__pointcloud_H
#define HEADER_INCLUDED__SAGA_API__pointcloud_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "dataobject.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_PointCloud_FieldType
{
	POINTCLOUD_FIELDTYPE_None		= 0,
	POINTCLOUD_FIELDTYPE_Char,
	POINTCLOUD_FIELDTYPE_Short,
	POINTCLOUD_FIELDTYPE_Int,
	POINTCLOUD_FIELDTYPE_Long,
	POINTCLOUD_FIELDTYPE_Float,
	POINTCLOUD_FIELDTYPE_Double
}
TSG_PointCloud_Field_Type;

//---------------------------------------------------------
const SG_Char	gSG_PointCloud_Field_Type_Names[][32]	=
{
	SG_T("NONE"),
	SG_T("CHARACTER"),
	SG_T("SHORTINT"),
	SG_T("INTEGER"),
	SG_T("LONGINT"),
	SG_T("FLOAT"),
	SG_T("DOUBLE"),
	SG_T("STRING"),
	SG_T("COLOR"),
	SG_T("DATE")
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_PointCloud : public CSG_Data_Object
{
public:

									CSG_PointCloud		(void);
	bool							Create				(void);

									CSG_PointCloud		(const CSG_PointCloud &PointCloud);
	bool							Create				(const CSG_PointCloud &PointCloud);

									CSG_PointCloud		(const CSG_String &File_Name);
	bool							Create				(const CSG_String &File_Name);

									CSG_PointCloud		(CSG_PointCloud *pStructure);
	bool							Create				(CSG_PointCloud *pStructure);

	virtual ~CSG_PointCloud(void);

	virtual bool					Destroy				(void);

	virtual TSG_Data_Object_Type	Get_ObjectType		(void)	const			{	return( DATAOBJECT_TYPE_PointCloud );	}

	virtual bool					Assign				(CSG_Data_Object *pSource);

	virtual bool					Save				(const CSG_String &File_Name, int Format = 0);

	const CSG_Rect &				Get_Extent			(void);

	void							Set_XYZ_Precision	(bool bDouble)			{	m_bXYZPrecDbl	= bDouble;	}

	//-----------------------------------------------------
	virtual bool					is_Valid			(void)	const			{	return( m_nPoints > 0 );	}
	bool							is_Compatible		(CSG_PointCloud *pPointCloud)	const;

	//-----------------------------------------------------
	bool							Add_Field			(const SG_Char *Name, TSG_PointCloud_Field_Type Type);
#ifdef _SAGA_UNICODE
	bool							Add_Field			(const char    *Name, TSG_PointCloud_Field_Type Type);
#endif

	int								Get_Field_Count		(void)			const	{	return( m_nFields );	}
	const SG_Char *					Get_Field_Name		(int iField)	const	{	return( iField >= 0 && iField < m_nFields ? m_Field_Name[iField]->c_str() : NULL );				}
	TSG_PointCloud_Field_Type		Get_Field_Type		(int iField)	const	{	return( iField >= 0 && iField < m_nFields ? m_Field_Type[iField] : POINTCLOUD_FIELDTYPE_None );	}

	int								Get_Attribute_Count	(void)			const	{	return( m_nFields );	}
	const SG_Char *					Get_Attribute_Name	(int iField)	const	{	return( iField >= 0 && iField < m_nFields ? m_Field_Name[iField]->c_str() : NULL );				}
	TSG_PointCloud_Field_Type		Get_Attribute_Type	(int iField)	const	{	return( iField >= 0 && iField < m_nFields ? m_Field_Type[iField] : POINTCLOUD_FIELDTYPE_None );	}

	double							Get_Minimum			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Minimum()  : 0.0 );	}
	double							Get_Maximum			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Maximum()  : 0.0 );	}
	double							Get_Range			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Range()    : 0.0 );	}
	double							Get_Mean			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Mean()     : 0.0 );	}
	double							Get_StdDev			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_StdDev()   : 0.0 );	}
	double							Get_Variance		(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Variance() : 0.0 );	}

	//-----------------------------------------------------
	bool							Add_Point			(double x, double y, double z);
	bool							Del_Point			(int iPoint);
	bool							Del_Points			(void);

	int								Get_Point_Count		(void)			const	{	return( m_nPoints );	}
	int								Get_Count			(void)			const	{	return( m_nPoints );	}

	//-----------------------------------------------------
	bool							Set_Cursor			(int iPoint)				{	return( (m_Cursor = iPoint >= 0 && iPoint < m_nPoints ? m_Points[iPoint] : NULL) != NULL );	}
	bool							Set_Value			(int iField, double Value)	{	return( _Set_Field_Value(m_Cursor, iField, Value) );	}
	double							Get_Value			(int iField)	const		{	return( _Get_Field_Value(m_Cursor, iField) );			}
	double							Get_X				(void)			const		{	return( _Get_Field_Value(m_Cursor, 0) );				}
	double							Get_Y				(void)			const		{	return( _Get_Field_Value(m_Cursor, 1) );				}
	double							Get_Z				(void)			const		{	return( _Get_Field_Value(m_Cursor, 2) );				}
	bool							Set_Attribute		(int iField, double Value)	{	return( Set_Value(iField + 3, Value) );					}
	double							Get_Attribute		(int iField)	const		{	return( Get_Value(iField + 3) );						}
	bool							Set_NoData			(int iField)				{	return( Set_Attribute(iField, m_NoData_Value) );		}
	bool							is_NoData			(int iField)	const		{	return( Get_Attribute(iField) == m_NoData_Value );		}

	bool							Set_Value			(int iPoint, int iField, double Value)	{	return( _Set_Field_Value(iPoint >= 0 && iPoint < m_nPoints ? m_Points[iPoint] : NULL, iField, Value) );	}
	double							Get_Value			(int iPoint, int iField)	const		{	return( _Get_Field_Value(iPoint >= 0 && iPoint < m_nPoints ? m_Points[iPoint] : NULL, iField) );		}
	double							Get_X				(int iPoint)				const		{	return( _Get_Field_Value(iPoint >= 0 && iPoint < m_nPoints ? m_Points[iPoint] : NULL, 0) );				}
	double							Get_Y				(int iPoint)				const		{	return( _Get_Field_Value(iPoint >= 0 && iPoint < m_nPoints ? m_Points[iPoint] : NULL, 1) );				}
	double							Get_Z				(int iPoint)				const		{	return( _Get_Field_Value(iPoint >= 0 && iPoint < m_nPoints ? m_Points[iPoint] : NULL, 2) );				}
	bool							Set_Attribute		(int iPoint, int iField, double Value)	{	return( Set_Value(iPoint, iField + 3, Value) );				}
	double							Get_Attribute		(int iPoint, int iField)	const		{	return( Get_Value(iPoint, iField + 3) );					}
	bool							Set_NoData			(int iPoint, int iField)				{	return( Set_Attribute(iPoint, iField, m_NoData_Value) );	}
	bool							is_NoData			(int iPoint, int iField)	const		{	return( Get_Attribute(iPoint, iField) == m_NoData_Value );	}

	TSG_Point_3D					Get_Point			(void)			const;
	TSG_Point_3D					Get_Point			(int iPoint)	const;

	double							Get_NoData_Value	(void)			const		{	return( m_NoData_Value );	}
	bool							Set_NoData_Value	(double NoData_Value);

	virtual void					Set_Modified		(bool bModified = true)	{	CSG_Data_Object::Set_Modified(bModified);	}


protected:

	bool							m_bXYZPrecDbl;

	char							**m_Points, *m_Cursor;

	int								m_nFields, m_nPoints, m_nBuffer, m_nPointBytes, *m_Field_Offset;

	double							m_NoData_Value;

	TSG_PointCloud_Field_Type		*m_Field_Type;

	CSG_Simple_Statistics			**m_Field_Stats;

	CSG_String						**m_Field_Name;

	CSG_Rect						m_Extent;


	virtual void					_On_Construction	(void);

	bool							_Load				(const CSG_String &File_Name);
	bool							_Save				(const CSG_String &File_Name);

	bool							_Add_Field			(const SG_Char *Name, TSG_PointCloud_Field_Type Type);
	bool							_Set_Field_Value	(char *pPoint, int iField, double Value);
	double							_Get_Field_Value	(char *pPoint, int iField)	const;
	int								_Get_Field_Bytes	(TSG_PointCloud_Field_Type Type);

	bool							_Inc_Array			(void);
	bool							_Dec_Array			(void);

	bool							_Stats_Invalidate	(void)			const;
	bool							_Stats_Invalidate	(int iField)	const;
	bool							_Stats_Update		(int iField)	const;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** Safe Point Cloud construction */
SAGA_API_DLL_EXPORT CSG_PointCloud *	SG_Create_PointCloud	(void);

/** Safe Point Cloud construction */
SAGA_API_DLL_EXPORT CSG_PointCloud *	SG_Create_PointCloud	(const CSG_PointCloud &PointCloud);

/** Safe Point Cloud construction */
SAGA_API_DLL_EXPORT CSG_PointCloud *	SG_Create_PointCloud	(const CSG_String &File_Name);

/** Safe Point Cloud construction */
SAGA_API_DLL_EXPORT CSG_PointCloud *	SG_Create_PointCloud	(CSG_PointCloud *pStructure);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__pointcloud_H

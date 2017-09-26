/**********************************************************
 * Version $Id$
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
//                       grids.h                         //
//                                                       //
//                 Copyright (C) 2017 by                 //
//                      Olaf Conrad                      //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__SAGA_API__grids_H
#define HEADER_INCLUDED__SAGA_API__grids_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "grid.h"


///////////////////////////////////////////////////////////
//														 //
//						CSG_Grids						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Grids_File_Format
{
	GRIDS_FILE_FORMAT_Undefined	= 0,
	GRIDS_FILE_FORMAT_Normal,
	GRIDS_FILE_FORMAT_Compressed
}
TSG_Grids_File_Format;

//---------------------------------------------------------
#define SG_GRIDS_NAME_OWNER	0x01
#define SG_GRIDS_NAME_INDEX	0x02
#define SG_GRIDS_NAME_VALUE	0x04
#define SG_GRIDS_NAME_GRID	0x08


///////////////////////////////////////////////////////////
//														 //
//						CSG_Grids						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * CSG_Grids is the data object created for handling
  * raster collections.
*/
//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Grids : public CSG_Data_Object
{
//---------------------------------------------------------
public:		///////////////////////////////////////////////

	//-----------------------------------------------------
	CSG_Grids(void);
	virtual ~CSG_Grids(void);

									CSG_Grids			(const CSG_Grids &Grids);
	virtual bool					Create				(const CSG_Grids &Grids);

									CSG_Grids			(const CSG_Grids *pGrids, bool bCopyData = false);
	virtual bool					Create				(const CSG_Grids *pGrids, bool bCopyData = false);

									CSG_Grids			(const CSG_String &FileName, bool bLoadData = true);
	virtual bool					Create				(const CSG_String &FileName, bool bLoadData = true);

									CSG_Grids			(const CSG_Grid_System &System, int NZ = 0, double zMin = 0.0, TSG_Data_Type Type = SG_DATATYPE_Undefined);
	virtual bool					Create				(const CSG_Grid_System &System, int NZ = 0, double zMin = 0.0, TSG_Data_Type Type = SG_DATATYPE_Undefined);

									CSG_Grids			(const CSG_Grid_System &System, const CSG_Table &Attributes, int zAttribute = 0, TSG_Data_Type Type = SG_DATATYPE_Undefined, bool bCreateGrids = false);
	virtual bool					Create				(const CSG_Grid_System &System, const CSG_Table &Attributes, int zAttribute = 0, TSG_Data_Type Type = SG_DATATYPE_Undefined, bool bCreateGrids = false);

									CSG_Grids			(int NX, int NY, int NZ = 0, double Cellsize = 0.0, double xMin = 0.0, double yMin = 0.0, double zMin = 0.0, TSG_Data_Type Type = SG_DATATYPE_Undefined);
	virtual bool					Create				(int NX, int NY, int NZ = 0, double Cellsize = 0.0, double xMin = 0.0, double yMin = 0.0, double zMin = 0.0, TSG_Data_Type Type = SG_DATATYPE_Undefined);

	virtual bool					Destroy				(void);


	//-----------------------------------------------------
	virtual bool					Load				(const CSG_String &FileName, bool bLoadData = true);
	virtual bool					Save				(const CSG_String &FileName, int Format = GRIDS_FILE_FORMAT_Undefined);


	//-----------------------------------------------------
	/** Data object type information.
	*/
	virtual TSG_Data_Object_Type	Get_ObjectType		(void)	const	{	return( SG_DATAOBJECT_TYPE_Grids );	}


	//-----------------------------------------------------
	// Data-Info...

	TSG_Data_Type					Get_Type			(void)	const	{	return( m_pGrids[0]->Get_Type() );			}

	int								Get_nValueBytes		(void)	const	{	return( m_pGrids[0]->Get_nValueBytes() );	}
	int								Get_nLineBytes		(void)	const	{	return( m_pGrids[0]->Get_nLineBytes () );	}

	void							Set_Unit			(const CSG_String &Unit);
	const SG_Char *					Get_Unit			(void)	const	{	return( m_pGrids[0]->Get_Unit() );			}


	//-----------------------------------------------------
	// Georeference...

	const CSG_Grid_System &			Get_System			(void)	const	{	return( m_pGrids[0]->Get_System() );		}

	int								Get_NX				(void)	const	{	return( m_pGrids[0]->Get_System().Get_NX() );	}
	int								Get_NY				(void)	const	{	return( m_pGrids[0]->Get_System().Get_NY() );	}
	int								Get_NZ				(void)	const	{	return( m_Attributes.Get_Count()           );	}
	sLong							Get_NCells			(void)	const	{	return( Get_System().Get_NCells() * Get_NZ() );	}
	sLong							Get_Data_Count		(void);
	sLong							Get_NoData_Count	(void);

	double							Get_Cellsize		(void)	const	{	return( m_pGrids[0]->Get_System().Get_Cellsize() );	}
	double							Get_Cellarea		(void)	const	{	return( m_pGrids[0]->Get_System().Get_Cellarea() );	}

	const CSG_Rect &				Get_Extent			(bool bCells = false)	const	{	return( m_pGrids[0]->Get_System().Get_Extent(bCells) );	}

	double							Get_XMin			(bool bCells = false)	const	{	return( m_pGrids[0]->Get_System().Get_XMin  (bCells) );	}
	double							Get_XMax			(bool bCells = false)	const	{	return( m_pGrids[0]->Get_System().Get_XMax  (bCells) );	}
	double							Get_XRange			(bool bCells = false)	const	{	return( m_pGrids[0]->Get_System().Get_XRange(bCells) );	}

	double							Get_YMin			(bool bCells = false)	const	{	return( m_pGrids[0]->Get_System().Get_YMin  (bCells) );	}
	double							Get_YMax			(bool bCells = false)	const	{	return( m_pGrids[0]->Get_System().Get_YMax  (bCells) );	}
	double							Get_YRange			(bool bCells = false)	const	{	return( m_pGrids[0]->Get_System().Get_YRange(bCells) );	}

	double							Get_ZMin			(bool bCells = false)	const	{	return( m_Attributes.Get_Minimum(m_Z_Attribute) );	}
	double							Get_ZMax			(bool bCells = false)	const	{	return( m_Attributes.Get_Maximum(m_Z_Attribute) );	}
	double							Get_ZRange			(bool bCells = false)	const	{	return( m_Attributes.Get_Range  (m_Z_Attribute) );	}


	//-----------------------------------------------------
	// Attributes...

	bool							Add_Attribute		(const CSG_String &Name, TSG_Data_Type Type, int iField = -1);
	bool							Del_Attribute		(int iField);

	const CSG_Table &				Get_Attributes		(void)	const	{	return(  m_Attributes );	}
	CSG_Table *						Get_Attributes_Ptr	(void)			{	return( &m_Attributes );	}

	CSG_Table_Record &				Get_Attributes		(int i)	const	{	return( m_Attributes[i] );	}

	bool							Set_Z_Attribute		(int i);
	int								Get_Z_Attribute		(void)	const	{	return( m_Z_Attribute );	}

	bool							Set_Z_Name_Field	(int i);
	int								Get_Z_Name_Field	(void)	const;

	bool							Set_Z				(int i, double Value);
	double							Get_Z				(int i)	const	{	return( m_Attributes[i].asDouble(m_Z_Attribute) );	}

	bool							Update_Z_Order		(void);


	//-----------------------------------------------------
	// Grids...

	bool							Set_Grid_Count		(int Count);
	int								Get_Grid_Count		(void)	const	{	return( m_Attributes.Get_Count() );	}

	bool							Add_Grid			(double                     Z);
	bool							Add_Grid			(double                     Z, CSG_Grid *pGrid, bool bAttach = false);
	bool							Add_Grid			(CSG_Table_Record &Attributes);
	bool							Add_Grid			(CSG_Table_Record &Attributes, CSG_Grid *pGrid, bool bAttach = false);

	bool							Del_Grid			(int i, bool bDetach = false);
	bool							Del_Grids			(       bool bDetach = false);

	const CSG_Grid &				Get_Grid			(int i)	const	{	return( *m_pGrids[i] );	}
	CSG_Grid *						Get_Grid_Ptr		(int i)	const	{	return(  m_pGrids[i] );	}
	CSG_String						Get_Grid_Name		(int i, int Style = 0)	const;

	sLong							Get_Memory_Size		(void)	const	{	return( m_pGrids[0]->Get_Memory_Size() * Get_NZ() );	}


	//-----------------------------------------------------
	// Values...

	void							Set_Scaling			(double Scale = 1.0, double Offset = 0.0);
	double							Get_Scaling			(void)	const	{	return( m_pGrids[0]->Get_Scaling() );	}
	double							Get_Offset			(void)	const	{	return( m_pGrids[0]->Get_Offset () );	}
	bool							is_Scaled			(void)	const	{	return( m_pGrids[0]->is_Scaled  () );	}

	double							Get_Mean			(void);
	double							Get_Min				(void);
	double							Get_Max				(void);
	double							Get_Range			(void);
	double							Get_StdDev			(void);
	double							Get_Variance		(void);
	double							Get_Quantile		(double Quantile);

	const CSG_Simple_Statistics &	Get_Statistics		(void);
	bool							Get_Statistics		(const CSG_Rect &rWorld, CSG_Simple_Statistics &Statistics, bool bHoldValues = false)	const;

	virtual bool					Set_Max_Samples		(sLong Max_Samples);


	//-----------------------------------------------------
	// Checks...

	virtual bool					is_Valid			(void)	const;

	TSG_Intersection				is_Intersecting		(const CSG_Rect &Extent) const	{	return( m_pGrids[0]->is_Intersecting(Extent) );	}
	TSG_Intersection				is_Intersecting		(const TSG_Rect &Extent) const	{	return( m_pGrids[0]->is_Intersecting(Extent) );	}
	TSG_Intersection				is_Intersecting		(double xMin, double yMin, double xMax, double yMax) const	{	return( m_pGrids[0]->is_Intersecting(xMin, yMin, xMax, yMax) );	}

	bool							is_Compatible		(CSG_Grid  *pGrid ) const;
	bool							is_Compatible		(CSG_Grids *pGrids) const;
	bool							is_Compatible		(const CSG_Grid_System &System) const;
	bool							is_Compatible		(int NX, int NY, double Cellsize, double xMin, double yMin) const;

	bool							is_InGrid			(int    x, int    y, int    z, bool bCheckNoData = true)	const	{	return( Get_System().is_InGrid(x, y) && z >= 0 && z < Get_NZ() && (!bCheckNoData || !is_NoData(x, y, z)) );	}
	bool							is_InGrid_byPos		(double x, double y, double z, bool bCheckNoData = true)	const	{	return( Get_Extent(true).Contains(x, y) && z >= Get_ZMin() && z <= Get_ZMax() && (!bCheckNoData || !is_NoData(Get_System().Get_xWorld_to_Grid(x), Get_System().Get_yWorld_to_Grid(y), (int)z)) );	}
	bool							is_InGrid_byPos		(const TSG_Point_Z         &p, bool bCheckNoData = true)	const	{	return( is_InGrid_byPos(p.x, p.y, p.z, bCheckNoData) );	}


	//-----------------------------------------------------
	// Set update flag when modified...

	virtual void					Set_Modified	(bool bModified = true)
	{
		CSG_Data_Object::Set_Modified(bModified);

		m_Attributes.Set_Modified(bModified);

		if( bModified )
		{
			Set_Update_Flag();
		}
	}

	virtual bool					is_Modified		(void)	const
	{
		return( CSG_Data_Object::is_Modified() || m_Attributes.is_Modified() );
	}


	//-----------------------------------------------------
	// Operations...

	void							Assign_NoData				(void);

	virtual bool					Assign						(double Value = 0.0);
	virtual bool					Assign						(CSG_Data_Object *pObject);
	virtual bool					Assign						(CSG_Grids *pGrids, TSG_Grid_Resampling Interpolation);


	//-----------------------------------------------------
	// Operators...

	virtual CSG_Grids &				operator  =		(const CSG_Grids &Grids);
	virtual CSG_Grids &				operator  =		(double Value          );

	virtual CSG_Grids &				operator +=		(double Value          );
	virtual CSG_Grids &				Add				(double Value          );

	virtual CSG_Grids &				operator -=		(double Value          );
	virtual CSG_Grids &				Subtract		(double Value          );

	virtual CSG_Grids &				operator *=		(double Value          );
	virtual CSG_Grids &				Multiply		(double Value          );

	virtual CSG_Grids &				operator /=		(double Value          );
	virtual CSG_Grids &				Divide			(double Value          );

	virtual double					operator ()		(int x, int y, int z) const	{	return( asDouble(x, y, z) );	}


	//-----------------------------------------------------
	// No Data Value...

	virtual bool					Set_NoData_Value_Range	(double loValue, double hiValue);

	virtual bool					is_NoData	(int x, int y, int z)	const	{	return( is_NoData_Value(asDouble(x, y, z, false)) );	}
	virtual bool					is_NoData	(sLong             i)	const	{	return( is_NoData_Value(asDouble(      i, false)) );	}

	virtual void					Set_NoData	(int x, int y, int z)	{	Set_Value(x, y, z, Get_NoData_Value(), false);	}
	virtual void					Set_NoData	(sLong             i)	{	Set_Value(      i, Get_NoData_Value(), false);	}


	//-----------------------------------------------------
	// Get Value...

	double							Get_Value	(double x, double y, double z,                TSG_Grid_Resampling Resampling = GRID_RESAMPLING_BSpline) const;
	double							Get_Value	(const TSG_Point_Z         &p,                TSG_Grid_Resampling Resampling = GRID_RESAMPLING_BSpline) const;
	bool							Get_Value	(double x, double y, double z, double &Value, TSG_Grid_Resampling Resampling = GRID_RESAMPLING_BSpline) const;
	bool							Get_Value	(const TSG_Point_Z         &p, double &Value, TSG_Grid_Resampling Resampling = GRID_RESAMPLING_BSpline) const;

	virtual BYTE					asByte		(int x, int y, int z, bool bScaled = true) const	{	return( SG_ROUND_TO_BYTE (asDouble(x, y, z, bScaled)) );	}
	virtual BYTE					asByte		(sLong             i, bool bScaled = true) const	{	return( SG_ROUND_TO_BYTE (asDouble(      i, bScaled)) );	}
	virtual char					asChar		(int x, int y, int z, bool bScaled = true) const	{	return( SG_ROUND_TO_CHAR (asDouble(x, y, z, bScaled)) );	}
	virtual char					asChar		(sLong             i, bool bScaled = true) const	{	return( SG_ROUND_TO_CHAR (asDouble(      i, bScaled)) );	}
	virtual short					asShort		(int x, int y, int z, bool bScaled = true) const	{	return( SG_ROUND_TO_SHORT(asDouble(x, y, z, bScaled)) );	}
	virtual short					asShort		(sLong             i, bool bScaled = true) const	{	return( SG_ROUND_TO_SHORT(asDouble(      i, bScaled)) );	}
	virtual int						asInt		(int x, int y, int z, bool bScaled = true) const	{	return( SG_ROUND_TO_INT  (asDouble(x, y, z, bScaled)) );	}
	virtual int						asInt		(sLong             i, bool bScaled = true) const	{	return( SG_ROUND_TO_INT  (asDouble(      i, bScaled)) );	}
	virtual sLong					asLong		(int x, int y, int z, bool bScaled = true) const	{	return( SG_ROUND_TO_SLONG(asDouble(x, y, z, bScaled)) );	}
	virtual sLong					asLong		(sLong             i, bool bScaled = true) const	{	return( SG_ROUND_TO_SLONG(asDouble(      i, bScaled)) );	}
	virtual float					asFloat		(int x, int y, int z, bool bScaled = true) const	{	return( (float)          (asDouble(x, y, z, bScaled)) );	}
	virtual float					asFloat		(sLong             i, bool bScaled = true) const	{	return( (float)          (asDouble(      i, bScaled)) );	}

	//-----------------------------------------------------
	virtual double					asDouble(sLong             i, bool bScaled = true) const
	{
		int	z	= (int)(i / m_pGrids[0]->Get_NCells());

		return( m_pGrids[z]->asDouble((sLong)(i % m_pGrids[0]->Get_NCells()), bScaled) );
	}

	virtual double					asDouble(int x, int y, int z, bool bScaled = true) const
	{
		return( m_pGrids[z]->asDouble(x, y, bScaled) );
	}


	//-----------------------------------------------------
	// Set Value...

	virtual void					Add_Value(int x, int y, int z, double Value)	{	Set_Value(x, y, z, asDouble(x, y, z) + Value );	}
	virtual void					Add_Value(sLong             i, double Value)	{	Set_Value(      i, asDouble(      i) + Value );	}

	virtual void					Mul_Value(int x, int y, int z, double Value)	{	Set_Value(x, y, z, asDouble(x, y, z) * Value );	}
	virtual void					Mul_Value(sLong             i, double Value)	{	Set_Value(      i, asDouble(      i) * Value );	}

	//-----------------------------------------------------
	virtual void					Set_Value(sLong             i, double Value, bool bScaled = true)
	{
		int	z	= (int)(i / m_pGrids[0]->Get_NCells());

		m_pGrids[z]->Set_Value((sLong)(i % m_pGrids[0]->Get_NCells()), Value, bScaled);
	}

	virtual void					Set_Value(int x, int y, int z, double Value, bool bScaled = true)
	{
		m_pGrids[z]->Set_Value(x, y, Value, bScaled);
	}


	//-----------------------------------------------------
	// Index...

	bool							Set_Index		(bool bOn = true)
	{
		if( !bOn )
		{
			SG_FREE_SAFE(m_Index);

			return( true );
		}

		return( m_Index || _Set_Index() );
	}

	sLong							Get_Sorted		(sLong Position, bool bDown = true, bool bCheckNoData = true)
	{
		if( Position >= 0 && Position < Get_NCells() && (m_Index || _Set_Index()) )
		{
			Position	= m_Index[bDown ? Get_NCells() - Position - 1 : Position];

			if( !bCheckNoData || !is_NoData(Position) )
			{
				return( Position );
			}
		}

		return( -1 );
	}

	bool							Get_Sorted		(sLong Position, sLong &i, bool bDown = true, bool bCheckNoData = true)
	{
		return( (i = Get_Sorted(Position, bDown, false)) >= 0 && (!bCheckNoData || !is_NoData(i)) );
	}

	bool							Get_Sorted		(sLong Position, int &x, int &y, int &z, bool bDown = true, bool bCheckNoData = true)
	{
		if( (Position = Get_Sorted(Position, bDown, false)) >= 0 )
		{
			z	= (int)(Position / m_pGrids[0]->Get_NCells());

			Position	= Position % m_pGrids[0]->Get_NCells();

			x	= (int)(Position % Get_NX());
			y	= (int)(Position / Get_NX());

			return( !bCheckNoData || !is_NoData(x, y, z) );
		}

		return( false );
	}


//---------------------------------------------------------
protected:	///////////////////////////////////////////////

	virtual bool					On_Update				(void);
	virtual bool					On_Reload				(void);
	virtual bool					On_Delete				(void);


//---------------------------------------------------------
private:	///////////////////////////////////////////////

	int								m_Z_Attribute, m_Z_Name;

	sLong							*m_Index;

	CSG_Table						m_Attributes;

	CSG_Array_Pointer				m_Grids;

	CSG_Grid						**m_pGrids;

	CSG_Simple_Statistics			m_Statistics;


	//-----------------------------------------------------
	void							_On_Construction		(void);

	void							_Synchronize			(CSG_Grid *pGrid);

	//-----------------------------------------------------
	bool							_Get_Z					(double Value, int &iz, double &dz)	const;

	//-----------------------------------------------------
	bool							_Set_Index				(void);

	//-----------------------------------------------------
	bool							_Load_External			(const CSG_String &FileName);
	bool							_Load_PGSQL				(const CSG_String &FileName);

	bool							_Load_Normal			(const CSG_String &FileName);
	bool							_Save_Normal			(const CSG_String &FileName);

	bool							_Load_Compressed		(const CSG_String &FileName);
	bool							_Save_Compressed		(const CSG_String &FileName);

	bool							_Load_Header			(CSG_File &Stream);
	bool							_Save_Header			(CSG_File &Stream);

	bool							_Load_Attributes		(CSG_File &Stream);
	bool							_Save_Attributes		(CSG_File &Stream);

	bool							_Load_Data				(CSG_File &Stream, CSG_Grid *pGrid);
	bool							_Save_Data				(CSG_File &Stream, CSG_Grid *pGrid);

	//-----------------------------------------------------
	bool							_Assign_Interpolated	(CSG_Grids *pSource, TSG_Grid_Resampling Interpolation);
	bool							_Assign_MeanValue		(CSG_Grids *pSource, bool bAreaProportional);
	bool							_Assign_ExtremeValue	(CSG_Grids *pSource, bool bMaximum);
	bool							_Assign_Majority		(CSG_Grids *pSource);

	//-----------------------------------------------------
	CSG_Grids &						_Operation_Arithmetic	(const CSG_Grids &Grids, TSG_Grid_Operation Operation);
	CSG_Grids &						_Operation_Arithmetic	(double Value          , TSG_Grid_Operation Operation);

};


///////////////////////////////////////////////////////////
//														 //
//						Functions						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_GRIDS_PTR_SAFE_SET_NODATA(g, x, y, z)	{ if( g && g->is_InGrid(x, y, z, false) ) { g->Set_NoData(x, y, z   ); } }
#define SG_GRIDS_PTR_SAFE_SET_VALUE(g, x, y, z, v)	{ if( g && g->is_InGrid(x, y, z, false) ) { g->Set_Value (x, y, z, v); } }

//---------------------------------------------------------
/** Safe construction of a grid collection */
SAGA_API_DLL_EXPORT CSG_Grids *		SG_Create_Grids		(void);

/** Safe construction of a grid collection */
SAGA_API_DLL_EXPORT CSG_Grids *		SG_Create_Grids		(const CSG_Grids &Grids);

/** Safe construction of a grid collection */
SAGA_API_DLL_EXPORT CSG_Grids *		SG_Create_Grids		(const CSG_Grids *pGrids, bool bCopyData = false);

/** Safe construction of a grid collection */
SAGA_API_DLL_EXPORT CSG_Grids *		SG_Create_Grids		(const CSG_String &FileName, bool bLoadData = true);

/** Safe construction of a grid collection */
SAGA_API_DLL_EXPORT CSG_Grids *		SG_Create_Grids		(const CSG_Grid_System &System, int NZ = 0, double zMin = 0.0, TSG_Data_Type Type = SG_DATATYPE_Undefined);

/** Safe construction of a grid collection */
SAGA_API_DLL_EXPORT CSG_Grids *		SG_Create_Grids		(const CSG_Grid_System &System, const CSG_Table &Attributes, int zAttribute = 0, TSG_Data_Type Type = SG_DATATYPE_Undefined, bool bCreateGrids = false);

/** Safe construction of a grid collection */
SAGA_API_DLL_EXPORT CSG_Grids *		SG_Create_Grids		(int NX, int NY, int NZ = 0, double Cellsize = 0.0, double xMin = 0.0, double yMin = 0.0, double zMin = 0.0, TSG_Data_Type Type = SG_DATATYPE_Undefined);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__grids_H

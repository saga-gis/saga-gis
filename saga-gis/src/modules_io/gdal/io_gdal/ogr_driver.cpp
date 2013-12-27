/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                                                       //
//                       io_gdal                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    ogr_driver.cpp                     //
//                                                       //
//            Copyright (C) 2008 by O. Conrad            //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Bundesstr. 55                          //
//                D-20146 Hamburg                        //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "ogr_driver.h"

#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <ogr_core.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_OGR_Drivers		gSG_OGR_Drivers;

const CSG_OGR_Drivers &	SG_Get_OGR_Drivers	(void)
{
	return( gSG_OGR_Drivers );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_OGR_Drivers::CSG_OGR_Drivers(void)
{
	OGRRegisterAll();

	m_pDrivers	= OGRSFDriverRegistrar::GetRegistrar();
}

//---------------------------------------------------------
CSG_OGR_Drivers::~CSG_OGR_Drivers(void)
{
//	OGRCleanupAll();	
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_OGR_Drivers::Get_Count(void) const
{
	return( m_pDrivers->GetDriverCount() );
}

//---------------------------------------------------------
OGRSFDriver * CSG_OGR_Drivers::Get_Driver(int Index) const
{
	return( m_pDrivers->GetDriver(Index) );
}

//---------------------------------------------------------
OGRSFDriver * CSG_OGR_Drivers::Get_Driver(const CSG_String &Name) const
{
	return( m_pDrivers ? m_pDrivers->GetDriverByName(Name) : NULL );
}

//---------------------------------------------------------
CSG_String CSG_OGR_Drivers::Get_Name(int Index) const
{
	return( m_pDrivers->GetDriver(Index)->GetName() );
}

//---------------------------------------------------------
CSG_String CSG_OGR_Drivers::Get_Description(int Index) const
{
	OGRSFDriver	*pDriver	= m_pDrivers->GetDriver(Index);
	CSG_String	s;

	s	+= pDriver->TestCapability(ODrCCreateDataSource)	? SG_T("\n[x] ") : SG_T("\n[ ] ");
	s	+= _TL("create data source");

	s	+= pDriver->TestCapability(ODrCDeleteDataSource)	? SG_T("\n[x] ") : SG_T("\n[ ] ");
	s	+= _TL("delete data source");

/*	s	+= pDriver->TestCapability(ODsCCreateLayer)			? SG_T("\n[x]") : SG_T("\n[ ]");
	s	+= _TL("create layer");

	s	+= pDriver->TestCapability(ODsCDeleteLayer)			? SG_T("\n[x]") : SG_T("\n[ ]");
	s	+= _TL("delete layer");

	s	+= pDriver->TestCapability(OLCDeleteFeature)		? SG_T("\n[x]") : SG_T("\n[ ]");
	s	+= _TL("delete feature");

	s	+= pDriver->TestCapability(OLCRandomRead)			? SG_T("\n[x]") : SG_T("\n[ ]");
	s	+= _TL("random read");

	s	+= pDriver->TestCapability(OLCRandomWrite)			? SG_T("\n[x]") : SG_T("\n[ ]");
	s	+= _TL("random write");

	s	+= pDriver->TestCapability(OLCSequentialWrite)		? SG_T("\n[x]") : SG_T("\n[ ]");
	s	+= _TL("sequential write");
/**/

	return( s );
}

//---------------------------------------------------------
bool CSG_OGR_Drivers::Can_Read(int Index) const
{
	return( Get_Driver(Index) != NULL );
}

//---------------------------------------------------------
bool CSG_OGR_Drivers::Can_Write(int Index) const
{
	return( Get_Driver(Index) != NULL );//&& Get_Driver(Index)->TestCapability(ODrCCreateDataSource) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Vertex_Type CSG_OGR_Drivers::Get_Vertex_Type(int Type)
{
	switch( Type )
	{
	case wkbPoint25D:				// 2.5D extension as per 99-402
	case wkbMultiPoint25D:			// 2.5D extension as per 99-402
	case wkbLineString25D:			// 2.5D extension as per 99-402
	case wkbMultiLineString25D:		// 2.5D extension as per 99-402
	case wkbPolygon25D:				// 2.5D extension as per 99-402
	case wkbMultiPolygon25D:		// 2.5D extension as per 99-402
	case wkbGeometryCollection25D:	// 2.5D extension as per 99-402 
		return( SG_VERTEX_TYPE_XYZ );

	default:
		return( SG_VERTEX_TYPE_XY );
	}
}

//---------------------------------------------------------
TSG_Shape_Type CSG_OGR_Drivers::Get_Shape_Type(int Type)
{
	switch( Type )
	{
	case wkbPoint:					// 0-dimensional geometric object, standard WKB
	case wkbPoint25D:				// 2.5D extension as per 99-402
		return( SHAPE_TYPE_Point );

	case wkbMultiPoint:				// GeometryCollection of Points, standard WKB
	case wkbMultiPoint25D:			// 2.5D extension as per 99-402
		return( SHAPE_TYPE_Points );

	case wkbLineString:				// 1-dimensional geometric object with linear interpolation between Points, standard WKB
	case wkbMultiLineString:		// GeometryCollection of LineStrings, standard WKB
	case wkbLineString25D:			// 2.5D extension as per 99-402
	case wkbMultiLineString25D:		// 2.5D extension as per 99-402
		return( SHAPE_TYPE_Line );

	case wkbPolygon:				// planar 2-dimensional geometric object defined by 1 exterior boundary and 0 or more interior boundaries, standard WKB
	case wkbMultiPolygon:			// GeometryCollection of Polygons, standard WKB
	case wkbPolygon25D:				// 2.5D extension as per 99-402
	case wkbMultiPolygon25D:		// 2.5D extension as per 99-402
		return( SHAPE_TYPE_Polygon );

	default:
	case wkbUnknown:				// unknown type, non-standard
	case wkbNone:					// non-standard, for pure attribute records
	case wkbLinearRing:				// non-standard, just for createGeometry()
	case wkbGeometryCollection:		// geometric object that is a collection of 1 or more geometric objects, standard WKB
	case wkbGeometryCollection25D:	// 2.5D extension as per 99-402 
		return( SHAPE_TYPE_Undefined );
	}
}

//---------------------------------------------------------
int CSG_OGR_Drivers::Get_Shape_Type(TSG_Shape_Type Type, bool bZ)
{
	switch( Type )
	{
	case SHAPE_TYPE_Point:	 	return( bZ ? wkbPoint25D           : wkbPoint           );	// point
	case SHAPE_TYPE_Points: 	return( bZ ? wkbMultiPoint25D      : wkbMultiPoint      );	// points
	case SHAPE_TYPE_Line: 		return( bZ ? wkbMultiLineString25D : wkbMultiLineString );	// line
	case SHAPE_TYPE_Polygon:	return( bZ ? wkbMultiPolygon25D    : wkbMultiPolygon    );	// polygon

	default:	return( wkbUnknown );
	}
}

//---------------------------------------------------------
TSG_Data_Type CSG_OGR_Drivers::Get_Data_Type(int Type)
{
	switch( Type )
	{
	case OFTInteger:		// Simple 32bit integer
		return( SG_DATATYPE_Int );

	case OFTReal:			// Double Precision floating point
		return( SG_DATATYPE_Double );

	case OFTString:			// String of ASCII chars
		return( SG_DATATYPE_String );

	case OFTDateTime:		// Date and Time 
		return( SG_DATATYPE_Date );

	default:
	case OFTIntegerList:	// List of 32bit integers
	case OFTRealList:		// List of doubles
	case OFTStringList:		// Array of strings
	case OFTWideString:		// deprecated
	case OFTWideStringList:	// deprecated
	case OFTBinary:			// Raw Binary data
	case OFTDate:			// Date
	case OFTTime:			// Time
		return( SG_DATATYPE_Undefined );
	}
}

//---------------------------------------------------------
int CSG_OGR_Drivers::Get_Data_Type(TSG_Data_Type Type)
{
	switch( Type )
	{
	default:
	case SG_DATATYPE_String:
	case SG_DATATYPE_Date:
		return( OFTString );

	case SG_DATATYPE_Byte:
	case SG_DATATYPE_Char:
	case SG_DATATYPE_Word:
	case SG_DATATYPE_Short:
	case SG_DATATYPE_DWord:
	case SG_DATATYPE_Int:
	case SG_DATATYPE_ULong:
	case SG_DATATYPE_Long:
	case SG_DATATYPE_Color:
		return( OFTInteger );

	case SG_DATATYPE_Float:
	case SG_DATATYPE_Double:
		return( OFTReal );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_OGR_DataSource::CSG_OGR_DataSource(void)
{
	m_pDataSource	= NULL;
}

CSG_OGR_DataSource::CSG_OGR_DataSource(const CSG_String &File)
{
	m_pDataSource	= NULL;

	Create(File);
}

//---------------------------------------------------------
CSG_OGR_DataSource::~CSG_OGR_DataSource(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_OGR_DataSource::Create(const CSG_String &File)
{
	Destroy();

	m_pDataSource	= OGRSFDriverRegistrar::Open(File);

	return( m_pDataSource != NULL );
}

bool CSG_OGR_DataSource::Create(const CSG_String &File, const CSG_String &DriverName)
{
	OGRSFDriver	*pDriver;

	Destroy();

	if( (pDriver = gSG_OGR_Drivers.Get_Driver(DriverName)) != NULL )
	{
		m_pDataSource	= pDriver->CreateDataSource(File, NULL);
	}

	return( m_pDataSource != NULL );
}

//---------------------------------------------------------
bool CSG_OGR_DataSource::Destroy(void)
{
	if( m_pDataSource )
	{
		OGRDataSource::DestroyDataSource(m_pDataSource);

		m_pDataSource	= NULL;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_OGR_DataSource::Get_Count(void)
{
	if( m_pDataSource )
	{
		return( m_pDataSource->GetLayerCount() );
	}

	return( 0 );
}

//---------------------------------------------------------
OGRLayer * CSG_OGR_DataSource::Get_Layer(int iLayer)
{
	if( m_pDataSource && iLayer >= 0 && iLayer < m_pDataSource->GetLayerCount() )
	{
		return( m_pDataSource->GetLayer(iLayer) );
	}

	return( NULL );
}

//---------------------------------------------------------
TSG_Shape_Type CSG_OGR_DataSource::Get_Type(int iLayer)
{
	if( Get_Layer(iLayer) )
	{
		return( CSG_OGR_Drivers::Get_Shape_Type(Get_Layer(iLayer)->GetLayerDefn()->GetGeomType()) );
	}

	return( SHAPE_TYPE_Undefined );
}

//---------------------------------------------------------
TSG_Vertex_Type CSG_OGR_DataSource::Get_Coordinate_Type(int iLayer)
{
	if( Get_Layer(iLayer) )
	{
		return( CSG_OGR_Drivers::Get_Vertex_Type(Get_Layer(iLayer)->GetLayerDefn()->GetGeomType()) );
	}

	return( SG_VERTEX_TYPE_XY );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shapes * CSG_OGR_DataSource::Read(int iLayer, int iGeomTypeChoice)
{
	OGRLayer	*pLayer	= Get_Layer(iLayer);

	if( iGeomTypeChoice != 0 )
		pLayer->GetLayerDefn()->SetGeomType((OGRwkbGeometryType)_Get_GeomType_Choice(iGeomTypeChoice));

	//-----------------------------------------------------
	if( pLayer && Get_Type(iLayer) != SHAPE_TYPE_Undefined )
	{
		int				iField;
		OGRFeature		*pFeature;
		OGRFeatureDefn	*pDef		= pLayer->GetLayerDefn();
		CSG_Shapes		*pShapes	= SG_Create_Shapes(Get_Type(iLayer), CSG_String(pDef->GetName()), NULL, Get_Coordinate_Type(iLayer));

		for(iField=0; iField<pDef->GetFieldCount(); iField++)
		{
			OGRFieldDefn	*pDefField	= pDef->GetFieldDefn(iField);

			pShapes->Add_Field(pDefField->GetNameRef(), CSG_OGR_Drivers::Get_Data_Type(pDefField->GetType()));
		}

		pLayer->ResetReading();

		//-------------------------------------------------
		while( (pFeature = pLayer->GetNextFeature()) != NULL && SG_UI_Process_Get_Okay(false) )
		{
			OGRGeometry	*pGeometry	= pFeature->GetGeometryRef();

			if( pGeometry != NULL )
			{
				CSG_Shape	*pShape	= pShapes->Add_Shape();

				for(iField=0; iField<pDef->GetFieldCount(); iField++)
				{
					OGRFieldDefn	*pDefField	= pDef->GetFieldDefn(iField);

					switch( pDefField->GetType() )
					{
					default:			pShape->Set_Value(iField, pFeature->GetFieldAsString (iField));	break;
					case OFTString:		pShape->Set_Value(iField, pFeature->GetFieldAsString (iField));	break;
					case OFTInteger:	pShape->Set_Value(iField, pFeature->GetFieldAsInteger(iField));	break;
					case OFTReal:		pShape->Set_Value(iField, pFeature->GetFieldAsDouble (iField));	break;
					}
				}

				//-----------------------------------------
				if( _Read_Geometry(pShape, pGeometry) == false )
				{
					pShapes->Del_Shape(pShape);
				}
			}

			OGRFeature::DestroyFeature(pFeature);
		}

		return( pShapes );
	}

	//-----------------------------------------------------
	return( NULL );
}

//---------------------------------------------------------
int CSG_OGR_DataSource::_Get_GeomType_Choice(int iGeomTypeChoice)
{
	switch( iGeomTypeChoice )
	{
	default:
	case AUTOMATIC:					return( wkbUnknown );
	case WKBPOINT:					return( wkbPoint );
	case WKBPOINT25D:				return( wkbPoint25D );
	case WKBMULTIPOINT:				return( wkbMultiPoint );
	case WKBMULTIPOINT25D:			return( wkbMultiPoint25D );
	case WKBLINESTRING:				return( wkbLineString );
	case WKBLINESTRING25D:			return( wkbLineString25D );
	case WKBMULTILINESTRING:		return( wkbMultiLineString );
	case WKBMULTILINESTRING25D:		return( wkbMultiLineString25D );
	case WKBPOLYGON:				return( wkbPolygon );
	case WKBPOLYGON25D:				return( wkbPolygon25D );
	case WKBMULTIPOLYGON:			return( wkbMultiPolygon );
	case WKBMULTIPOLYGON25D:		return( wkbMultiPolygon25D );
	case WKBGEOMETRYCOLLECTION:		return( wkbGeometryCollection );
	case WKBGEOMETRYCOLLECTION25D:	return( wkbGeometryCollection25D );
	}
}

//---------------------------------------------------------
bool CSG_OGR_DataSource::_Read_Geometry(CSG_Shape *pShape, OGRGeometry *pGeometry)
{
	if( pShape && pGeometry )
	{
		switch( pGeometry->getGeometryType() )
		{
		//-------------------------------------------------
		case wkbPoint:				// 0-dimensional geometric object, standard WKB
		case wkbPoint25D:			// 2.5D extension as per 99-402
			pShape->Add_Point(((OGRPoint *)pGeometry)->getX(), ((OGRPoint *)pGeometry)->getY());
			pShape->Set_Z(((OGRPoint *)pGeometry)->getZ(), 0);
			return( true );

		//-------------------------------------------------
		case wkbLineString:			// 1-dimensional geometric object with linear interpolation between Points, standard WKB
		case wkbLineString25D:		// 2.5D extension as per 99-402
			return( _Read_Line(pShape, (OGRLineString *)pGeometry) );

		//-------------------------------------------------
		case wkbPolygon:			// planar 2-dimensional geometric object defined by 1 exterior boundary and 0 or more interior boundaries, standard WKB
		case wkbPolygon25D:			// 2.5D extension as per 99-402
			return( _Read_Polygon(pShape, (OGRPolygon *)pGeometry) );

		//-------------------------------------------------
		case wkbMultiPoint:			// GeometryCollection of Points, standard WKB
		case wkbMultiPoint25D:		// 2.5D extension as per 99-402
		case wkbMultiLineString:	// GeometryCollection of LineStrings, standard WKB
		case wkbMultiLineString25D:	// 2.5D extension as per 99-402
		case wkbMultiPolygon:		// GeometryCollection of Polygons, standard WKB
		case wkbMultiPolygon25D:	// 2.5D extension as per 99-402
			{
				for(int i=0; i<((OGRGeometryCollection *)pGeometry)->getNumGeometries(); i++)
				{
					if( _Read_Geometry(pShape, ((OGRGeometryCollection *)pGeometry)->getGeometryRef(i)) == false )
					{
						return( false );
					}
				}
			}

			return( true );

		//-------------------------------------------------
		default:
			break;
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_OGR_DataSource::_Read_Line(CSG_Shape *pShape, OGRLineString *pLine)
{
	if( pShape && pLine && pLine->getNumPoints() > 0 )
	{
		int		iPart	= pShape->Get_Part_Count();

		for(int iPoint=0; iPoint<pLine->getNumPoints(); iPoint++)
		{
			pShape->Add_Point(pLine->getX(iPoint), pLine->getY(iPoint), iPart);

			pShape->Set_Z(pLine->getZ(iPoint), iPoint, iPart);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_OGR_DataSource::_Read_Polygon(CSG_Shape *pShape, OGRPolygon *pPolygon)
{
	if( pShape && pPolygon )
	{
		_Read_Line(pShape, pPolygon->getExteriorRing());

		for(int i=0; i<pPolygon->getNumInteriorRings(); i++)
		{
			pPolygon->getInteriorRing(i);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_OGR_DataSource::Write(CSG_Shapes *pShapes, const CSG_String &DriverName)
{
	bool		bZ	= pShapes->Get_Vertex_Type() != SG_VERTEX_TYPE_XY;
	OGRLayer	*pLayer;

	//-----------------------------------------------------
	if( m_pDataSource && pShapes && pShapes->is_Valid() && (pLayer = m_pDataSource->CreateLayer(CSG_String(pShapes->Get_Name()), NULL, (OGRwkbGeometryType)gSG_OGR_Drivers.Get_Shape_Type(pShapes->Get_Type(), bZ))) != NULL )
	{
		bool			bResult	= true;
		int				iField;
		
		//-------------------------------------------------
		if( SG_STR_CMP(DriverName, "DXF") )
		// the dxf driver does not support arbitrary field creation and returns OGRERR_FAILURE;
		// it seems like there is no method in OGR to check whether a driver supports field creation or not;
		// another issue with the dxf driver: 3D polygon data is not supported (would require e.g. "3DFACE" entity implementation in GDAL/OGR),
		// so we would need to treat them as polylines (not implemented, currently it is necessary to convert to a line shapefile a priori)
		{
			for(iField=0; iField<pShapes->Get_Field_Count() && bResult; iField++)
			{
				OGRFieldDefn	DefField(CSG_String(pShapes->Get_Field_Name(iField)), (OGRFieldType)gSG_OGR_Drivers.Get_Data_Type(pShapes->Get_Field_Type(iField)));

				//	DefField.SetWidth(32);

				if( pLayer->CreateField(&DefField) != OGRERR_NONE )
				{
					bResult	= false;
				}
			}
		}

		//-------------------------------------------------
		for(int iShape=0; iShape<pShapes->Get_Count() && bResult && SG_UI_Process_Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape		= pShapes->Get_Shape(iShape);
			OGRFeature	*pFeature	= OGRFeature::CreateFeature(pLayer->GetLayerDefn());

			// no need for a special treatment of DXF here, as pFeature->SetField() just silently ignores iFields out of range
			for(iField=0; iField<pShapes->Get_Field_Count(); iField++)
			{
				switch( pShapes->Get_Field_Type(iField) )
				{
				default:
				case SG_DATATYPE_Char:
				case SG_DATATYPE_String:
				case SG_DATATYPE_Date:
					pFeature->SetField(iField, CSG_String(pShape->asString(iField)));
					break;

				case SG_DATATYPE_Short:
				case SG_DATATYPE_Int:
				case SG_DATATYPE_Long:
				case SG_DATATYPE_Color:
					pFeature->SetField(iField, pShape->asInt(iField));
					break;

				case SG_DATATYPE_Float:
				case SG_DATATYPE_Double:
					pFeature->SetField(iField, pShape->asDouble(iField));
					break;
				}
			}

			if( !_Write_Geometry(pShape, pFeature, bZ) || pLayer->CreateFeature(pFeature) != OGRERR_NONE )
			{
				bResult	= false;
			}

			OGRFeature::DestroyFeature(pFeature);
		}

		//-------------------------------------------------
		return( bResult );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_OGR_DataSource::_Write_Geometry(CSG_Shape *pShape, OGRFeature *pFeature, bool bZ)
{
	if( pShape && pFeature )
	{
		int					iPoint, iPart;
		TSG_Point			sgPoint;
		OGRPoint			Point;
		OGRMultiPoint		Points;
		OGRLineString		Line;
		OGRMultiLineString	Lines;
		OGRLinearRing		Ring;
		OGRPolygon			Polygon;

		switch( pShape->Get_Type() )
		{
		//-------------------------------------------------
		case SHAPE_TYPE_Point:
			sgPoint	= pShape->Get_Point(0);
			Point.setX(sgPoint.x);
			Point.setY(sgPoint.y);

			if( bZ )
			{
				Point.setZ(pShape->Get_Z(0));
			}

			return( pFeature->SetGeometry(&Point) == OGRERR_NONE );

		//-------------------------------------------------
		case SHAPE_TYPE_Points:
			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					sgPoint	= pShape->Get_Point(iPoint, iPart);
					Point.setX(sgPoint.x);
					Point.setY(sgPoint.y);

					if( bZ )
					{
						Point.setZ(pShape->Get_Z(0));
					}

					Points.addGeometry(&Point);
				}
			}

			return( pFeature->SetGeometry(&Points) == OGRERR_NONE );

		//-------------------------------------------------
		case SHAPE_TYPE_Line:
			if( pShape->Get_Part_Count() == 1 )
			{
				_Write_Line(pShape, &Line, 0, bZ);

				return( pFeature->SetGeometry(&Line) == OGRERR_NONE );
			}
			else
			{
				for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
				{
					if( _Write_Line(pShape, &Line, iPart, bZ) )
					{
						Lines.addGeometry(&Line);
					}
				}

				return( pFeature->SetGeometry(&Lines) == OGRERR_NONE );
			}

		//-------------------------------------------------
		case SHAPE_TYPE_Polygon:
			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				if( _Write_Line(pShape, &Ring, iPart, bZ) )
				{
					Polygon.addRing(&Ring);
				}
			}

			return( pFeature->SetGeometry(&Polygon) == OGRERR_NONE );

		//-------------------------------------------------
		default:
			break;
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_OGR_DataSource::_Write_Line(CSG_Shape *pShape, OGRLineString *pLine, int iPart, bool bZ)
{
	if( pLine && pShape && iPart >= 0 && iPart < pShape->Get_Part_Count() )
	{
		pLine->empty();

		for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			TSG_Point	sgPoint	= pShape->Get_Point(iPoint, iPart);

			if( !bZ )
			{
				pLine->addPoint(sgPoint.x, sgPoint.y);
			}
			else
			{
				pLine->addPoint(sgPoint.x, sgPoint.y, pShape->Get_Z(iPoint, iPart));
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

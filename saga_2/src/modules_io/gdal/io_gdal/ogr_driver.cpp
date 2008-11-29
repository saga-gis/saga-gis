
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

#include <ogr_core.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COGR_Driver		g_OGR_Driver;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COGR_Driver::COGR_Driver(void)
{
	OGRRegisterAll();

	m_pManager	= OGRSFDriverRegistrar::GetRegistrar();
}

//---------------------------------------------------------
COGR_Driver::~COGR_Driver(void)
{
//	OGRCleanupAll();	
}

//---------------------------------------------------------
OGRSFDriver * COGR_Driver::Get_Driver(const CSG_String &Name)
{
	return( m_pManager ? m_pManager->GetDriverByName(SG_STR_SGTOMB(Name)) : NULL );
}

//---------------------------------------------------------
bool COGR_Driver::Can_Read(int iDriver)
{
	return( Get_Driver(iDriver) != NULL );
}

bool COGR_Driver::Can_Write(int iDriver)
{
	return( Get_Driver(iDriver) != NULL );//&& Get_Driver(iDriver)->TestCapability(ODrCCreateDataSource) );
}

//---------------------------------------------------------
CSG_String COGR_Driver::Get_Description(int iDriver)
{
	OGRSFDriver	*pDriver	= m_pManager->GetDriver(iDriver);
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

	return( SG_STR_MBTOSG(s) );
}

//---------------------------------------------------------
TSG_Shape_Type COGR_Driver::Get_Type(OGRwkbGeometryType Type)
{
	switch( Type )
	{
	case wkbPoint:					// 0-dimensional geometric object, standard WKB
		return( SHAPE_TYPE_Point );

	case wkbMultiPoint:				// GeometryCollection of Points, standard WKB
		return( SHAPE_TYPE_Points );

	case wkbLineString:				// 1-dimensional geometric object with linear interpolation between Points, standard WKB
	case wkbMultiLineString:		// GeometryCollection of LineStrings, standard WKB
		return( SHAPE_TYPE_Line );

	case wkbPolygon:				// planar 2-dimensional geometric object defined by 1 exterior boundary and 0 or more interior boundaries, standard WKB
	case wkbMultiPolygon:			// GeometryCollection of Polygons, standard WKB
		return( SHAPE_TYPE_Polygon );

	default:
	case wkbUnknown:				// unknown type, non-standard
	case wkbGeometryCollection:		// geometric object that is a collection of 1 or more geometric objects, standard WKB
	case wkbNone:					// non-standard, for pure attribute records
	case wkbLinearRing:				// non-standard, just for createGeometry()
	case wkbPoint25D:				// 2.5D extension as per 99-402
	case wkbLineString25D:			// 2.5D extension as per 99-402
	case wkbPolygon25D:				// 2.5D extension as per 99-402
	case wkbMultiPoint25D:			// 2.5D extension as per 99-402
	case wkbMultiLineString25D:		// 2.5D extension as per 99-402
	case wkbMultiPolygon25D:		// 2.5D extension as per 99-402
	case wkbGeometryCollection25D:	// 2.5D extension as per 99-402 
		return( SHAPE_TYPE_Undefined );
	}
}

//---------------------------------------------------------
OGRwkbGeometryType COGR_Driver::Get_Type(TSG_Shape_Type Type)
{
	switch( Type )
	{
	case SHAPE_TYPE_Point:	 	return( wkbPoint );				// point
	case SHAPE_TYPE_Points: 	return( wkbMultiPoint );		// points
	case SHAPE_TYPE_Line: 		return( wkbMultiLineString );	// line
	case SHAPE_TYPE_Polygon:	return( wkbMultiPolygon );		// polygon

	default:					return( wkbUnknown );
	}
}

//---------------------------------------------------------
TSG_Table_Field_Type COGR_Driver::Get_Type(OGRFieldType Type)
{
	switch( Type )
	{
	case OFTInteger:		// Simple 32bit integer
		return( TABLE_FIELDTYPE_Int );

	case OFTReal:			// Double Precision floating point
		return( TABLE_FIELDTYPE_Double );

	case OFTString:			// String of ASCII chars
		return( TABLE_FIELDTYPE_String );

	case OFTDateTime:		// Date and Time 
		return( TABLE_FIELDTYPE_Date );

	default:
	case OFTIntegerList:	// List of 32bit integers
	case OFTRealList:		// List of doubles
	case OFTStringList:		// Array of strings
	case OFTWideString:		// deprecated
	case OFTWideStringList:	// deprecated
	case OFTBinary:			// Raw Binary data
	case OFTDate:			// Date
	case OFTTime:			// Time
		return( TABLE_FIELDTYPE_None );
	}
}

//---------------------------------------------------------
OGRFieldType COGR_Driver::Get_Type(TSG_Table_Field_Type Type)
{
	switch( Type )
	{
	default:
	case TABLE_FIELDTYPE_Char:
	case TABLE_FIELDTYPE_String:
	case TABLE_FIELDTYPE_Date:
		return( OFTString );

	case TABLE_FIELDTYPE_Short:
	case TABLE_FIELDTYPE_Int:
	case TABLE_FIELDTYPE_Long:
	case TABLE_FIELDTYPE_Color:
		return( OFTInteger );

	case TABLE_FIELDTYPE_Float:
	case TABLE_FIELDTYPE_Double:
		return( OFTReal );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COGR_DataSource::COGR_DataSource(void)
{
	m_pDataSource	= NULL;
}

COGR_DataSource::COGR_DataSource(const CSG_String &File)
{
	m_pDataSource	= NULL;

	Create(File);
}

//---------------------------------------------------------
COGR_DataSource::~COGR_DataSource(void)
{
	Destroy();
}

//---------------------------------------------------------
bool COGR_DataSource::Create(const CSG_String &File)
{
	Destroy();

	m_pDataSource	= OGRSFDriverRegistrar::Open(SG_STR_SGTOMB(File));

	return( m_pDataSource != NULL );
}

bool COGR_DataSource::Create(const CSG_String &File, const CSG_String &DriverName)
{
	OGRSFDriver	*pDriver;

	Destroy();

	if( (pDriver = g_OGR_Driver.Get_Driver(DriverName)) != NULL )
	{
		m_pDataSource	= pDriver->CreateDataSource(SG_STR_SGTOMB(File), NULL);
	}

	return( m_pDataSource != NULL );
}

//---------------------------------------------------------
bool COGR_DataSource::Destroy(void)
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
int COGR_DataSource::Get_Count(void)
{
	if( m_pDataSource )
	{
		return( m_pDataSource->GetLayerCount() );
	}

	return( 0 );
}

//---------------------------------------------------------
OGRLayer * COGR_DataSource::Get_Layer(int iLayer)
{
	if( m_pDataSource && iLayer >= 0 && iLayer < m_pDataSource->GetLayerCount() )
	{
		return( m_pDataSource->GetLayer(iLayer) );
	}

	return( NULL );
}

//---------------------------------------------------------
TSG_Shape_Type COGR_DataSource::Get_Type(int iLayer)
{
	if( m_pDataSource && iLayer >= 0 && iLayer < m_pDataSource->GetLayerCount() )
	{
		return( COGR_Driver::Get_Type(m_pDataSource->GetLayer(iLayer)->GetLayerDefn()->GetGeomType()) );
	}

	return( SHAPE_TYPE_Undefined );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shapes * COGR_DataSource::Read_Shapes(int iLayer)
{
	OGRLayer	*pLayer	= Get_Layer(iLayer);

	//-----------------------------------------------------
	if( pLayer && Get_Type(iLayer) != SHAPE_TYPE_Undefined )
	{
		int				iField;
		OGRFeature		*pFeature;
		OGRFeatureDefn	*pDef		= pLayer->GetLayerDefn();
		CSG_Shapes		*pShapes	= SG_Create_Shapes(Get_Type(iLayer), CSG_String(pDef->GetName()));

		for(iField=0; iField<pDef->GetFieldCount(); iField++)
		{
			OGRFieldDefn	*pDefField	= pDef->GetFieldDefn(iField);

			pShapes->Get_Table().Add_Field(pDefField->GetNameRef(), COGR_Driver::Get_Type(pDefField->GetType()));
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
					default:			pShape->Set_Value(iField, SG_STR_MBTOSG(pFeature->GetFieldAsString (iField)));	break;
					case OFTString:		pShape->Set_Value(iField, SG_STR_MBTOSG(pFeature->GetFieldAsString (iField)));	break;
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
bool COGR_DataSource::_Read_Geometry(CSG_Shape *pShape, OGRGeometry *pGeometry)
{
	if( pShape && pGeometry )
	{
		switch( pGeometry->getGeometryType() )
		{
		//-------------------------------------------------
		case wkbPoint:				// 0-dimensional geometric object, standard WKB
			pShape->Add_Point(((OGRPoint *)pGeometry)->getX(), ((OGRPoint *)pGeometry)->getY());
			return( true );

		//-------------------------------------------------
		case wkbLineString:			// 1-dimensional geometric object with linear interpolation between Points, standard WKB
			return( _Read_Line(pShape, (OGRLineString *)pGeometry) );

		//-------------------------------------------------
		case wkbPolygon:			// planar 2-dimensional geometric object defined by 1 exterior boundary and 0 or more interior boundaries, standard WKB
			return( _Read_Polygon(pShape, (OGRPolygon *)pGeometry) );

		//-------------------------------------------------
		case wkbMultiPoint:			// GeometryCollection of Points, standard WKB
		case wkbMultiLineString:	// GeometryCollection of LineStrings, standard WKB
		case wkbMultiPolygon:		// GeometryCollection of Polygons, standard WKB
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
bool COGR_DataSource::_Read_Line(CSG_Shape *pShape, OGRLineString *pLine)
{
	if( pShape && pLine && pLine->getNumPoints() > 0 )
	{
		int		iPart	= pShape->Get_Part_Count();

		for(int iPoint=0; iPoint<pLine->getNumPoints(); iPoint++)
		{
			pShape->Add_Point(pLine->getX(iPoint), pLine->getY(iPoint), iPart);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool COGR_DataSource::_Read_Polygon(CSG_Shape *pShape, OGRPolygon *pPolygon)
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
bool COGR_DataSource::Write_Shapes(CSG_Shapes *pShapes)
{
	OGRLayer	*pLayer;

	//-----------------------------------------------------
	if( m_pDataSource && pShapes && pShapes->is_Valid() && (pLayer = m_pDataSource->CreateLayer(SG_STR_SGTOMB(pShapes->Get_Name()), NULL, g_OGR_Driver.Get_Type(pShapes->Get_Type()))) != NULL )
	{
		bool			bResult	= true;
		int				iField;

		//-------------------------------------------------
		for(iField=0; iField<pShapes->Get_Table().Get_Field_Count() && bResult; iField++)
		{
			OGRFieldDefn	DefField(SG_STR_SGTOMB(pShapes->Get_Table().Get_Field_Name(iField)), g_OGR_Driver.Get_Type(pShapes->Get_Table().Get_Field_Type(iField)));

			//	DefField.SetWidth(32);

			if( pLayer->CreateField(&DefField) != OGRERR_NONE )
			{
				bResult	= false;
			}
		}

		//-------------------------------------------------
		for(int iShape=0; iShape<pShapes->Get_Count() && bResult && SG_UI_Process_Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape		= pShapes->Get_Shape(iShape);
			OGRFeature	*pFeature	= OGRFeature::CreateFeature(pLayer->GetLayerDefn());

			for(iField=0; iField<pShapes->Get_Table().Get_Field_Count(); iField++)
			{
				switch( pShapes->Get_Table().Get_Field_Type(iField) )
				{
				default:
				case TABLE_FIELDTYPE_Char:
				case TABLE_FIELDTYPE_String:
				case TABLE_FIELDTYPE_Date:
					pFeature->SetField(iField, SG_STR_SGTOMB(pShape->asString(iField)));
					break;

				case TABLE_FIELDTYPE_Short:
				case TABLE_FIELDTYPE_Int:
				case TABLE_FIELDTYPE_Long:
				case TABLE_FIELDTYPE_Color:
					pFeature->SetField(iField, pShape->asInt(iField));
					break;

				case TABLE_FIELDTYPE_Float:
				case TABLE_FIELDTYPE_Double:
					pFeature->SetField(iField, pShape->asDouble(iField));
					break;
				}
			}

			if( !_Write_Geometry(pShape, pFeature) || pLayer->CreateFeature(pFeature) != OGRERR_NONE )
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
bool COGR_DataSource::_Write_Geometry(CSG_Shape *pShape, OGRFeature *pFeature)
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

					Points.addGeometry(&Point);
				}
			}

			return( pFeature->SetGeometry(&Points) == OGRERR_NONE );

		//-------------------------------------------------
		case SHAPE_TYPE_Line:
			if( pShape->Get_Part_Count() == 1 )
			{
				_Write_Line(pShape, &Line, 0);

				return( pFeature->SetGeometry(&Line) == OGRERR_NONE );
			}
			else
			{
				for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
				{
					if( _Write_Line(pShape, &Line, iPart) )
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
				if( _Write_Line(pShape, &Ring, iPart) )
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
bool COGR_DataSource::_Write_Line(CSG_Shape *pShape, OGRLineString *pLine, int iPart)
{
	if( pLine && pShape && iPart >= 0 && iPart < pShape->Get_Part_Count() )
	{
		pLine->empty();

		for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			TSG_Point	sgPoint	= pShape->Get_Point(iPoint, iPart);

			pLine->addPoint(sgPoint.x, sgPoint.y);
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

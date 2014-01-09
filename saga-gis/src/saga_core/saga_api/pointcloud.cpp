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
//                    pointcloud.cpp                     //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include "pointcloud.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define POINTCLOUD_FILE_VERSION	"SGPC01"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_PointCloud * SG_Create_PointCloud(void)
{
	return( new CSG_PointCloud );
}

//---------------------------------------------------------
CSG_PointCloud * SG_Create_PointCloud(const CSG_PointCloud &PointCloud)
{
	return( new CSG_PointCloud(PointCloud) );
}

//---------------------------------------------------------
CSG_PointCloud * SG_Create_PointCloud(const CSG_String &File_Name)
{
	return( new CSG_PointCloud(File_Name) );
}

//---------------------------------------------------------
CSG_PointCloud * SG_Create_PointCloud(CSG_PointCloud *pStructure)
{
	return( new CSG_PointCloud(pStructure) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_PointCloud::CSG_PointCloud(void)
	: CSG_Shapes()
{
	_On_Construction();

	Create();
}

bool CSG_PointCloud::Create(void)
{
	Destroy();

	Add_Field(SG_T(""), SG_DATATYPE_Undefined);	// add x, y, z fields

	return( true );
}

//---------------------------------------------------------
CSG_PointCloud::CSG_PointCloud(const CSG_PointCloud &PointCloud)
	: CSG_Shapes()
{
	_On_Construction();

	Create(PointCloud);
}

bool CSG_PointCloud::Create(const CSG_PointCloud &PointCloud)
{
	if( Assign((CSG_Data_Object *)&PointCloud) )
	{
		Set_Name(PointCloud.Get_Name());

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CSG_PointCloud::CSG_PointCloud(const CSG_String &File_Name)
	: CSG_Shapes()
{
	_On_Construction();

	Create(File_Name);
}

bool CSG_PointCloud::Create(const CSG_String &File_Name)
{
	return( _Load(File_Name) );
}

//---------------------------------------------------------
CSG_PointCloud::CSG_PointCloud(CSG_PointCloud *pStructure)
	: CSG_Shapes()
{
	_On_Construction();

	Create(pStructure);
}

bool CSG_PointCloud::Create(CSG_PointCloud *pStructure)
{
	Destroy();

	if( pStructure && pStructure->Get_Field_Count() > 0 )
	{
		for(int i=0; i<pStructure->Get_Field_Count(); i++)
		{
			_Add_Field(pStructure->Get_Field_Name(i), pStructure->Get_Field_Type(i));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_PointCloud::_On_Construction(void)
{
	m_Type			= SHAPE_TYPE_Point;
	m_Vertex_Type	= SG_VERTEX_TYPE_XYZ;

	m_nFields		= 0;
	m_Field_Name	= NULL;
	m_Field_Type	= NULL;
	m_Field_Stats	= NULL;
	m_Field_Offset	= NULL;

	m_Points		= NULL;
	m_nRecords		= 0;
	m_nPointBytes	= 0;

	m_Cursor		= NULL;
	m_bXYZPrecDbl	= true;

	m_Selected		= NULL;
	m_nSelected		= 0;

	Set_NoData_Value(-999999);

	Set_Update_Flag();

	m_Shapes.Create(SHAPE_TYPE_Point, NULL, NULL, SG_VERTEX_TYPE_XYZ);
	m_Shapes.Add_Shape();
	m_Shapes_Index	= -1;

	m_Array_Points  .Create(sizeof(char *), 0, SG_ARRAY_GROWTH_3);
	m_Array_Selected.Create(sizeof(int   ), 0, SG_ARRAY_GROWTH_3);
}

//---------------------------------------------------------
CSG_PointCloud::~CSG_PointCloud(void)
{
	Destroy();
}

bool CSG_PointCloud::Destroy(void)
{
	Del_Points();

	if( m_nFields > 0 )
	{
		for(int i=0; i<m_nFields; i++)
		{
			delete(m_Field_Name [i]);
			delete(m_Field_Stats[i]);
		}

		SG_Free(m_Field_Name);
		SG_Free(m_Field_Type);
		SG_Free(m_Field_Stats);
		SG_Free(m_Field_Offset);

		_On_Construction();
	}

	CSG_Data_Object::Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						File							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_PointCloud::_Load(const CSG_String &File_Name)
{
	TSG_Data_Type	Type;

	char		ID[6];
	int			i, iBuffer, nPointBytes, nFields;
	char		Name[1024];
	CSG_File	Stream;

	SG_UI_Msg_Add(CSG_String::Format(SG_T("%s: %s..."), _TL("Load point cloud"), File_Name.c_str()), true);

	//-----------------------------------------------------
	if( !Stream.Open(File_Name, SG_FILE_R, true) )
	{
		SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);
		SG_UI_Msg_Add_Error(_TL("file could not be opened."));

		return( false );
	}

	if( !Stream.Read(ID, 6) || strncmp(ID, POINTCLOUD_FILE_VERSION, 5) != 0 )
	{
		SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);
		SG_UI_Msg_Add_Error(_TL("incompatible file."));

		return( false );
	}

	if( !Stream.Read(&nPointBytes, sizeof(int)) || nPointBytes < (int)(3 * sizeof(float)) )
	{
		SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);
		SG_UI_Msg_Add_Error(_TL("incompatible file."));

		return( false );
	}

	if( !Stream.Read(&nFields, sizeof(int)) || nFields < 3 )
	{
		SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);
		SG_UI_Msg_Add_Error(_TL("incompatible file."));

		return( false );
	}

	//-----------------------------------------------------
	Destroy();

	for(i=0; i<nFields; i++)
	{
		if( !Stream.Read(&Type		, sizeof(TSG_Data_Type))
		||	!Stream.Read(&iBuffer	, sizeof(int)) || !(iBuffer > 0 && iBuffer < 1024)
		||	!Stream.Read(Name		, iBuffer) )
		{
			SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);
			SG_UI_Msg_Add_Error(_TL("incompatible file."));

			return( false );
		}

		if( ID[5] == '0' )	// Data Type Definition changed!!!
		{
			switch( Type )
			{
			default:	Type	= SG_DATATYPE_Undefined;	break;
			case 1:		Type	= SG_DATATYPE_Char;			break;
			case 2:		Type	= SG_DATATYPE_Short;		break;
			case 3:		Type	= SG_DATATYPE_Int;			break;
			case 4:		Type	= SG_DATATYPE_Long;			break;
			case 5:		Type	= SG_DATATYPE_Float;		break;
			case 6:		Type	= SG_DATATYPE_Double;		break;
			}
		}

		Name[iBuffer]	= '\0';

		if( !_Add_Field(CSG_String((const char *)Name), Type) )
		{
			SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);
			SG_UI_Msg_Add_Error(_TL("incompatible file."));

			return( false );
		}
	}

	if( m_nPointBytes != nPointBytes + 1 )
	{
		SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);
		SG_UI_Msg_Add_Error(_TL("incompatible file."));

		return( false );
	}

	//-----------------------------------------------------
	long		fLength	= Stream.Length();

	while( _Inc_Array() && Stream.Read(m_Cursor + 1, nPointBytes) && SG_UI_Process_Set_Progress(Stream.Tell(), fLength) )
	{}

	_Dec_Array();

	Set_File_Name(File_Name);

	Load_MetaData(File_Name);

	if( 0 > Get_Count() )
	{
		SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);
		SG_UI_Msg_Add_Error(_TL("no records in file."));

		return( false );
	}

	//-----------------------------------------------------
	SG_UI_Process_Set_Ready();

	Get_Projection().Load(SG_File_Make_Path(NULL, File_Name, SG_T("prj")), SG_PROJ_FMT_WKT);

	SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);

	return( true );
}

//---------------------------------------------------------
bool CSG_PointCloud::_Save(const CSG_String &File_Name)
{
	CSG_File	Stream;

	SG_UI_Msg_Add(CSG_String::Format(SG_T("%s: %s..."), _TL("Save point cloud"), File_Name.c_str()), true);

	CSG_String	sFile_Name = SG_File_Make_Path(NULL, File_Name, SG_T("spc"));

	if( Stream.Open(sFile_Name, SG_FILE_W, true) == false )
	{
		SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);
		SG_UI_Msg_Add_Error(_TL("unable to create file."));

		return( false );
	}

	int		i, iBuffer, nPointBytes	= m_nPointBytes - 1;

	Stream.Write((void *)POINTCLOUD_FILE_VERSION, 6);
	Stream.Write(&nPointBytes	, sizeof(int));
	Stream.Write(&m_nFields		, sizeof(int));

	for(i=0; i<m_nFields; i++)
	{
		Stream.Write(&m_Field_Type[i], sizeof(TSG_Data_Type));

		iBuffer	= (int)m_Field_Name[i]->Length();	if( iBuffer >= 1024 - 1 )	iBuffer	= 1024 - 1;
		Stream.Write(&iBuffer, sizeof(int));
		Stream.Write((void *)m_Field_Name[i]->b_str(), sizeof(char), iBuffer);
	}

	_Set_Shape(m_Shapes_Index);

	for(i=0; i<Get_Count() && SG_UI_Process_Set_Progress(i, Get_Count()); i++)
	{
		Stream.Write(m_Points[i] + 1, nPointBytes);
	}

	Set_Modified(false);

	Set_File_Name(sFile_Name);

	Save_MetaData(File_Name);

	Get_Projection().Save(SG_File_Make_Path(NULL, File_Name, SG_T("prj")), SG_PROJ_FMT_WKT);

	SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);

	return( true );
}

bool CSG_PointCloud::Save(const CSG_String &File_Name, int Format)
{
	return( _Save(File_Name) );
}


///////////////////////////////////////////////////////////
//														 //
//						Assign							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_PointCloud::Assign(CSG_Data_Object *pObject)
{
	if( pObject && pObject->is_Valid() && pObject->Get_ObjectType() == Get_ObjectType() )
	{
		Destroy();

		CSG_PointCloud	*pPointCloud	= (CSG_PointCloud *)pObject;

		Get_History()	= pPointCloud->Get_History();

		for(int iField=0; iField<pPointCloud->m_nFields; iField++)
		{
			_Add_Field(pPointCloud->m_Field_Name[iField]->c_str(), pPointCloud->m_Field_Type[iField]);
		}

		for(int iPoint=0; iPoint<pPointCloud->Get_Count(); iPoint++)
		{
			if( _Inc_Array() )
			{
				memcpy(m_Points[iPoint] + 1, pPointCloud->m_Points[iPoint] + 1, m_nPointBytes - 1);
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Checks							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_PointCloud::is_Compatible(CSG_PointCloud *pPointCloud) const
{
	if( Get_Field_Count() == pPointCloud->Get_Field_Count() )
	{
		for(int i=0; i<Get_Field_Count(); i++)
		{
			if( Get_Field_Type(i) != pPointCloud->Get_Field_Type(i) )
			{
				return( false );
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Fields							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_PointCloud::_Add_Field(const SG_Char *Name, TSG_Data_Type Type, int iField)
{
	if( !Name || SG_Data_Type_Get_Size(Type) <= 0 )
	{
		return( false );
	}

	m_Field_Name	= (CSG_String            **)SG_Realloc(m_Field_Name  , (m_nFields + 1) * sizeof(CSG_String *));
	m_Field_Type	= (TSG_Data_Type          *)SG_Realloc(m_Field_Type  , (m_nFields + 1) * sizeof(TSG_Data_Type));
	m_Field_Stats	= (CSG_Simple_Statistics **)SG_Realloc(m_Field_Stats , (m_nFields + 1) * sizeof(CSG_Simple_Statistics *));
	m_Field_Offset	= (int                    *)SG_Realloc(m_Field_Offset, (m_nFields + 1) * sizeof(int));

	m_Field_Name  [m_nFields]	= new CSG_String(Name);
	m_Field_Type  [m_nFields]	= Type;
	m_Field_Stats [m_nFields]	= new CSG_Simple_Statistics();
	m_Field_Offset[m_nFields]	= m_nFields == 0 ? 1 : m_Field_Offset[m_nFields - 1] + (int)SG_Data_Type_Get_Size(m_Field_Type[m_nFields - 1]);

	if( m_nFields == 0 )
	{
		m_nPointBytes	= 1;
	}

	m_nPointBytes	+= (int)SG_Data_Type_Get_Size(m_Field_Type[m_nFields]);
	m_nFields		++;

	m_Shapes.Add_Field(Name, Type);

	for(int i=0; i<Get_Count(); i++)
	{
		m_Points[i]	= (char *)SG_Realloc(m_Points[i], m_nPointBytes * sizeof(char));
	}

	Set_Modified();

	return( true );
}

//---------------------------------------------------------
bool CSG_PointCloud::Add_Field(const CSG_String &Name, TSG_Data_Type Type, int iField)
{
	if( m_nFields == 0 )
	{
		_Add_Field(SG_T("X"), m_bXYZPrecDbl ? SG_DATATYPE_Double : SG_DATATYPE_Float);
		_Add_Field(SG_T("Y"), m_bXYZPrecDbl ? SG_DATATYPE_Double : SG_DATATYPE_Float);
		_Add_Field(SG_T("Z"), m_bXYZPrecDbl ? SG_DATATYPE_Double : SG_DATATYPE_Float);
	}

	return( _Add_Field(Name, Type, iField) );
}

//---------------------------------------------------------
bool CSG_PointCloud::Del_Field(int iField)
{
	int		i;

	if( iField < 3 || iField >= m_nFields )
	{
		return( false );
	}

	if( m_nFields == 1 )
	{
		return( Destroy() );
	}

	//-----------------------------------------------------
	m_nFields		--;
	m_nPointBytes	-= (int)SG_Data_Type_Get_Size(m_Field_Type[iField]);

	for(i=0; i<m_nRecords; i++)
	{
		if( iField < m_nFields )
		{
			memmove(
				m_Points[i] + m_Field_Offset[iField],
				m_Points[i] + m_Field_Offset[iField + 1],
				m_Field_Offset[iField + 1] - m_Field_Offset[iField]
			);
		}

		m_Points[i]	= (char *)SG_Realloc(m_Points[i], m_nPointBytes * sizeof(char));
	}

	//-----------------------------------------------------
	delete(m_Field_Name [iField]);
	delete(m_Field_Stats[iField]);

	for(i=iField; i<m_nFields; i++)
	{
		m_Field_Name  [i]	= m_Field_Name  [i + 1];
		m_Field_Type  [i]	= m_Field_Type  [i + 1];
		m_Field_Stats [i]	= m_Field_Stats [i + 1];
		m_Field_Offset[i]	= m_Field_Offset[i - 1] + (int)SG_Data_Type_Get_Size(m_Field_Type[i - 1]);
	}

	m_Field_Name	= (CSG_String            **)SG_Realloc(m_Field_Name  , m_nFields * sizeof(CSG_String *));
	m_Field_Type	= (TSG_Data_Type          *)SG_Realloc(m_Field_Type  , m_nFields * sizeof(TSG_Data_Type));
	m_Field_Stats	= (CSG_Simple_Statistics **)SG_Realloc(m_Field_Stats , m_nFields * sizeof(CSG_Simple_Statistics *));
	m_Field_Offset	= (int                    *)SG_Realloc(m_Field_Offset, m_nFields * sizeof(int));

	Set_Modified();

	return( true );
}

//---------------------------------------------------------
bool CSG_PointCloud::_Set_Field_Value(char *pPoint, int iField, double Value)
{
	if( pPoint && iField >= 0 && iField < m_nFields )
	{
		pPoint	= ((char *)pPoint) + m_Field_Offset[iField];

		switch( m_Field_Type[iField] )
		{
		default:
		case SG_DATATYPE_Undefined:	break;
		case SG_DATATYPE_Byte:		*((BYTE   *)pPoint)	= (BYTE  )Value;	break;
		case SG_DATATYPE_Char:		*((char   *)pPoint)	= (char  )Value;	break;
		case SG_DATATYPE_Word:		*((WORD   *)pPoint)	= (WORD  )Value;	break;
		case SG_DATATYPE_Short:		*((short  *)pPoint)	= (short )Value;	break;
		case SG_DATATYPE_DWord:		*((DWORD  *)pPoint)	= (DWORD )Value;	break;
		case SG_DATATYPE_Int:		*((int    *)pPoint)	= (int   )Value;	break;
		case SG_DATATYPE_Long:		*((long   *)pPoint)	= (long  )Value;	break;
		case SG_DATATYPE_Float:		*((float  *)pPoint)	= (float )Value;	break;
		case SG_DATATYPE_Double:	*((double *)pPoint)	= (double)Value;	break;
		}

		m_Field_Stats[iField]->Invalidate();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
double CSG_PointCloud::_Get_Field_Value(char *pPoint, int iField) const
{
	if( pPoint && iField >= 0 && iField < m_nFields )
	{
		pPoint	= ((char *)pPoint) + m_Field_Offset[iField];

		switch( m_Field_Type[iField] )
		{
		case SG_DATATYPE_Undefined:		default:	break;
		case SG_DATATYPE_Byte:			return( *((BYTE   *)pPoint) );
		case SG_DATATYPE_Char:			return( *((char   *)pPoint) );
		case SG_DATATYPE_Word:			return( *((WORD   *)pPoint) );
		case SG_DATATYPE_Short:			return( *((short  *)pPoint) );
		case SG_DATATYPE_DWord:			return( *((DWORD  *)pPoint) );
		case SG_DATATYPE_Int:			return( *((int    *)pPoint) );
		case SG_DATATYPE_Long:			return( *((long   *)pPoint) );
		case SG_DATATYPE_Float:			return( *((float  *)pPoint) );
		case SG_DATATYPE_Double:		return( *((double *)pPoint) );
		}
	}

	return( 0.0 );
}

//---------------------------------------------------------
bool CSG_PointCloud::Set_Value(int iPoint, int iField, const SG_Char *Value)
{
	if( iPoint >= 0 && iPoint < m_nRecords && Value && *Value )
	{
		double		d;
		CSG_String	s(Value);

		if( s.asDouble(d) )
		{
			return( _Set_Field_Value(m_Points[iPoint], iField, d) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_PointCloud::Get_Value(int iPoint, int iField, CSG_String &Value)	const
{
	if( iPoint >= 0 && iPoint < m_nRecords && iField >= 0 && iField < m_nFields )
	{
		Value.Printf(SG_T("%f"), _Get_Field_Value(m_Points[iPoint], iField));

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
TSG_Point_Z CSG_PointCloud::Get_Point(void)	const
{
	TSG_Point_Z	p;

	if( m_Cursor )
	{
		p.x	= _Get_Field_Value(m_Cursor, 0);
		p.y	= _Get_Field_Value(m_Cursor, 1);
		p.z	= _Get_Field_Value(m_Cursor, 2);
	}
	else
	{
		p.x	= p.y	= p.z	= 0.0;
	}

	return( p );
}

//---------------------------------------------------------
TSG_Point_Z CSG_PointCloud::Get_Point(int iPoint)	const
{
	TSG_Point_Z	p;

	if( iPoint >= 0 && iPoint < Get_Count() )
	{
		char	*pPoint	= m_Points[iPoint];

		p.x	= _Get_Field_Value(pPoint, 0);
		p.y	= _Get_Field_Value(pPoint, 1);
		p.z	= _Get_Field_Value(pPoint, 2);
	}
	else
	{
		p.x	= p.y	= p.z	= 0.0;
	}

	return( p );
}

//---------------------------------------------------------
bool CSG_PointCloud::On_NoData_Changed(void)
{
	for(int i=3; i<m_nFields; i++)
	{
		m_Field_Stats[i]->Invalidate();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_PointCloud::Add_Point(double x, double y, double z)
{
	if( _Inc_Array() )
	{
		_Set_Field_Value(m_Cursor, 0, x);
		_Set_Field_Value(m_Cursor, 1, y);
		_Set_Field_Value(m_Cursor, 2, z);

		Set_Modified();
		Set_Update_Flag();
		_Stats_Invalidate();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_PointCloud::Del_Point(int iPoint)
{
	if( iPoint >= 0 && iPoint < Get_Count() )
	{
		if( is_Selected(iPoint) )
		{
			Select(iPoint, true);
		}

		m_Cursor	= m_Points[iPoint];

		for(int i=iPoint, j=iPoint+1; j<Get_Count(); i++, j++)
		{
			m_Points[i]	= m_Points[j];
		}

		m_Points[Get_Count() - 1]	= m_Cursor;

		m_Cursor	= NULL;

		_Dec_Array();

		Set_Modified();
		Set_Update_Flag();
		_Stats_Invalidate();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_PointCloud::Del_Points(void)
{
	for(int iPoint=0; iPoint<m_nRecords; iPoint++)
	{
		SG_Free(m_Points[iPoint]);
	}

	m_Array_Points  .Destroy();
	m_Array_Selected.Destroy();

	m_nRecords	= 0;
	m_Points	= NULL;
	m_Cursor	= NULL;

	m_nSelected	= 0;
	m_Selected	= NULL;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_PointCloud::_Inc_Array(void)
{
	if( m_nFields > 0 && m_Array_Points.Set_Array(m_nRecords + 1, (void **)&m_Points) )
	{
		m_Points[m_nRecords++]	= m_Cursor	= (char *)SG_Calloc(m_nPointBytes, sizeof(char));

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_PointCloud::_Dec_Array(void)
{
	if( m_nRecords > 0 )
	{
		m_nRecords	--;

		m_Cursor	= NULL;

		SG_Free(m_Points[m_nRecords]);

		m_Array_Points.Set_Array(m_nRecords, (void **)&m_Points);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Statistics						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_PointCloud::On_Update(void)
{
	if( m_nFields >= 2 )
	{
		_Set_Shape(m_Shapes_Index);

		_Stats_Update(0);
		_Stats_Update(1);

		m_Extent.Assign(
			m_Field_Stats[0]->Get_Minimum(), m_Field_Stats[1]->Get_Minimum(),
			m_Field_Stats[0]->Get_Maximum(), m_Field_Stats[1]->Get_Maximum()
		);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_PointCloud::_Stats_Update(int iField) const
{
	if( iField >= 0 && iField < m_nFields && Get_Count() > 0 )
	{
		if( !m_Field_Stats[iField]->is_Evaluated() )
		{
			char	**pPoint	= m_Points;

			for(int iPoint=0; iPoint<Get_Count(); iPoint++, pPoint++)
			{
				double	Value	= _Get_Field_Value(*pPoint, iField);

				if( iField < 3 || is_NoData_Value(Value) == false )
				{
					m_Field_Stats[iField]->Add_Value(Value);
				}
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
CSG_Shape * CSG_PointCloud::_Set_Shape(int iPoint)
{
	SG_UI_Progress_Lock(true);

	CSG_Shape	*pShape	= m_Shapes.Get_Shape(0);

	if( pShape->is_Modified() && m_Shapes_Index >= 0 && m_Shapes_Index < Get_Count() )
	{
		m_Cursor	= m_Points[m_Shapes_Index];

		for(int i=0; i<Get_Field_Count(); i++)
		{
			Set_Value(i, pShape->asDouble(i));
		}

		Set_Value(0, pShape->Get_Point(0).x);
		Set_Value(1, pShape->Get_Point(0).y);
		Set_Value(2, pShape->Get_Z(0));
	}

	if( iPoint >= 0 && iPoint < Get_Count() )
	{
		if( iPoint != m_Shapes_Index )
		{
			m_Cursor	= m_Points[iPoint];

			pShape->Set_Point(Get_X(), Get_Y(), 0, 0);
			pShape->Set_Z    (Get_Z()         , 0, 0);

			for(int i=0; i<Get_Field_Count(); i++)
			{
				pShape->Set_Value(i, Get_Value(i));
			}

			m_Shapes_Index	= iPoint;
		}

		m_Shapes.Set_Modified(false);

		SG_UI_Progress_Lock(false);

		return( pShape );
	}

	m_Shapes_Index	= -1;

	SG_UI_Progress_Lock(false);

	return( NULL );
}

//---------------------------------------------------------
CSG_Table_Record * CSG_PointCloud::Get_Record(int iRecord)	const
{
	return( ((CSG_PointCloud *)this)->_Set_Shape(iRecord) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape * CSG_PointCloud::Get_Shape(TSG_Point Point, double Epsilon)
{
	CSG_Rect	r(Point.x - Epsilon, Point.y - Epsilon, Point.x + Epsilon, Point.y + Epsilon);

	if( r.Intersects(Get_Extent()) != INTERSECTION_None )
	{
		int		iPoint		= -1;
		double	iDistance	= -1.0;

		for(int iRecord=0; iRecord<Get_Count(); iRecord++)
		{
			Set_Cursor(iRecord);

			if( r.Contains(Get_X(), Get_Y()) )
			{
				if( iPoint < 0 || iDistance > SG_Get_Distance(Point.x, Point.y, Get_X(), Get_Y()) )
				{
					iPoint		= iRecord;
					iDistance	= SG_Get_Distance(Point.x, Point.y, Get_X(), Get_Y());
				}
			}
		}

		if( iPoint >= 0 )
		{
			return( CSG_Shapes::Get_Shape(iPoint) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_Table_Record * CSG_PointCloud::Ins_Record(int iRecord, CSG_Table_Record *pCopy)
{
	return( NULL );
}

//---------------------------------------------------------
CSG_Table_Record * CSG_PointCloud::Add_Record(CSG_Table_Record *pCopy)
{
	return( NULL );
}

//---------------------------------------------------------
CSG_Shape * CSG_PointCloud::Add_Shape(CSG_Table_Record *pCopy, TSG_ADD_Shape_Copy_Mode mCopy)
{
	Add_Point(0.0, 0.0, 0.0);

	if( pCopy && (mCopy == SHAPE_COPY_ATTR || mCopy == SHAPE_COPY) )
	{
		for(int iField=0; iField<Get_Field_Count() && iField<pCopy->Get_Table()->Get_Field_Count(); iField++)
		{
			if( Get_Field_Type(iField) == pCopy->Get_Table()->Get_Field_Type(iField) )
			{
				Set_Value(iField, pCopy->asDouble(iField));
			}
		}
	}

	return( _Set_Shape(Get_Count() - 1) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_PointCloud::Select(int iRecord, bool bInvert)
{
	if( !bInvert )
	{
		for(int i=0; i<m_nSelected; i++)
		{
			m_Points[m_Selected[i]][0]	&= ~SG_TABLE_REC_FLAG_Selected;
		}

		m_Array_Selected.Destroy();
		m_Selected	= NULL;
		m_nSelected	= 0;
	}

	if( Set_Cursor(iRecord) )
	{
		if( (m_Cursor[0] & SG_TABLE_REC_FLAG_Selected) == 0 )	// select
		{
			if( m_Array_Selected.Set_Array(m_nSelected + 1, (void **)&m_Selected) )
			{
				m_Cursor[0]	|= SG_TABLE_REC_FLAG_Selected;

				m_Selected[m_nSelected++]	= iRecord;

				return( true );
			}
		}
		else													// deselect
		{
			m_Cursor[0]	&= ~SG_TABLE_REC_FLAG_Selected;

			m_nSelected--;

			for(int i=0; i<m_nSelected; i++)
			{
				if( m_Points[iRecord] == m_Points[m_Selected[i]] )
				{
					for(; i<m_nSelected; i++)
					{
						m_Selected[i]	= m_Selected[i + 1];
					}
				}
			}

			m_Array_Selected.Set_Array(m_nSelected, (void **)&m_Selected);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_PointCloud::Select(CSG_Shape *pShape, bool bInvert)
{
	return( false );
}

//---------------------------------------------------------
bool CSG_PointCloud::Select(TSG_Rect Extent, bool bInvert)
{
	if( !bInvert )	// clear selection
	{
		Select(-1, false);
	}

	if( Get_Extent().Intersects(Extent) != INTERSECTION_None )
	{
		for(int i=0; i<Get_Count(); i++)
		{
			Set_Cursor(i);

			if(	Extent.xMin <= Get_X() && Get_X() <= Extent.xMax
			&&	Extent.yMin <= Get_Y() && Get_Y() <= Extent.yMax )
			{
				Select(i, true);
			}
		}
	}

	return( m_nSelected > 0 );
}

//---------------------------------------------------------
bool CSG_PointCloud::Select(TSG_Point Point, bool bInvert)
{
	CSG_Rect	r(Point.x, Point.y, Point.x, Point.y);

	return( Select(r.m_rect, bInvert) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_PointCloud::is_Selected(int iRecord)	const
{
	return( iRecord >= 0 && iRecord < Get_Count() && (m_Points[iRecord][0] & SG_TABLE_REC_FLAG_Selected) != 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape * CSG_PointCloud::Get_Selection(int Index)
{
	return( Index >= 0 && Index < m_nSelected ? _Set_Shape(m_Selected[Index]) : NULL );
}

//---------------------------------------------------------
const CSG_Rect & CSG_PointCloud::Get_Selection_Extent(void)
{
	if( m_nSelected > 0 && Set_Cursor(m_Selected[0]) )
	{
		TSG_Rect	r;

		r.xMin	= r.xMax	= Get_X();
		r.yMin	= r.yMax	= Get_Y();

		for(int i=1; i<m_nSelected; i++)
		{
			if( Set_Cursor(m_Selected[i]) )
			{
				if( Get_X() < r.xMin )	r.xMin	= Get_X();	else if( Get_X() > r.xMax )	r.xMax	= Get_X();
				if( Get_Y() < r.yMin )	r.yMin	= Get_Y();	else if( Get_Y() > r.yMax )	r.yMax	= Get_Y();
			}
		}

		m_Extent_Selected.Assign(r);
	}
	else
	{
		m_Extent_Selected.Assign(0.0, 0.0, 0.0, 0.0);
	}

	return( m_Extent_Selected );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_PointCloud::Del_Selection(void)
{
	if( m_nSelected <= 0 )
	{
		return( 0 );
	}

	//-----------------------------------------------------
	int		i, n;

	m_Array_Selected.Set_Array(0, (void **)&m_Selected);
	m_nSelected	= 0;
	m_Cursor	= NULL;

	for(i=0, n=0; i<m_nRecords; i++)
	{
		if( (m_Points[i][0] & SG_TABLE_REC_FLAG_Selected) != 0 )
		{
			SG_Free(m_Points[i]);
		}
		else
		{
			if( n < i )
			{
				m_Points[n]	= m_Points[i];
			}

			n++;
		}
	}

	m_Array_Points.Set_Array(m_nRecords = n, (void **)&m_Points);

	return( n );
}

//---------------------------------------------------------
int CSG_PointCloud::Inv_Selection(void)
{
	char	**pPoint;
	int		i, n;

	n	= m_nRecords - m_nSelected;

	if( m_Array_Selected.Set_Array(n, (void **)&m_Selected) )
	{
		for(i=0, m_nSelected=0, pPoint=m_Points; i<m_nRecords; i++, pPoint++)
		{
			if( ((*pPoint)[0] & SG_TABLE_REC_FLAG_Selected) == 0 && m_nSelected < n )
			{
				m_Selected[m_nSelected++]	= i;

				(*pPoint)[0]	|= SG_TABLE_REC_FLAG_Selected;
			}
			else
			{
				(*pPoint)[0]	&= ~SG_TABLE_REC_FLAG_Selected;
			}
		}
	}

	return( m_nSelected );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

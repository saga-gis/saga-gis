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
//                      metadata.h                       //
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
#ifndef HEADER_INCLUDED__SAGA_API__metadata_H
#define HEADER_INCLUDED__SAGA_API__metadata_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "api_core.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_MetaData
{
public:
								CSG_MetaData		(void);
	bool						Create				(void);

								CSG_MetaData		(const CSG_MetaData &MetaData);
	bool						Create				(const CSG_MetaData &MetaData);

								CSG_MetaData		(const CSG_String &File, const SG_Char *Extension = NULL);
	bool						Create				(const CSG_String &File, const SG_Char *Extension = NULL);

								CSG_MetaData		(CSG_File &File);
	bool						Create				(CSG_File &File);

	virtual						~CSG_MetaData		(void);
	void						Destroy				(void);

	bool						Load				(const CSG_String &File, const SG_Char *Extension = NULL);
	bool						Save				(const CSG_String &File, const SG_Char *Extension = NULL)	const;

	bool						Load				(CSG_File &Stream);
	bool						Save				(CSG_File &Stream)			const;

	bool						Load_WKT			(const CSG_String &WKT);
	bool						Save_WKT			(      CSG_String &WKT)		const;

	bool						Assign				(const CSG_MetaData &MetaData, bool bAddChildren = true);

	CSG_MetaData &				operator  =			(const CSG_MetaData &MetaData)		{	Assign   (MetaData, true);	return( *this );	}
	CSG_MetaData &				operator +=			(const CSG_MetaData &MetaData)		{	Add_Child(MetaData, true);	return( *this );	}

	const CSG_String &			Get_Name			(void)						const	{	return( m_Name );		}
	void						Set_Name			(const CSG_String &Name)			{	m_Name		= Name;		}
	const CSG_String &			Get_Content			(void)						const	{	return( m_Content );	}
	void						Set_Content			(const CSG_String &Content)			{	m_Content	= Content;	}
	void						Fmt_Content			(const SG_Char *Format, ...);
	bool						Cmp_Content			(const CSG_String &String, bool bNoCase = false)	const;

	int							Get_Children_Count	(void)						const	{	return( m_nChildren );	}
	CSG_MetaData *				Get_Child			(int Index)					const	{	return( Index >= 0 && Index < m_nChildren ? m_pChildren[Index] : NULL );	}
	CSG_MetaData *				Get_Child			(const CSG_String &Name)	const	{	return( Get_Child(_Get_Child(Name)) );	}
	CSG_MetaData *				Add_Child			(void);
	CSG_MetaData *				Add_Child			(const CSG_String &Name);
	CSG_MetaData *				Add_Child			(const CSG_String &Name, const CSG_String &Content);
	CSG_MetaData *				Add_Child			(const CSG_String &Name, double            Content);
	CSG_MetaData *				Add_Child			(const CSG_String &Name, int               Content);
	CSG_MetaData *				Add_Child			(const CSG_MetaData &MetaData, bool bAddChildren = true);
	bool						Del_Child			(int Index);
	bool						Del_Child			(const CSG_String &Name)			{	return( Del_Child(_Get_Child(Name)) );	}

	bool						Add_Children		(const CSG_MetaData &MetaData);
	bool						Del_Children		(int Depth = 0);

	const CSG_MetaData &		operator []			(int Index)					const	{	CSG_MetaData	*pData	= Index >= 0 && Index < m_nChildren ? m_pChildren[Index] : NULL;	return( *pData );	}
	const CSG_MetaData &		operator []			(const CSG_String &Name)	const	{	return( *Get_Child(_Get_Child(Name)) );	}

	bool						is_Valid			(void)						const	{	return( this != NULL );					}

	int							Get_Property_Count	(void)						const	{	return( m_Prop_Names.Get_Count() );	}
	const CSG_String &			Get_Property_Name	(int i)						const	{	return( m_Prop_Names [i] );			}
	const SG_Char *				Get_Property		(int i)						const	{	return( i >= 0 && i < m_Prop_Values.Get_Count() ? m_Prop_Values[i].c_str() : NULL );	}
	const SG_Char *				Get_Property		(const CSG_String &Name)	const	{	return( Get_Property(_Get_Property(Name)) );	}
	bool						Get_Property		(const CSG_String &Name, CSG_String &Value)	const;
	bool						Get_Property		(const CSG_String &Name, double     &Value)	const;
	bool						Get_Property		(const CSG_String &Name, int        &Value)	const;
	bool						Add_Property		(const CSG_String &Name, const CSG_String &Value);
	bool						Add_Property		(const CSG_String &Name, double            Value);
	bool						Add_Property		(const CSG_String &Name, int               Value);
	bool						Set_Property		(const CSG_String &Name, const CSG_String &Value, bool bAddIfNotExists = true);
	bool						Set_Property		(const CSG_String &Name, double            Value, bool bAddIfNotExists = true);
	bool						Set_Property		(const CSG_String &Name, int               Value, bool bAddIfNotExists = true);
	bool						Cmp_Property		(const CSG_String &Name, const CSG_String &String, bool bNoCase = false)	const;

	CSG_String					asText				(int Flags = 0) const;
	class CSG_Table				asTable				(int Flags = 0) const;


private:

	CSG_MetaData(CSG_MetaData *pParent);


	int							m_nChildren, m_nBuffer;

	CSG_MetaData				**m_pChildren, *m_pParent, *m_pDummy;

	CSG_String					m_Name, m_Content;

	CSG_Strings					m_Prop_Names, m_Prop_Values;


	void						_On_Construction	(void);

	int							_Get_Child			(const CSG_String &Name)	const;
	int							_Get_Property		(const CSG_String &Name)	const;

	void						_Load				(class wxXmlNode *pNode);
	void						_Save				(class wxXmlNode *pNode)	const;

	bool						_Load_WKT			(const CSG_String &WKT);
	bool						_Save_WKT			(      CSG_String &WKT)		const;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__metadata_H

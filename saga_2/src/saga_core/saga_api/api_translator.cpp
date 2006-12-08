
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
//                  api_translator.cpp                   //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include <string.h>

#include "api_core.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Translator		gSG_Translator;

//---------------------------------------------------------
CSG_Translator *	SG_Get_Translator(void)
{
	return( &gSG_Translator );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const SG_Char *	SG_Translate(const SG_Char *Text)
{
	return( gSG_Translator.Get_Translation(Text) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Translator::CSG_Translator(void)
{
	m_nTranslations	= 0;
	m_Translations	= NULL;
}

//---------------------------------------------------------
CSG_Translator::CSG_Translator(const SG_Char *File_Name, bool bSetExtension)
{
	m_nTranslations	= 0;
	m_Translations	= NULL;

	Create(File_Name, bSetExtension);
}

//---------------------------------------------------------
CSG_Translator::~CSG_Translator(void)
{
	Destroy();
}

//---------------------------------------------------------
void CSG_Translator::Destroy(void)
{
	for(int i=0; i<m_nTranslations; i++)
	{
		delete(m_Translations[i]);
	}

	SG_Free(m_Translations);

	m_nTranslations	= 0;
	m_Translations	= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Translator::Create(const SG_Char *File_Name, bool bSetExtension)
{
	int			a, b;
	CSG_File	Stream;
	CSG_String	Line, Text, Translation, LNG_File;

	Destroy();

	if( bSetExtension )
	{
		LNG_File	= SG_File_Make_Path(NULL, File_Name, SG_T("lng"));
	}
	else
	{
		LNG_File	= File_Name;
	}

	if( Stream.Open(LNG_File, SG_FILE_R, false) )
	{
		while( Stream.Read_Line(Line) )
		{
			if( Line.Find(SG_T("ENTRY")) >= 0
			&&	(a = Line.Find(SG_T('('), false)) >= 0
			&&	(b = Line.Find(SG_T(')'), true )) >= 0 )
			{
				if( Text.Length() > 0 && Translation.Length() > 0 )
				{
					_Add_Translation(Text, Translation);
				}

				Text	= Line.Mid(a + 1, (b - a) - 1);
				Translation.Clear();
			}

			if(	(a = Line.Find(SG_T('\"'), false)) >= 0
			&&	(b = Line.Find(SG_T('\"'), true )) >= 0 && b - a > 0 )
			{
				Translation	= Line.Mid(a + 1, (b - a) - 1);
			}
		}
	}

	return( m_nTranslations > 0 );
}

//---------------------------------------------------------
void CSG_Translator::_Add_Translation(const SG_Char *Text, const SG_Char *Translation)
{
	int		i, Insert;

	Insert	= _Get_Index(Text);

	if( Insert == m_nTranslations || m_Translations[Insert]->m_Text.Cmp(Text) )
	{
		m_nTranslations++;
		m_Translations	= (CTranslation **)SG_Realloc(m_Translations, m_nTranslations * sizeof(CTranslation *));

		for(i=m_nTranslations-1; i>Insert; i--)
		{
			m_Translations[i]	= m_Translations[i - 1];
		}

		m_Translations[Insert]	= new CTranslation(Text, Translation);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Translator::_Get_Index(const SG_Char *Text)
{
	int		a, b, i, c;

	if( m_nTranslations == 1 )
	{
		c	= m_Translations[0]->m_Text.Cmp(Text);

		return( c >= 0 ? 0 : 1 );
	}

	if( m_nTranslations > 1 )
	{
		for(a=0, b=m_nTranslations-1; b - a > 1; )
		{
			i	= a + (b - a) / 2;
			c	= m_Translations[i]->m_Text.Cmp(Text);

			if( c > 0 )
			{
				b	= i;
			}
			else if( c < 0 )
			{
				a	= i;
			}
			else
			{
				return( i );
			}
		}

		if( m_Translations[a]->m_Text.Cmp(Text) < 0 )
		{
			if( m_Translations[b]->m_Text.Cmp(Text) < 0 )
			{
				return( m_nTranslations );
			}

			return( b );
		}

		if( m_Translations[b]->m_Text.Cmp(Text) > 0 )
		{
			return( a );
		}
	}

	return( m_nTranslations );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const SG_Char * CSG_Translator::Get_Text(int Index)
{
	return( Index >= 0 && Index < m_nTranslations ? m_Translations[Index]->m_Text : SG_T("") );
}

//---------------------------------------------------------
const SG_Char * CSG_Translator::Get_Translation(int Index)
{
	return( Index >= 0 && Index < m_nTranslations ? m_Translations[Index]->m_Translation : SG_T("") );
}

//---------------------------------------------------------
const SG_Char * CSG_Translator::Get_Translation(const SG_Char *Text)
{
	if( Text )
	{
		if( m_nTranslations > 0 )
		{
			int			i;
			CSG_String	s(Text);

			if( *Text == '{' )
			{
				s	= s.AfterFirst('{').BeforeFirst('}');
			}

			if(	(i = _Get_Index(s)) < m_nTranslations && !m_Translations[i]->m_Text.Cmp(s) )
			{
				return( m_Translations[i]->m_Translation );
			}
		}

		if( *Text == '{' )
		{
			do	{	Text++;	}	while( *Text != '}' && *Text != '\0' );
			do	{	Text++;	}	while( *Text == ' ' && *Text != '\0' );
		}

		if( *Text == '[' )
		{
			do	{	Text++;	}	while( *Text != ']' && *Text != '\0' );
			do	{	Text++;	}	while( *Text == ' ' && *Text != '\0' );
		}
	}

	return( Text );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

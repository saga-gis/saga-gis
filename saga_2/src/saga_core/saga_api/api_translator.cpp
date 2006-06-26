
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
CTranslator		API_Translator;

//---------------------------------------------------------
CTranslator *	API_Get_Translator(void)
{
	return( &API_Translator );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const char *	LNG(const char *Text)
{
	return( API_Translator.Get_Translation(Text) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTranslator::CTranslator(void)
{
	m_nTranslations	= 0;
	m_Translations	= NULL;
}

//---------------------------------------------------------
CTranslator::CTranslator(const char *File_Name, bool bSetExtension)
{
	m_nTranslations	= 0;
	m_Translations	= NULL;

	Create(File_Name, bSetExtension);
}

//---------------------------------------------------------
CTranslator::~CTranslator(void)
{
	Destroy();
}

//---------------------------------------------------------
void CTranslator::Destroy(void)
{
	for(int i=0; i<m_nTranslations; i++)
	{
		delete(m_Translations[i]);
	}

	API_Free(m_Translations);

	m_nTranslations	= 0;
	m_Translations	= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTranslator::Create(const char *File_Name, bool bSetExtension)
{
	int			a, b;
	FILE		*Stream;
	CAPI_String	Line, Text, Translation, LNG_File;

	Destroy();

	if( bSetExtension )
	{
		LNG_File	= API_Make_File_Path(NULL, File_Name, "lng");
	}
	else
	{
		LNG_File	= File_Name;
	}

	if( File_Name && (Stream = fopen(LNG_File, "r")) != NULL )
	{
		while( API_Read_Line(Stream, Line) )
		{
			if( Line.Find("ENTRY") >= 0 && (a = Line.Find('(')) >= 0 && (b = Line.Find(')', true)) >= 0 )
			{
				if( Text.Length() > 0 && Translation.Length() > 0 )
				{
					_Add_Translation(Text, Translation);
				}

				Text	= Line.Mid(a + 1, (b - a) - 1);
				Translation.Clear();
			}

			if( (a = Line.Find('\"')) >= 0 && (b = Line.Find('\"', true)) >= 0 && b - a > 0 )
			{
				Translation	= Line.Mid(a + 1, (b - a) - 1);
			}
		}

		fclose(Stream);
	}

	return( m_nTranslations > 0 );
}

//---------------------------------------------------------
void CTranslator::_Add_Translation(const char *Text, const char *Translation)
{
	int		i, Insert;

	Insert	= _Get_Index(Text);

	if( Insert == m_nTranslations || m_Translations[Insert]->m_Text.Cmp(Text) )
	{
		m_nTranslations++;
		m_Translations	= (CTranslation **)API_Realloc(m_Translations, m_nTranslations * sizeof(CTranslation *));

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
int CTranslator::_Get_Index(const char *Text)
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
const char * CTranslator::Get_Text(int Index)
{
	return( Index >= 0 && Index < m_nTranslations ? m_Translations[Index]->m_Text : "" );
}

//---------------------------------------------------------
const char * CTranslator::Get_Translation(int Index)
{
	return( Index >= 0 && Index < m_nTranslations ? m_Translations[Index]->m_Translation : "" );
}

//---------------------------------------------------------
const char * CTranslator::Get_Translation(const char *Text)
{
	if( Text )
	{
		if( m_nTranslations > 0 )
		{
			int			i;
			CAPI_String	s(Text);

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


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
//                   api_history.cpp                     //
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

#include "dataobject.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define HISTORY_VERSION		SG_T("HISTORY_VERSION_1.02")
#define HISTORY_BEGIN		SG_T("HISTORY_BEGIN")
#define HISTORY_END			SG_T("HISTORY_END")
#define HISTORY_ENTRY_BEGIN	SG_T("ENTRY_BEGIN")
#define HISTORY_ENTRY_END	SG_T("ENTRY_END")


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_History_Entry::CSG_History_Entry(const SG_Char *Date, const SG_Char *Name, const SG_Char *Entry, CSG_History *pHistory)
{
	m_Date		= Date ? Date : SG_Get_CurrentTimeStr().c_str();
	m_Name		= Name ? Name : SG_T("Entry");
	m_Entry		= Entry;
	m_pHistory	= pHistory && pHistory->Get_Count() > 0
				? new CSG_History(*pHistory)
				: NULL;
}

//---------------------------------------------------------
CSG_History_Entry::CSG_History_Entry(const CSG_History_Entry &Entry)
{
	m_Date		= Entry.m_Date;
	m_Name		= Entry.m_Name;
	m_Entry		= Entry.m_Entry;
	m_pHistory	= Entry.m_pHistory && Entry.m_pHistory->Get_Count() > 0
				? new CSG_History(*Entry.m_pHistory)
				: NULL;
}

//---------------------------------------------------------
CSG_History_Entry::~CSG_History_Entry(void)
{
	if( m_pHistory )
	{
		delete(m_pHistory);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_History::CSG_History(void)
{
	m_nEntries	= 0;
	m_pEntries	= NULL;
}

//---------------------------------------------------------
CSG_History::CSG_History(const CSG_History &History)
{
	m_nEntries	= 0;
	m_pEntries	= NULL;

	Assign(History, false);
}

//---------------------------------------------------------
CSG_History::~CSG_History(void)
{
	Destroy();
}

//---------------------------------------------------------
void CSG_History::Destroy(void)
{
	if( m_nEntries > 0 )
	{
		for(int i=0; i<m_nEntries; i++)
		{
			delete(m_pEntries[i]);
		}

		SG_Free(m_pEntries);
	}

	m_nEntries	= 0;
	m_pEntries	= NULL;
}

//---------------------------------------------------------
void CSG_History::Assign(const CSG_History &History, bool bAdd)
{
	if( !bAdd )
	{
		Destroy();
	}

	for(int i=0; i<History.m_nEntries; i++)
	{
		_Add_Entry(new CSG_History_Entry(*History.m_pEntries[i]));
	}
}

//---------------------------------------------------------
void CSG_History::Add_Entry(const SG_Char *Name, const SG_Char *Entry, CSG_History *pHistory)
{
	if( Entry && *Entry )
	{
		_Add_Entry(new CSG_History_Entry(NULL, Name, Entry, pHistory));
	}
}

//---------------------------------------------------------
void CSG_History::_Add_Entry(CSG_History_Entry *pEntry)
{
	m_pEntries	= (CSG_History_Entry **)SG_Realloc(m_pEntries, (m_nEntries + 1) * sizeof(CSG_History_Entry *));
	m_pEntries[m_nEntries++]	= pEntry;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_History::Load(const CSG_String &File_Name, const CSG_String &File_Extension)
{
	bool		bResult	= false;
	CSG_File	Stream;
	CSG_String	sLine, fName(SG_File_Make_Path(NULL, File_Name, File_Extension));

	Destroy();

	if( Stream.Open(fName, SG_FILE_R, false) )
	{
		if(	Stream.Read_Line(sLine) && !sLine.Cmp(HISTORY_VERSION)
		&&	Stream.Read_Line(sLine) && !sLine.Cmp(HISTORY_BEGIN) )
		{
			bResult	= _Load(Stream);
		}
	}

	return( bResult );
}

//---------------------------------------------------------
bool CSG_History::Save(const CSG_String &File_Name, const CSG_String &File_Extension)
{
	bool		bResult	= false;
	CSG_File	Stream;
	CSG_String	fName(SG_File_Make_Path(NULL, File_Name, File_Extension));

	if( m_nEntries > 0 && Stream.Open(fName, SG_FILE_W) )
	{
		Stream.Printf(SG_T("%s\n"), HISTORY_VERSION);

		bResult	= _Save(Stream);
	}

	return( bResult );
}

//---------------------------------------------------------
bool CSG_History::_Load(CSG_File &Stream)
{
	Destroy();

	if( Stream.is_Open() )
	{
		CSG_String	sLine, Name, Entry, Date;
		CSG_History	History;

		while( Stream.Read_Line(sLine) && sLine.Cmp(HISTORY_END) )
		{
			if(	!sLine.Cmp(HISTORY_ENTRY_BEGIN) )
			{
				Stream.Read_Line(Date);
				Stream.Read_Line(Name);

				Entry.Clear();

				while( Stream.Read_Line(sLine) && sLine.Cmp(HISTORY_ENTRY_END) && sLine.Cmp(HISTORY_BEGIN) )
				{
					Entry.Append(sLine);
				}

				if( !sLine.Cmp(HISTORY_BEGIN) )
				{
					History._Load(Stream);
				}

				_Add_Entry(new CSG_History_Entry(Date, Name, Entry, &History));
			}
		}
	}

	return( m_nEntries > 0 );
}

//---------------------------------------------------------
bool CSG_History::_Save(CSG_File &Stream)
{
	if( Stream.is_Open() )
	{
		Stream.Printf(SG_T("%s\n"), HISTORY_BEGIN);

		for(int i=0; i<m_nEntries; i++)
		{
			Stream.Printf(SG_T("%s\n"), HISTORY_ENTRY_BEGIN);
			Stream.Printf(SG_T("%s\n"), m_pEntries[i]->m_Date .c_str());
			Stream.Printf(SG_T("%s\n"), m_pEntries[i]->m_Name .c_str());
			Stream.Printf(SG_T("%s\n"), m_pEntries[i]->m_Entry.c_str());

			if( m_pEntries[i]->m_pHistory )
			{
				m_pEntries[i]->m_pHistory->_Save(Stream);
			}

			Stream.Printf(SG_T("%s\n"), HISTORY_ENTRY_END);
		}

		Stream.Printf(SG_T("%s\n"), HISTORY_END);

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
CSG_String CSG_History::Get_HTML(void)
{
	CSG_String	s;

	if( m_nEntries > 0 )
	{
		s.Append(SG_T("<ul>"));

		for(int i=0; i<m_nEntries; i++)
		{
			s.Append(CSG_String::Format(SG_T("<li>[%s] <b>%s:</b> %s</li>\n"),
				m_pEntries[i]->Get_Date(),
				m_pEntries[i]->Get_Name(),
				m_pEntries[i]->Get_Entry()
			));

			if( m_pEntries[i]->m_pHistory )
			{
				s.Append(m_pEntries[i]->m_pHistory->Get_HTML());
			}
		}

		s.Append(SG_T("</ul>"));
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

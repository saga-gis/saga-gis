/*******************************************************************************
    PhotoLinks.cpp
    Copyright (C) Victor Olaya

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/
#include "CreateWebContent.h"
#include <saga_api/doc_html.h>

CCreateWebContent::CCreateWebContent(void)
{
	CSG_Parameter *pNode;
	
	//-----------------------------------------------------
	Parameters.Set_Name(_TL("Create Web Content"));

	Parameters.Set_Description(_TL(""));

	pNode = Parameters.Add_Shapes(NULL, 
						"SHAPES", 
						_TL("Shapes"),
						_TL(""),
						PARAMETER_INPUT);

	Parameters.Add_Table_Field(pNode,
								"NAME",
								_TL("Name Field"),
								_TL(""));
	
	Parameters.Add_FilePath(NULL,
							"OUTPUTPATH",
							_TL("Path for HTML Files"),
							_TL(""),
							_TL(""),
							_TL(""),
							true,
							true);

}

CCreateWebContent::~CCreateWebContent(void)
{}

bool CCreateWebContent::On_Execute(void)
{
	m_bDown	= false;
	m_pShapes = Parameters("SHAPES")->asShapes();

	m_iNameField = Parameters("NAME")->asInt();
	m_sOutputPath = Parameters("OUTPUTPATH")->asString();

	m_pShapes->Add_Field("HTML", TABLE_FIELDTYPE_String);
	m_iField = m_pShapes->Get_Field_Count() - 1;
	
	m_Pictures = new std::vector<CSG_String> [m_pShapes->Get_Count()];
	m_Links = new std::vector<CSG_String> [m_pShapes->Get_Count()];
	m_LinksDescription = new std::vector<CSG_String> [m_pShapes->Get_Count()];

	return true ;
}

bool CCreateWebContent::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	int i;
	int iIndex, iShape;
	CSG_Rect r;

	switch( Mode ){
	case MODULE_INTERACTIVE_LDOWN:
		
		if( m_bDown == false ){
			m_bDown	= true;
			m_ptDown = ptWorld;
		}//if
		
		return true ;

	case MODULE_INTERACTIVE_LUP:
		if( m_bDown == true ){
			m_bDown	= false;

			r.Assign(m_ptDown.Get_X(), m_ptDown.Get_Y(), ptWorld.Get_X(), ptWorld.Get_Y());

			if( r.Get_XRange() == 0.0 && r.Get_YRange() == 0.0 ){
				m_pShapes->Select(m_pShapes->Get_Shape(m_ptDown), is_Shift());
			}//if
			else{
				m_pShapes->Select(r, is_Shift());
			}//else

			DataObject_Update(m_pShapes, true);
			
		}//if
		
		return true;

	case MODULE_INTERACTIVE_RDOWN:

		CSG_String	sFileName;
		CSG_Strings	files;
		CSG_Parameters	dlg_files;
		dlg_files.Add_FilePath(NULL, "FILES", _TL("Files"), _TL(""), _TL("Images|*.bmp;*.jpg;*.png;*.tif|HTML Files|*.htm|All Files|*.*"), NULL, false, false, true);

		if( Dlg_Parameters(&dlg_files, _TL("Add Link to Files")) && dlg_files("FILES")->asFilePath()->Get_FilePaths(files) )
		{
			for (iShape = 0; iShape < m_pShapes->Get_Selection_Count(); iShape++){
				iIndex = m_pShapes->Get_Selection(iShape)->Get_Index();
				for (i = 0; i < files.Get_Count(); i++)
				{
					sFileName = "file://";
					sFileName.Append((CSG_String)files[i]);
					if (SG_File_Cmp_Extension (files[i],SG_T("htm"))){						
						m_LinksDescription[iIndex].push_back(SG_File_Get_Name(files[i], true));
						m_Links[iIndex].push_back(sFileName);
					}//if
					else if (  SG_File_Cmp_Extension (files[i],SG_T("bmp")) 
							|| SG_File_Cmp_Extension (files[i],SG_T("jpg")) 
							|| SG_File_Cmp_Extension (files[i],SG_T("png")) 				
							|| SG_File_Cmp_Extension (files[i],SG_T("tif"))){
						m_Pictures[iIndex].push_back(sFileName);
					}//else
				}				
				sFileName = SG_File_Make_Path(m_sOutputPath.c_str(), m_pShapes->Get_Shape(iIndex)->asString(m_iNameField), SG_T("htm"));
				m_pShapes->Get_Shape(iIndex)->Set_Value(m_iField, sFileName);
			}
		}
		break;

	}//switch

	return false ;

}//method

bool CCreateWebContent::On_Execute_Finish(void){

	int i,j;
	CSG_String sFileName;
	CSG_Doc_HTML HTMLDoc;

	for (i = 0; i < m_pShapes->Get_Count(); i++){
		Set_Progress(i, m_pShapes->Get_Count());
		const SG_Char **Pictures;

		if (m_Pictures[i].size() || m_Links[i].size()){
			HTMLDoc.Open(m_pShapes->Get_Shape(i)->asString(m_iNameField));
			Pictures = new const SG_Char *[m_Pictures[i].size()];
			for (j = 0; j < m_Pictures[i].size(); j++){
				Pictures[j] = m_Pictures[i].at(j).c_str();
			}//for*/
		
			HTMLDoc.AddThumbnails(Pictures, m_Pictures[i].size(), 4);
			HTMLDoc.AddLineBreak();
			for (j = 0; j < m_Links[i].size(); j++){
				HTMLDoc.AddHyperlink(m_LinksDescription[i].at(j).c_str(), m_Links[i].at(j).c_str());
				HTMLDoc.AddLineBreak();
			}//for

			sFileName = SG_File_Make_Path(m_sOutputPath.c_str(), m_pShapes->Get_Shape(i)->asString(m_iNameField), SG_T("htm"));
			HTMLDoc.Save(sFileName);
		}//if

	}//for

	return true;

}//method

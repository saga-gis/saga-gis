/*******************************************************************************
    Summarize.cpp
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
#include "Summarize.h"

#define VERY_LARGE_NUMBER 9999999999.

CSG_String sParam[] = {"[Sum]", "[Mean]", "[Variance]", "[Minimum]", "[Maximum]"};

CSummarize::CSummarize(void){

	CSG_Parameter *pNode;
	CSG_String sName;

	Parameters.Set_Name(_TL("Summary"));
	Parameters.Set_Description(_TL("(c) 2004 by Victor Olaya. summary."));

	pNode = Parameters.Add_Shapes(NULL,
								"SHAPES",
								_TL("Shapes"),
								"",
								PARAMETER_INPUT);

	Parameters.Add_Table_Field(pNode,
								"FIELD",
								_TL("Field"),
								"");

	Parameters.Add_Table(NULL,
						"TABLE",
						_TL("Summary Table"),
						"",
						PARAMETER_OUTPUT);

	pNode = Parameters.Add_Node(NULL,
						"PDFNODE",
						_TL("PDF Docs"),
						"");

	Parameters.Add_Value(pNode,
						"PDF",
						_TL("Create PDF Docs"),
						_TL("Create PDF Docs"),
						PARAMETER_TYPE_Bool,
						true);

	Parameters.Add_FilePath(pNode,
							"OUTPUTPATH",
							_TL("Folder"),
							_TL("Folder"),
							"",
							"",
							true,
							true);

	m_pExtraParameters	= Add_Parameters("EXTRA",
										_TL("Field for Summary"),
										"");

}//constructor


CSummarize::~CSummarize(void)
{}

bool CSummarize::On_Execute(void){

	int i,j;
	CSG_Table *pShapesTable;
	CSG_Parameter **pExtraParameter;
	CSG_String sName, sFilePath;

	m_iField = Parameters("FIELD")->asInt();
	m_pShapes = Parameters("SHAPES")->asShapes();
	m_pTable = Parameters("TABLE")->asTable();

	pShapesTable = &m_pShapes->Get_Table();
	m_bIncludeParam = new bool [pShapesTable->Get_Field_Count() * 5];
	pExtraParameter = new CSG_Parameter* [pShapesTable->Get_Field_Count() * 5];

	for (i = 0; i < pShapesTable->Get_Field_Count(); i++){
		for (j = 0; j < 5; j++){
			if (pShapesTable->Get_Field_Type(i) > 1 && pShapesTable->Get_Field_Type(i) < 7){ //is numeric field
				sName = pShapesTable->Get_Field_Name(i);
				sName.Append(_TL(sParam[j]));
				pExtraParameter[i * 5 + j] = m_pExtraParameters->Add_Value(NULL,
																			SG_Get_String(i * 5 + j,0).c_str(),
																			sName.c_str(),
																			"",
																			PARAMETER_TYPE_Bool,
																			false);
				m_bIncludeParam[i * 5 + j] = true;
			}//if
			else{
				m_bIncludeParam[i * 5 + j] = false;
			}//else
		}//for
	}//for

	if(Dlg_Parameters("EXTRA")){
		for (i = 0; i < pShapesTable->Get_Field_Count() * 5; i++){
			sName = SG_Get_String(i,0);
			if (m_bIncludeParam[i]){
				m_bIncludeParam[i] = Get_Parameters("EXTRA")->Get_Parameter(sName.c_str())->asBool();
			}//if			
		}//for

		Summarize();

		if (Parameters("PDF")->asBool()){
			if (Parameters("OUTPUTPATH")->asString()){
				sName = _TL("Summary_");
				sName.Append(m_pShapes->Get_Name());
				m_DocEngine.Open(PDF_PAGE_SIZE_A3, PDF_PAGE_ORIENTATION_LANDSCAPE, sName);
				CreatePDFDocs();
				sFilePath = SG_File_Make_Path(Parameters("OUTPUTPATH")->asString(), sName, "pdf");
				if (m_DocEngine.Save(sFilePath)){
					if (!m_DocEngine.Close()){
						Message_Add(_TL("\n\n ** Error : Could not close PDF engine ** \n\n"));
					}
				}//if
				else{
					Message_Add(_TL("\n\n ** Error : Could not save PDF file ** \n\n"));
				}//else
			}//if
		}//if

		m_pExtraParameters->Destroy();

		delete [] m_bIncludeParam;

		return true;

	}//if

	m_pExtraParameters->Destroy();

	delete [] m_bIncludeParam;

	return false;

}//method

void CSummarize::CreatePDFDocs(){

	CSG_Shapes *pShapes;
	CSG_Shape *pShape;
	CSG_Table *pShapesTable;
	int i,j;
	
	m_DocEngine.Add_Page_Title (_TL("Summary"), PDF_TITLE_01, PDF_PAGE_SIZE_A3, PDF_PAGE_ORIENTATION_LANDSCAPE);

	pShapesTable = &m_pShapes->Get_Table();
	pShapes = new CSG_Shapes();
	for (i = 0; i < m_ClassesID.size(); i++){
		Set_Progress(i,m_ClassesID.size());
		pShapes->Create(m_pShapes->Get_Type());
		for (j = 0; j < m_pShapes->Get_Count(); j++){
			if (m_pClasses[j] == i){
				pShape = pShapes->Add_Shape();
				pShape->Assign(m_pShapes->Get_Shape(j));
			}//if
		}//for
		m_DocEngine.AddClassSummaryPage(pShapes, m_pTable, i, m_ClassesID[i]);
	}//for
	
	if (m_pTable->Get_Record_Count() > 1){
		m_DocEngine.Add_Page_Title (_TL("Statistics"), PDF_TITLE_01, PDF_PAGE_SIZE_A4, PDF_PAGE_ORIENTATION_PORTRAIT);
		m_DocEngine.AddSummaryStatisticsPage(m_pTable);
	}//if

	delete pShapes;

}//method


void CSummarize::Summarize(){

	int i,j;

	CSG_Table *pShapesTable;
	CSG_Table_Record *pRecord;
	CSG_String sName;
	float *pSum;
	float *pMin;
	float *pMax;
	float *pVar;
	float fValue;
	float fMean;
	int iLastField = -1;
	int iField;
	int iParam;

	m_ClassesID.clear();

	pShapesTable = &m_pShapes->Get_Table();
	m_pClasses = new int[pShapesTable->Get_Record_Count()];

	for (i = 0; i < pShapesTable->Get_Record_Count(); i++){
		pRecord = pShapesTable->Get_Record(i);
		sName = pRecord->asString(m_iField);
		for (j = 0; j < m_ClassesID.size(); j++){
			if (!m_ClassesID[j].CmpNoCase(sName)){
				m_pClasses[i] = j;
				break;
			}//if
		}//for
		if (j == m_ClassesID.size()){
			m_pClasses[i] = j;
			m_ClassesID.push_back(sName);
		}//if
	}//for

	m_pTable->Create((CSG_Table*)NULL);
	m_pTable->Set_Name(_TL("Summary Table"));
	m_pTable->Add_Field(_TL("Class"), TABLE_FIELDTYPE_String);
	m_pTable->Add_Field(_TL("Count"), TABLE_FIELDTYPE_Int);

	m_pCount = new int[m_ClassesID.size()];
	pSum = new float[m_ClassesID.size()];
	pMax = new float[m_ClassesID.size()];
	pMin = new float[m_ClassesID.size()];
	pVar = new float[m_ClassesID.size()];

	for (i = 0; i < m_ClassesID.size(); i++){
		m_pCount[i] = 0;
	}//for

	for (i = 0; i < pShapesTable->Get_Record_Count(); i++){
		m_pCount[m_pClasses[i]]++;
	}//for

	for (i = 0; i < m_ClassesID.size(); i++){
		pRecord = m_pTable->Add_Record();
		sName = m_ClassesID[i];
		pRecord->Set_Value(0,sName.c_str());
		pRecord->Set_Value(1,m_pCount[i]);
	}//for

	for (i = 0; i < pShapesTable->Get_Field_Count() * 5; i++){
		if (m_bIncludeParam[i]){
			iField = (int) (i / 5);
			iParam = i % 5;
			sName = pShapesTable->Get_Field_Name(iField);
			sName.Append(_TL(sParam[iParam]));
			m_pTable->Add_Field(sName.c_str(), TABLE_FIELDTYPE_Double);
			if (iField != iLastField){
				for (j = 0; j < m_ClassesID.size(); j++){
					pSum[j] = 0;
					pMax[j] = -(float)VERY_LARGE_NUMBER;
					pMin[j] =  (float)VERY_LARGE_NUMBER;
					pVar[j] = 0;
				}//for
				for (j = 0; j < pShapesTable->Get_Record_Count(); j++){
					pRecord = pShapesTable->Get_Record(j);
					fValue = pRecord->asFloat(iField);
					pSum[m_pClasses[j]] += fValue;
					pVar[m_pClasses[j]] += (fValue * fValue);
					if (fValue > pMax[m_pClasses[j]]){
						 pMax[m_pClasses[j]] = fValue;
					}//if
					if (fValue < pMin[m_pClasses[j]]){
						 pMin[m_pClasses[j]] = fValue;
					}//if
				}//for
			}//if
			iField = m_pTable->Get_Field_Count() - 1;
			for (j = 0; j < m_ClassesID.size(); j++){
				pRecord = m_pTable->Get_Record(j);
				switch (iParam){
				case 0: //sum
					pRecord->Set_Value(iField, pSum[j]);
					break;
				case 1: //average
					pRecord->Set_Value(iField, pSum[j] / (float) m_pCount[j]);
					break;
				case 2: //variance
					fMean = pSum[j] / (float) m_pCount[j];
					pRecord->Set_Value(iField, pVar[j] / (float) m_pCount[j] - fMean * fMean);
					break;
				case 3: //min
					pRecord->Set_Value(iField, pMin[j]);
					break;
				case 4: //max
					pRecord->Set_Value(iField, pMax[j]);
					break;
				default:
					break;
				}//switch
			}//for

		}//if

	}//for


}//method

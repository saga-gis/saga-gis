
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                     imagery_rga                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                fast_region_growing.cpp                //
//                                                       //
//                   Copyright (C) 2009                  //
//                     Andre Ringeler                    //
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
//    e-mail:     aringel@saga-gis.org                   //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "fast_region_growing.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int	xTo[]={ 0, 1, 1, 1, 0,-1,-1,-1};
int	yTo[]={ 1, 1, 0,-1,-1,-1, 0, 1};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPtrArray::Add(void * newElement)
{
	if (m_size >= m_allocsize)
	{
		m_allocsize += 4;
		if (m_size == 0)
		{
			data = (void **) new int[4];
		}
		// else
		// realloc(data,m_allocsize*sizeof(void*));
	}
	data[m_size++] = newElement;
	return m_size;
}

//---------------------------------------------------------
void CPtrArray::RemoveAll()
{
	if (m_allocsize)
	{
		m_size = 0;
		m_allocsize = 0;
		delete	data; 
	}
};

//---------------------------------------------------------
void *** PointerMatrix(int NX, int NY)
{
	int i;
	void *** ar;
	
	ar = (void ***) malloc(NY*sizeof(void **));
	
	for (i = 0; i < NY; i++)
	{
		ar[i] = (void **) malloc(NX*sizeof(void *));
	}
	
	return (ar);
}

//---------------------------------------------------------
void FreePointerMatrix(void *** data, int NX  , int NY)
{
	int i;
	for (i = 0; i < NY; i++)
		free(data[i]);
	
	free(data);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCluster::CCluster()
{}

//---------------------------------------------------------
void CCluster::Create(int _dim)
{
    dim = _dim;    
	DataAscent = (TDataNodeList *) new TDataNodeList[dim];
}

//---------------------------------------------------------
CCluster::~CCluster()
{
	delete[] DataAscent;
}

//---------------------------------------------------------
void CCluster::Add(CDataNode ** node, int dim)
{
	(*node)->sortdim = dim;

	DataAscent[dim].InsertSorted(*node);
}

//---------------------------------------------------------
void CCluster::Del(CDataNode * node, int dim)
{
	DataAscent[dim].Remove(node);
}

//---------------------------------------------------------
CDataNode * CCluster::GetDataHead(int dim)
{
	return DataAscent[dim].GetHead();
}

//---------------------------------------------------------
float CCluster::Distance(CData *data)
{
	float	sum;
	int		j;
	float	val;
	
	BYTE	bitmask;
	float	max;
	
	RGADATA *vals = data->Values;
	max = -9999999.9f;
	
	for (bitmask = 0; bitmask < (2 <<(dim - 1)); bitmask++)
	{
		sum = 0;
		
		for (j = 0; j < dim; j++)
		{
			CDataNode * min_node = DataAscent[j].GetHead();
			CDataNode * max_node = DataAscent[j].GetLast();
			
			val = ((BYTE)(bitmask & (1 << j)) ? max_node->Data->Values[j]:min_node->Data->Values[j])-  vals[j];
			
			sum +=val*val;
		}
		
		if (sum > max)
		{
			max = sum;
		}
	}
	return  max;
}

//---------------------------------------------------------
float CCluster::GetMaxVal(int dim)
{
	CDataNode * head = DataAscent[dim].GetLast();
	
	return head->Data->Values[dim];
}

//---------------------------------------------------------
float CCluster::GetMinVal(int dim)
{
	CDataNode * head = DataAscent[dim].GetHead();
	return head->Data->Values[dim];
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFast_Region_Growing::CFast_Region_Growing(void)
{
	Set_Name		(_TL("Fast Region Growing Algorithm"));

	Set_Author		(SG_T("A.Ringeler (c) 2009"));

	Set_Description	(_TW(
		"A fast region growing algorithm.\n"
		"\n"
		"References:\n"
		"Boehner, J., Selige, T., Ringeler, A. (2006): Image segmentation using representativeness analysis and region growing. "
		"In: Boehner, J., McCloy, K.R., Strobl, J. [Eds.]:  SAGA – Analysis and Modelling Applications. "
		"Goettinger Geographische Abhandlungen, Vol.115, "
		"<a href=\"http://downloads.sourceforge.net/saga-gis/gga115_03.pdf\">pdf</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL, "INPUT"	, _TL("Input Grids"),
		_TL(""),
		PARAMETER_INPUT
	);
	
	Parameters.Add_Grid(
		NULL, "START"	, _TL("Seeds Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "REP"		, _TL("Smooth Rep"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);
	
	Parameters.Add_Grid(
		NULL, "RESULT"	, _TL("Segmente"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL, "MEAN"	, _TL("Mean"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFast_Region_Growing::On_Execute(void)
{
	pResult		= Parameters("RESULT")->asGrid();

	pMean		= Parameters("MEAN")->asGrid();
	
	nGrids		= Parameters("INPUT")->asInt();
	
	Grids		=(CSG_Grid **)Parameters("INPUT")->asPointer();
	
	CSG_Grid *StartGrid = Parameters("START")->asGrid();

	CSG_Grid *RepGrid = Parameters("REP")->asGrid();

	RepGrid = NULL;
	
	Process_Set_Text(_TL("Init RGA"));
	
	InitData((PGrid *)Grids, nGrids, StartGrid);
	
	Process_Set_Text(_TL("RGA"));
	
	RGA();
	
	Process_Set_Text(_TL("Clear RGA"));
	
	GetClusterGrid(pResult);

	Get_Mean_Grid();
	
	KillData();
	
	return (true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFast_Region_Growing::Get_Mean_Grid()
{
	int x,y;
	int n_seg;
	int *nr_elements;
	double *sum;

	n_seg = (int) pResult->Get_ZMax();	

	nr_elements = (int *) new int[n_seg+1];

	sum = (double *) new double[n_seg+1];

	for (x= 0; x <= n_seg+1; x++ )
	{
		sum[x]=0.0;
		nr_elements[x] = 0;

	}

	for ( y = 0; y < NY; y++)
	{
		for ( x = 0; x < NX; x++)
		{
			int pos = pResult->asInt(x,y);
			if(pos >= 0)
			{
				nr_elements[pos]++;
				sum[pos] += Grids[0]->asDouble(x,y);
			}
		
		}
	}

	for ( y = 0; y < NY; y++)
	{
		for ( x = 0; x < NX; x++)
		{
			int pos = pResult->asInt(x,y);
			if(pos >= 0)
			{
			
				pMean->Set_Value(x,y,sum[pos]/nr_elements[pos]);
			}
		
		}
	}
	
}


void CFast_Region_Growing::GetClusterGrid(CSG_Grid *OutGrid)
{
	for (int y = 0; y < NY; y++)
	{
		for (int x = 0; x < NX; x++)
		{
			OutGrid->Set_Value(x, y, pDataGrid[y][x]->ClusterNr);
			
		}
	}
}

void CFast_Region_Growing::InitData(CSG_Grid **InGrid, int dim, CSG_Grid *StartGrid)
{
	NX = InGrid[0]->Get_NX();
	NY = InGrid[0]->Get_NY();
	
	m_dim = dim;
	
	BounderyList = (TBounderyNodeList *) new TDataNodeList(false);
	
	pClusterGrid = (PPCDataNode **)  new PPCDataNode[dim];
	
	pRandGrid    = (PCBounderyArray **)  PointerMatrix(NX, NY);
int		i;	
	for (i = 0; i < dim; i++)
	{
		pClusterGrid[i] = (PCDataNode **) PointerMatrix(NX, NY);
    }
	
	pDataGrid = (PCData **) PointerMatrix(NX, NY);
    
    m_NrCluster = 0;
int		y;	
	for (y = 1; y < NY - 1; y++)
	{
		for (int x = 1; x < NX - 1; x++)
		{
			if (!StartGrid->is_NoData(x, y) && StartGrid->asInt(x, y) > 0)
			{
				m_NrCluster++;
			}
		}
	}
	
	Cluster = (CCluster *) new CCluster[m_NrCluster + 1];
	
	for (i = 0; i < m_NrCluster + 1; i++)
	{
		Cluster[i].Create(dim);        
	}
	
	BounderyNodes = (CBounderyNode *) new CBounderyNode[NX*NY];
	
	last = (float)(dim*LAST);
	
	for (y = 0; y < NY*NX; y++)
	{
		BounderyNodes[y].distance = (float)last;
		last += LASTINC;
		BounderyList->InsertSorted(&BounderyNodes[y]);
	}
int		x, k;	
	for (y = 0; y < NY; y++)
	{       
		for (x = 0; x < NX; x++)
		{
			pRandGrid[y][x] = (PCBounderyArray) new CPtrArray;
			RGADATA *tmp;
			tmp = (RGADATA *) new RGADATA[dim];
			
			for (k = 0; k < dim; k++)
			{
				tmp[k] = InGrid[k]->asFloat(x, y)+ ((float)rand() / (float)RAND_MAX / 100.0f);
			}
			
			pDataGrid[y][x]=  new CData(x, y, - 1, tmp);
			
			for (k = 0; k < dim; k++)
			{
				pClusterGrid[k][y][x] = (PCDataNode) new CDataNode(pDataGrid[y][x], k);
			}
		}
	}
	
	m_NrCluster=-1;
	
	for (y = 1; y < NY - 1; y++)
	{
		for (x = 1; x < NX - 1; x++)
		{
			if (!StartGrid->is_NoData(x, y) && StartGrid->asInt(x, y) > 0)
			{
				m_NrCluster++;
				
				AddClusterPoint(x, y, m_NrCluster);
				
				AddBounderyPoints(x, y, m_NrCluster);
			}
		}
	}
}

void CFast_Region_Growing::KillData()
{int y, x, i;
	for (y = 0; y < NY; y++)
	{
		for (x = 0; x < NX; x++)
		{	
			pRandGrid[y][x]->RemoveAll();
			delete	pRandGrid[y][x];
		}
	}		
	delete BounderyList;
	delete[]	Cluster;
	
	for (y = 0; y < NY; y++)
	{
		for (x = 0; x < NX; x++)
		{
			delete pDataGrid[y][x];
		}
	}	
	delete[] BounderyNodes;
	
	FreePointerMatrix((void ***)pRandGrid,  NX  , NY);
	FreePointerMatrix((void ***)pDataGrid,  NX  , NY);
	
	for (i = 0; i < m_dim; i++)
	{
		FreePointerMatrix((void ***)pClusterGrid[i], NX, NY);
	}
}

inline void CFast_Region_Growing::AddBounderyNode(int x, int y, int cluster)
{
	CBounderyNode * Bound = BounderyList->GetLast();

	BounderyList->Remove(Bound);
	
	Bound->Data	= pDataGrid[y][x];
	
	Bound->ClusterNr = cluster;
	
	Bound->distance	= Cluster[cluster].Distance(Bound->Data);

	//if(RepGrid)
	//	Bound->distance/=(1.0+	 RepGrid->asFloat(Bound->Data->x,Bound->Data->y)/1000.0);
	///(1+	 mRepGrid->asFloat(Bound->Data->x,Bound->Data->y));
	
	pRandGrid[y][x]->Add(Bound);
	
	BounderyList->InsertSorted(Bound);
}

inline void CFast_Region_Growing::DelBounderyNode(CBounderyNode * Node)
{       
	BounderyList->Remove(Node);
	Node->distance = (float)(last += LASTINC);
	
	BounderyList->InsertSorted(Node);
}

inline void CFast_Region_Growing::DelBounderyPoints(int x, int y)
{
	for (int i = 0; i < pRandGrid[y][x]->GetSize(); i++)
	{
		CBounderyNode * Node = (CBounderyNode *) pRandGrid[y][x]->GetAt(i);
		DelBounderyNode(Node);
	}
	pRandGrid[y][x]->RemoveAll();
}

inline void CFast_Region_Growing::AddClusterPoint(int x, int y, int cluster)
{
	pDataGrid[y][x]->ClusterNr = cluster;
	
	for (int k = 0; k < m_dim; k++)
		Cluster[cluster].Add(&pClusterGrid[k][y][x], k);
}

inline void CFast_Region_Growing::AddBounderyPoints(int x, int y, int cluster)
{
	for (int i = 0; i < 8; i += 2)
	{
		int posx = x + xTo[i]; 
		int posy = y + yTo[i]; 
		
		if (pDataGrid[posy][posx]->ClusterNr == -1)
		{
			AddBounderyNode(posx, posy, cluster);
		}
	}
}

void CFast_Region_Growing::RGA()
{	
	int count = 0;
	
	CBounderyNode * Node= BounderyList->GetHead();
	
	while (Node && Node->distance < m_dim*LAST)
	{
		int x = Node->Data->x;
		int y = Node->Data->y;
		
		if (!(x>1 && y > 1 && x<NX - 1 && y < NY - 1))   
		{       
			DelBounderyNode(Node);
			Node= BounderyList->GetHead();
			continue; 
		}
		
		if (pDataGrid[y][x]->ClusterNr == -1)
		{
			AddClusterPoint(x, y, Node->ClusterNr);
			
			DelBounderyPoints(x, y);
			
            AddBounderyPoints(x, y, Node->ClusterNr);
			
			Node= BounderyList->GetHead();  
			
			count ++;
			
			if (!(count%1000))
			{
				Set_Progress(count, NX*NY);
			}
        }
        else
        {
            DelBounderyNode(Node);
            Node= BounderyList->GetHead();
        }
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFast_Region_Growing::Histgram(char * FileName)
{
/*	CSG_Grid Mittel(m_dim, m_NrCluster + 1, dtFloat);
CSG_Grid Anzahl(m_dim, m_NrCluster + 1, dtInt);
FILE * out = fopen((LPCSTR)FileName, "wt");

  for (int c = 0; c < m_NrCluster; c++)
  {	
		fprintf(out, "%d\t", c);
		for (int k = 0; k < m_dim; k++)
		{
		int hst[256];
		int hstsum = 0;
		
		  for (int i = 0; i < 256; i++)
		  hst[i] = 0;
		  
			float min, max; 
			
			  CDataNode * Data =Cluster[c].GetDataHead(k);
			  
				min = max = Data->Data->Values[k];
				Data = Data->Next;
				while (Data) 
				{
				if (Data->Data->Values[k]>max)
				max = Data->Data->Values[k];
				
				  if (Data->Data->Values[k]<min)
				  min = Data->Data->Values[k];
				  
					Data = Data->Next;
					}
					
					  
						Data =Cluster[c].GetDataHead(k);
						while (Data) 
						{
						hst[int((Data->Data->Values[k] - min)/ (max - min)*255)]++;
						
						  Data = Data->Next;
						  }
						  
							for (i = 0; i < 256; i++)
							hstsum += hst[i];
							
							  int sum = 0;
							  i = 0;
							  while ((float)sum < 0.1* (float)hstsum)
							  {
							  if (i > 255)
							  break;
							  sum += hst[i++];
							  }
							  
								fprintf(out, "%f ", (float)(i* (max - min))/255.0 + min);		
								
								  while ((float)sum < 0.5* (float)hstsum)
								  {
								  if (i > 255)
								  break;
								  sum += hst[i++];
								  }
								  
									fprintf(out, "%f ", (float)(i* (max - min))/255.0 + min);		
									
									  while ((float)sum < 0.9* (float)hstsum)
									  {
									  if (i > 255)
									  break;
									  sum += hst[i++];
									  }
									  fprintf(out, "%f ", (float)(i* (max - min))/255.0 + min);		
									  }
									  fprintf(out, "\n");
									  }
									  
	fclose(out); */
}

//---------------------------------------------------------
void CFast_Region_Growing::Statistik(char * FileName)
{
/*	CSG_Grid Mittel(dtFloat, m_dim, m_NrCluster + 1);
CSG_Grid Anzahl(dtInt, m_dim, m_NrCluster + 1);

  CSG_Grid Var(dtFloat, m_dim, m_NrCluster + 1);
  
	float **	pVar	=(float **)Var.Get_Array();
	
	  float **	pMittel = (float **)Mittel.Get_Array();
	  
		int **      pAnzahl = (int **)  Anzahl.Get_Array();
		
		  for (int y = 0; y < NY; y++)
		  {       
		  for (int x = 0; x < NX; x++)
		  {
		  int Clu = ((CData *) pDataGrid[y][x])->ClusterNr;
		  
			if (Clu >= 0)
			for (int k = 0; k < m_dim; k++)
			{
			pMittel[Clu][k] = pMittel[Clu][k] + pDataGrid[y][x]->Values[k];
			
			  pVar[Clu][k] = pVar[Clu][k] + (pDataGrid[y][x]->Values[k]*pDataGrid[y][x]->Values[k]);
			  
				pAnzahl[Clu][k] += 1;	
				}
				}
				}
				
				  for (int c = 0; c < m_NrCluster; c++)
				  for (int k = 0; k < m_dim; k++)
				  {
				  pMittel[c][k] = pMittel[c][k]/pAnzahl[c][k];
				  
					pVar[c][k] = (pVar[c][k] - pMittel[c][k]*pMittel[c][k]*pAnzahl[c][k])/pAnzahl[c][k];
					}
					
					  FILE * out = fopen((LPCSTR)FileName, "wt");
					  
						for (c = 0; c < m_NrCluster; c++)
						{
						fprintf(out, "%d\t", c);
						for (int k = 0; k < m_dim; k++)
						{
						fprintf(out, "%d\t%f\t%f\t", pAnzahl[c][k], pMittel[c][k], pVar[c][k]);		
						}
						fprintf(out, "\n");
						}
	fclose(out); */
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

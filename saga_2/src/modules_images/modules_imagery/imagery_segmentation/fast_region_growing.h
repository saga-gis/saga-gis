
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                 imagery_segmentation                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 fast_region_growing.h                 //
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
#ifndef HEADER_INCLUDED__Region_Growing_H
#define HEADER_INCLUDED__Region_Growing_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define RGADATA	float
#define LAST		1.0e20f
#define LASTINC	1.0e19f


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef unsigned int DW;

template <class T>
class SListNode {
	public:
	T		*		Prev;
	T		*		Next;

	SListNode()
	{
		Prev=Next=NULL;
	};	
	
	virtual ~SListNode()
	{
		if (Prev || Next) Remove();
	}; 

	bool	InsertThisBetween(T * before,T * after);
	bool	Remove(T * previous=NULL);		
};


template <class T>
inline bool	SListNode<T>::InsertThisBetween(T * before,T * after)
{
	if (before) before->Next = (T*)this;
	if (after)	after->Prev=(T*)this;
	Next=after; Prev=before;
	
	return true;
};


template <class T>
inline bool	SListNode<T>::Remove(T * previous)
{
	if (Prev) Prev->Next=Next;
	if (Next) Next->Prev=Prev;
	Next=Prev=NULL;
	
	return true;
};

enum QSL_SortTypes 
{
	LSort_Ascent,	
	LSort_Ascent_Find,	
};

template <class TBottomNode>
class QTopNode : public SListNode<QTopNode<TBottomNode> > 
{
public:
	DW						SubNodesNum;    
	QTopNode	*			Parent;			
	QTopNode	*			Child;			
	TBottomNode *			BottomNode;		

	QTopNode()
	{ 
		Child =NULL; SubNodesNum = 0;
		BottomNode = NULL; Parent = NULL;
	}
	
	virtual ~QTopNode()
	{ 
		if (Prev || Next || Parent || Child) Remove();
	}

	TBottomNode *	GetInsertionPoint(TBottomNode * node,QTopNode ** parent,TBottomNode ** insert_after,QSL_SortTypes sorttype);
	TBottomNode *	GetBottomInsertionPoint(TBottomNode * start,TBottomNode * node,QTopNode ** parent,TBottomNode ** insert_after,QSL_SortTypes sorttype);

	bool			InsertThisBetween(QTopNode * before,QTopNode * after);
	bool			Remove();			

	bool			SetChild(QTopNode* child);
	bool			SetBottomNode(TBottomNode * node);

	QTopNode *		GetChild(DW num);
	TBottomNode *	GetBottomNode(DW num);
	QTopNode *		GetParent();
	QTopNode *		GetTopNode();	

	bool			SubNodeInserted(void * node);
	bool			SubNodeRemoved(void * node);

};

#define QTTopNode QTopNode<TBottomNode>

template <class TBottomNode>
inline QTTopNode *	QTTopNode::GetParent()
{
	QTopNode * cur = this;
	while(cur)
	{
		if (cur->Parent) return  cur->Parent;
		cur=cur->Prev;
	}
	return NULL;
};


#define QTOPNODES_MIN 3
#define QTOPNODES_MAX 9
#define QBOTTOMNODES_MIN 10
#define QBOTTOMNODES_MAX 30


template <class TBottomNode>
inline bool	QTTopNode::SubNodeInserted(void * node)
{
	SubNodesNum++;
	if (!BottomNode)
	{
		BottomNode = (TBottomNode*)node;
	}
	if (!Child && ((TBottomNode*)node)->Next == BottomNode)
	{
		SetBottomNode((TBottomNode*)node);
	};
	
	DW max_num = Child ? QTOPNODES_MAX : QBOTTOMNODES_MAX;
	
	if (SubNodesNum >= max_num)
	{
		QTopNode * new_node = new QTopNode;
		new_node->InsertThisBetween(this,Next);
	}
	return true;
};

template <class TBottomNode>
inline 	bool QTTopNode::SubNodeRemoved(void * node)
{
	SubNodesNum--;
	if (!Child && ((TBottomNode*)node) == BottomNode)
	{
		SetBottomNode(BottomNode->Next);
	};
	if (Child == (QTTopNode*)node)
	{
		SetChild(Child->Next);
	} 
	DW min_num = Child ? QTOPNODES_MIN : QBOTTOMNODES_MIN;
	
	bool top_node = !Prev && !Parent;
	
	if ((!top_node && SubNodesNum < min_num )
	|| (top_node && SubNodesNum == 1 && Child))
	{
		if (!Prev)
		{ 
			if (Next)
			{
				QTTopNode * parent = GetParent();
				if (parent) 
					parent->SubNodeRemoved(Next);
				
				Next->Remove();
			}
			else
			{
				if (Child) Remove();
			}
		}
		else
		{					
			QTTopNode * parent = GetParent();
			if (parent) parent->SubNodeRemoved(this);
			Remove();
		}
	}
	return true;
};


template <class TBottomNode>
inline bool	QTTopNode::Remove()
{
	if (Prev)
	{
		Prev->SubNodesNum += SubNodesNum;
	}

	SetChild(NULL);
	SubNodesNum = 0;
	BottomNode = NULL;

	if (!SListNode<QTTopNode >::Remove())
		return false;
	
	delete this;
	return true;
}

template <class TBottomNode>
inline bool	QTTopNode::InsertThisBetween(QTTopNode * before,QTTopNode * after)
{
	if (!SListNode<QTTopNode >::InsertThisBetween(before,after))
		return false;


	SubNodesNum=Prev->SubNodesNum/2;
	Prev->SubNodesNum -= SubNodesNum;
	if (Prev->Child)
	{
		SetChild(Prev->GetChild(Prev->SubNodesNum));
		BottomNode = Child->BottomNode;
	}
	else
	{
		BottomNode = Prev->GetBottomNode(Prev->SubNodesNum);
	}

	QTTopNode * parent = GetParent();
	if (!parent)
	{ 
		if (!Prev && !Next)	
			return true;
	
		parent = new QTTopNode;
		parent->SubNodesNum++;
		parent->SetChild(Prev);
	}
	parent->SubNodeInserted(this);

	return true;
};

template <class TBottomNode>
inline QTTopNode *	QTTopNode::GetChild(DW num)
{
	QTTopNode * cur = Child;
	while(num--)
	{
		if (!cur) return NULL;
		cur = cur->Next;
	}
	return cur;
};

template <class TBottomNode>
inline TBottomNode * QTTopNode::GetBottomNode(DW num)
{
	TBottomNode * cur = BottomNode;
	while(num--)
	{
		if (!cur) 
			return NULL;
		
		cur = cur->Next;
	}
	return cur;
};

template <class TBottomNode>
inline bool		QTTopNode::SetChild(QTopNode* child)
{
	if (Child)
	{
		Child->Parent = NULL; Child = NULL;
	}
	if (!child) 
		return true;

	if (child->Parent) 
		child->Parent->Child = NULL;

	Child=child; child->Parent=this; 
	SetBottomNode(Child->BottomNode);

	return true;
};

template <class TBottomNode>
inline bool	QTTopNode::SetBottomNode(TBottomNode * node)
{
	QTTopNode * parent = this;
	while(parent)
	{ 
		parent->BottomNode=node;
		parent = parent->Parent;
	}

	return true;
};

template <class TBottomNode>
inline QTTopNode *	QTTopNode::GetTopNode()
{
	QTopNode * cur = this;
	if (!Prev)
	{
		while(cur->Parent) cur = cur->Parent;
	}
	else
	{
		while(1)
		{
			QTTopNode * parent = cur->GetParent();
			if (!parent) break;
			cur=parent;
		}
	}


	return cur;
};


template <class TBottomNode>
inline TBottomNode *	QTTopNode::GetInsertionPoint(TBottomNode * node,QTTopNode ** parent,
									TBottomNode ** insert_after, QSL_SortTypes sorttype)
{
	QTTopNode * cur  = this;
	TBottomNode * bottom;
//------------------------------------------------------------
#define FIND_ALGORITHM(condition) \
				{QTTopNode * prev=cur->Prev;\
				while(1){\
				bottom = cur->BottomNode;\
				if (condition){\
					if (!prev){ *parent = (QTTopNode *)0xFFFFFFFF; return bottom;}\
							QTTopNode * child = prev->Child;\
							if (child)\
								return child->Next->GetInsertionPoint(node,parent,insert_after,sorttype);\
							else {\
								*parent = prev;\
								return prev->GetBottomInsertionPoint(prev->BottomNode,node,parent,insert_after,sorttype);}\
							}\
				prev = cur;\
				cur = cur->Next;\
				if (!cur){ cur=prev; break;}\
				}}\
				break;
//------------------------------------------------------------


	switch(sorttype)
	{
 		case LSort_Ascent:			FIND_ALGORITHM((*node < *bottom));
 		case LSort_Ascent_Find: 	FIND_ALGORITHM((*node < *bottom));
	}			
#undef FIND_ALGORITHM

	QTTopNode * child = cur->Child;
	if (child)
		return child->GetInsertionPoint(node,parent,insert_after,sorttype);
	else
	{
		*parent = cur;
		return cur->GetBottomInsertionPoint(cur->BottomNode,node,parent,insert_after,sorttype);
	}
}




template <class TBottomNode>
inline TBottomNode *	QTTopNode::GetBottomInsertionPoint(TBottomNode * start,TBottomNode * node,
									QTTopNode ** parent,
									TBottomNode ** insert_after,QSL_SortTypes sorttype)
{
	TBottomNode * cur = start;
	TBottomNode * prev = NULL;
	DW i = 0;
//------------------------------------------------------------
#define FIND_ALGORITHM(condition) \
			while(cur){\
				if (condition) break;\
				prev = cur; cur = cur ->Next;};\
				break;
//------------------------------------------------------------
	switch(sorttype)
	{
 		case LSort_Ascent:			FIND_ALGORITHM((*node < *cur));
 		case LSort_Ascent_Find: 	FIND_ALGORITHM((*node < *cur));
	}			
#undef FIND_ALGORITHM
	*insert_after = prev;
	if (Next && Next->BottomNode==cur)
		*parent=Next;
	
	return cur;
};

template <class T> class SortList
{

	QTopNode<T> *		TopNode;
	QSL_SortTypes		SortType;
	bool				AutoDelete;
	T * Last;

	T *		FindInsertionPoint(T * node,QTopNode<T> ** parent,
							T ** insert_after,QSL_SortTypes sorttype);

	public:

		SortList(bool autodelete=true);
		virtual ~SortList();
		
		bool	InsertSorted( T * node);
		
		bool	Remove(T* node);

		T *		GetHead()
				{ 
					return TopNode->BottomNode;
				};
		T *		GetLast()
				{ 
					return Last;
				};

		T *		RemoveHead(bool and_delete = false);

	
};

template <class T>
inline	SortList<T>::SortList<T>(bool autodelete)
{
	AutoDelete = autodelete;
	TopNode = new QTopNode<T>;
	SortType = LSort_Ascent;
	Last=NULL;
};

template <class T>
inline	SortList<T>::~SortList<T>()
{
	while(RemoveHead(AutoDelete));
	delete TopNode;
	TopNode = NULL;
	return;
};


template <class T>
inline	T * SortList<T>::FindInsertionPoint(T * node,QTopNode<T> ** parent,T ** insert_after,QSL_SortTypes sorttype)
{
	if (!TopNode->BottomNode)
	{ 
		*insert_after = NULL;
		*parent=TopNode;
		return NULL;
	}
	return (T*)TopNode->GetTopNode()->GetInsertionPoint(node,parent,(T**)insert_after,sorttype);
}

template <class T>


inline bool SortList<T>::InsertSorted(T * node)
{
	QTopNode<T> * parent=NULL;
	
	T * insert_after=NULL;
	T * insertion_point = FindInsertionPoint(node,&parent,&insert_after,SortType);

	if (parent == (QTopNode<T> *)0xFFFFFFFF)
		parent = TopNode;
	if (!insertion_point)
	{
		 node->InsertThisBetween(insert_after,NULL);
	}
	else
	{ 
		 node->InsertThisBetween(insert_after,insertion_point);
	}
	parent->SubNodeInserted(node);

	if (Last==NULL) Last=node;
	if (*Last<*node) Last=node;
	
	return true;
};

template <class T>
inline bool SortList<T>::Remove(T * node)
{
	if(!node)
		return false;

	if (node==Last) Last=node->Prev;
	QSL_SortTypes sorttype = LSort_Ascent_Find;

	QTopNode<T> *	parent=NULL;
	T	*		prev=NULL;
	T	*		current = FindInsertionPoint(node,&parent,&prev,sorttype);
	
	if (!current)
	{ 
	
		return false;
	}

	if (parent == (QTopNode<T> *)0xFFFFFFFF)
		parent = TopNode;
	
	T	*		next_child = parent->Next ? parent->Next->BottomNode : NULL;
	
	while(1)
	{

		if (current==node)
			break;
		
		prev = current;
		current = current->Next;
	
		
		if (current==next_child)
		{
			 parent=parent->Next;
			 next_child = parent->Next ? parent->Next->BottomNode : NULL;
		}
	}

	parent->SubNodeRemoved(current);
	current->Remove(prev);

	return true;
};

template <class T>
inline	T *	SortList<T>::RemoveHead(bool and_delete)
{
	T * head = TopNode->BottomNode;
	if (!head)
		return NULL;
	
	TopNode->SubNodeRemoved(head);
	head->Remove(NULL);
	if (and_delete)
	{
		delete head;
		return (T*)0xFFFFFFFF;
	}

	return head;
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCluster;
class CRGA;

//---------------------------------------------------------
class CPtrArray
{
public:
	
	CPtrArray()
	{
		m_size=0;
		m_allocsize=0;
		
	}

	int Add(void * newElement);
	int GetSize() {return m_size;};
	void *GetAt(int i) {return data[i];};	
	void RemoveAll();

private:
 
	int	m_size;
	int m_allocsize;
    void ** data;
};

//---------------------------------------------------------
class CData
{
public:
	CData(int _x, int _y, int _ClusterNr, RGADATA * _Values)
	{
		x = _x;
		y = _y;
		ClusterNr= _ClusterNr;
		Values = _Values;
	}
	
	~CData()
	{
		delete[] Values;
	}
	
	RGADATA * Values;
	short int x, y;
	int  ClusterNr;
};

//---------------------------------------------------------
class CDataNode : public SListNode <CDataNode>
{
public:
	
    char sortdim;
	CData * Data;
	
    CDataNode(CData * _Data, int _sortdim)
    {
        Data = _Data;
		sortdim = _sortdim;	
    }
    bool operator >(CDataNode& comp){return Data->Values[sortdim] > comp.Data->Values[sortdim];}
    bool operator <(CDataNode& comp){return Data->Values[sortdim] <= comp.Data->Values[sortdim];}
};

//---------------------------------------------------------
class CBounderyNode : public SListNode <CBounderyNode>
{
public:
	float	distance;       
	
	int ClusterNr;
	
	CData * Data;
	
	CBounderyNode::CBounderyNode(CData * _Data)
	{
		Data = _Data;
	}
	
	CBounderyNode()
	{
		distance= LAST;
	};
	
	bool operator >(CBounderyNode& comp){return distance > comp.distance;}
	bool operator <(CBounderyNode& comp){return distance <= comp.distance;}
};

//---------------------------------------------------------
typedef SortList		<CDataNode>		TDataNodeList;
typedef SortList		<CBounderyNode>	TBounderyNodeList;

typedef CDataNode 		*PCDataNode;
typedef CData 			*PCData;

typedef	PCDataNode		*PPCDataNode;

typedef CBounderyNode	*PCBounderyNode;
typedef CPtrArray		*PCBounderyArray;
typedef RGADATA			**ppRGADATA;
typedef CSG_Grid		*PGrid;

//---------------------------------------------------------
class CCluster
{ 
public:
    CCluster();
    ~CCluster();
	void Create(int _dim);
    void Add(CDataNode ** node, int dim);
    void Del(CDataNode * node, int dim);
	
	CDataNode * GetDataHead(int dim);
    
	float Distance(CData *data); 
	float GetMinVal(int dim);
	float GetMaxVal(int dim);
	int dim;
	
private:
    TDataNodeList *DataAscent;
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CFast_Region_Growing : public CSG_Module_Grid
{
public:
	CFast_Region_Growing(void);

	void Get_Mean_Grid(void);

	void RGA();
	void GetClusterGrid(CSG_Grid *OutGrid);
//	GetValGrid(CSG_Grid *OutGrid);
	void Statistik(char * FileName);		
	void Histgram(char * FileName);
	
	

protected:

	virtual bool		On_Execute(void);


private:

	void KillData();

	void InitData(CSG_Grid **InGrid, int dim, CSG_Grid *StartGrid);
	
	int					nGrids;

	CSG_Grid				**Grids;

	CSG_Grid				*pResult, *pMean;

	CSG_Grid				*RepGrid;
		int m_NrCluster;
	
	CCluster	*Cluster;

	CSG_Grid *mRepGrid;

	TBounderyNodeList	*BounderyList; 
	
	PCBounderyArray		**pRandGrid;
	
	PPCDataNode			**pClusterGrid;
	
	PCData				**pDataGrid;
	
	float last;
	int m_dim;

	int NX,NY;
	
    void AddBounderyNode(int x, int y, int cluster);
	
    void DelBounderyNode(CBounderyNode * Node);
	
	void DelBounderyPoints(int x, int y);
	
	void AddBounderyPoints(int x, int y, int cluster);
	
	void AddClusterPoint(int x, int y, int cluster);	
	
	CBounderyNode *BounderyNodes;


	
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Region_Growing_H

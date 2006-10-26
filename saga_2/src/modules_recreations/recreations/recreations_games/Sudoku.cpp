#include "Sudoku.h"
//#include "Sudoku_res.h"
int	numbers[10][36][36];

#define BOARD_SIZE 354
#define BLOCK_SIZE 119
#define CELL_SIZE 38

#define CELL_COLOR_BLACK 0
#define CELL_COLOR_WHITE 1
#define CELL_COLOR_YELLOW 2
#define CELL_COLOR_BROWN 3
#define CELL_COLOR_GREEN 4
#define CELL_COLOR_RED 5

unsigned int cell_color[] =
{
	SG_GET_RGB(0,  0,  0), // 0 = black
	SG_GET_RGB(255,  255,  255), // 1 = white
	SG_GET_RGB(250, 250, 150), // 2 = yellow
	SG_GET_RGB(200, 150, 100), // 3 = browm
	SG_GET_RGB(0,  220, 0), // 4 = green	
	SG_GET_RGB(200, 0 ,0) // 5 = red
};

CSudoku::CSudoku(void)
{
	Set_Name	(_TL("Sudoku"));

	Set_Author	(_TL("Copyrights (c) 2006 by Victor Olaya"));

	Set_Description("");

	Parameters.Add_Grid_Output(
		NULL	, "GRID"	, _TL("Grid"),
		""
	);

	int				i, j;
	CSG_Table			*pBoard;
	CSG_Table_Record	*pRecord;

	pBoard	= Parameters.Add_FixedTable(
		NULL	, "BOARD"	, _TL("Board"),
		""
	)->asTable();

	for (i = 0; i < 9; i++){
		pBoard->Add_Field("", TABLE_FIELDTYPE_Int);
	}

	for (i = 0; i < 9; i++){
		pRecord	= pBoard->Add_Record();
		for (j = 0; j < 9; j++){
			pRecord->Set_Value(j, 0.);
		}
	}
}

CSudoku::~CSudoku(void)
{}

bool CSudoku::On_Execute(void)
{
	int		i;
	CSG_Colors	Colors;
	
	m_pSudoku		= new int  * [9];
	m_pFixedCells	= new bool * [9];
	for (i = 0; i < 9; i++){
		m_pSudoku[i]		= new int [9];
		m_pFixedCells[i]	= new bool[9];
	}

	m_pBoard = SG_Create_Grid(GRID_TYPE_Int, BOARD_SIZE, BOARD_SIZE, 1);
	m_pBoard->Set_Name("Sudoku");
	Parameters("GRID")->Set_Value(m_pBoard);

	Colors.Set_Count(6);
	for (i = 0; i < 6; i++){
		Colors.Set_Color(i, cell_color[i]);
	}
	DataObject_Set_Colors(m_pBoard, Colors);
	DataObject_Update(m_pBoard, true); 

	CreateSudoku();
	DrawBoard();

	return true;
}

bool CSudoku::On_Execute_Finish(void)
{
	for (int i = 0; i < 9; i++){
		delete [] m_pSudoku[i];
		delete [] m_pFixedCells[i];
	}

	delete [] m_pSudoku;
	delete [] m_pFixedCells;

	return( true );
}

void CSudoku::CreateSudoku()
{
	CSG_Table *pTable;
	int i,j;
	int iValue;

	pTable = Parameters("BOARD")->asTable(); 
	for (i = 0; i < 9; i++){
		for (j = 0; j < 9; j++){
			iValue = pTable->Get_Record(i)->asInt(j);
			if (iValue > 0 && iValue < 10){
				m_pSudoku[i][j] = iValue;
				m_pFixedCells[i][j] = true;
			}
			else{
				m_pSudoku[i][j] = 0;
				m_pFixedCells[i][j] = false;
			}

		}
	}
}


bool CSudoku::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{

	int iXGrid, iYGrid;
	int iXSudoku, iYSudoku;
	int iBlock, iCell;
	bool	pIsPossible[10];

	if(	Mode != MODULE_INTERACTIVE_LDOWN && Mode != MODULE_INTERACTIVE_RDOWN) {
		return false;
	}

	if (!Get_Grid_Pos(iXGrid, iYGrid) ){
		return false;
	}

	iBlock	= (int)floor((double)iXGrid / (double)BLOCK_SIZE);
	iCell	= (int)floor((double)iXGrid - (iBlock * BLOCK_SIZE)) / CELL_SIZE;
	iXSudoku = iBlock * 3 + iCell;

	iBlock	= (int)floor((double)iYGrid / (double)BLOCK_SIZE);
	iCell	= (int)floor((double)iYGrid - (iBlock * BLOCK_SIZE)) / CELL_SIZE;
	iYSudoku = iBlock * 3 + iCell;

	if (iXSudoku < 0 || iXSudoku > 8 || iYSudoku < 0 || iYSudoku > 8){
		return false;
	}

	if (m_pFixedCells[iYSudoku][iXSudoku]){
		return false;
	}

	GetPossibleValues(iXSudoku, iYSudoku, pIsPossible);

	if(	Mode == MODULE_INTERACTIVE_LDOWN ){
		do{
			m_pSudoku[iYSudoku][iXSudoku]++;
			if (m_pSudoku[iYSudoku][iXSudoku] > 9){
				m_pSudoku[iYSudoku][iXSudoku] = 0;
			}
		}while(!pIsPossible[m_pSudoku[iYSudoku][iXSudoku]]);
	}
	else{
		do{
			m_pSudoku[iYSudoku][iXSudoku]--;
			if (m_pSudoku[iYSudoku][iXSudoku] < 0){
				m_pSudoku[iYSudoku][iXSudoku] = 9;
			}
		}while(!pIsPossible[m_pSudoku[iYSudoku][iXSudoku]]);
	}

	DrawBoard();

	DataObject_Update(m_pBoard);

	return true;
}

void CSudoku::DrawBoard()
{
	int i,j;
	int iX, iY;
	bool	pIsPossible[10];
	
	m_pBoard->Assign(CELL_COLOR_WHITE);
	
	for (i = 0; i < 3; i++){
		for (j = 0; j < 3; j++){
			iX = (116 + 3) * i;
			iY = (116 + 3) * j;
			DrawSquare(iX, iY, CELL_COLOR_BROWN, 116);
		}
	}

	for (i = 0; i < 9; i++){
		for (j = 0; j < 9; j++){
			GetPossibleValues(i, j, pIsPossible);
			DrawCell(i, j, pIsPossible);
		}
	}

	m_pBoard->Set_Value(0,0, CELL_COLOR_RED);
	m_pBoard->Set_Value(0,1, CELL_COLOR_BLACK);

}


void CSudoku::DrawCell(int iXCell, int iYCell, bool *pIsPossible)
{
	int i,j;
	int iX, iY;
	int iXBlock, iYBlock;
	int iXCellInBlock, iYCellInBlock;
	int iInnerX, iInnerY;
	int iInnerXCell, iInnerYCell;
	int iValue;
	int iNumberColor;//, iColor;

	iXBlock  = (iXCell - (iXCell % 3)) / 3;
	iYBlock  = (iYCell - (iYCell % 3)) / 3;
	iXCellInBlock = iXCell % 3;
	iYCellInBlock = iYCell % 3;			
	iX = (116 + 3) * iXBlock + 38 * iXCellInBlock + 2;
	iY = (116 + 3) * iYBlock + 38 * iYCellInBlock + 2;
	DrawSquare(iX, iY, CELL_COLOR_YELLOW, 36);

	if (iValue = m_pSudoku[iYCell][iXCell]){
		if (m_pFixedCells[iYCell][iXCell]){
			iNumberColor = CELL_COLOR_RED;
		}
		else{
			iNumberColor = CELL_COLOR_BLACK;
		}
		for (i = 0; i < 36; i++){
			for (j = 0; j < 36; j++){
				if (numbers[iValue - 1][j][i]){
					m_pBoard->Set_Value(iX + i, iY + 36 - j - 1, CELL_COLOR_WHITE);
				}
				else{
					m_pBoard->Set_Value(iX + i, iY + 36 - j - 1, iNumberColor);
				}
			}
		}
	}
	else{
		for (i = 0; i < 9; i++){
			iInnerXCell = i % 3;
			iInnerYCell = (i - iInnerXCell ) / 3;
			iInnerX = iX + iInnerXCell * 12 + 1;
			iInnerY = iY + iInnerYCell * 12 + 1;
			if (pIsPossible[i + 1]){
				DrawSquare(iInnerX, iInnerY, CELL_COLOR_GREEN, 10);
			}
			else{
				DrawSquare(iInnerX, iInnerY, CELL_COLOR_RED, 10);
			}
		}
	}


}

void CSudoku::DrawSquare(int iX, int iY, int iColor, int iSize)
{
	int i, j;

	for (i = 0; i < iSize; i++){
		for (j = 0; j < iSize; j++){
			m_pBoard->Set_Value(iX + i, iY + j, iColor);
		}
	}
}

void CSudoku::GetPossibleValues(int iX, int iY, bool *pIsPossible)
{

	int i,j;
	int iInitX, iInitY;

	iInitX = (int)floor((double)iX / 3.) * 3;
	iInitY = (int)floor((double)iY / 3.) * 3;
	
	for (i = 0; i < 10; i++){
		pIsPossible[i] = true;
	}

	for (i = 0; i < 9; i++){
		pIsPossible[m_pSudoku[iY][i]] = false;
	}

	for (i = 0; i < 9; i++){
		pIsPossible[m_pSudoku[i][iX]] = false;
	}

	for (i = 0; i < 3; i++){
		for (j = 0; j < 3; j++){
			pIsPossible[m_pSudoku[iInitY + i][iInitX + j]] = false;
		}
	}

	pIsPossible[0] = true;

}

bool CSudoku::Get_Grid_Pos(int &x, int &y)
{
	bool	bResult;

	if( m_pBoard && m_pBoard->is_Valid() )
	{
		bResult	= true;

		x		= (int)(0.5 + (Get_xPosition() - m_pBoard->Get_XMin()) / m_pBoard->Get_Cellsize());

		if( x < 0 )
		{
			bResult	= false;
			x		= 0;
		}
		else if( x >= m_pBoard->Get_NX() )
		{
			bResult	= false;
			x		= m_pBoard->Get_NX() - 1;
		}

		y		= (int)(0.5 + (Get_yPosition() - m_pBoard->Get_YMin()) / m_pBoard->Get_Cellsize());

		if( y < 0 )
		{
			bResult	= false;
			y		= 0;
		}
		else if( y >= m_pBoard->Get_NY() )
		{
			bResult	= false;
			y		= m_pBoard->Get_NY() - 1;
		}

		return( bResult );
	}

	x		= 0;
	y		= 0;

	return( false );
}

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Shepard_H
#define HEADER_INCLUDED__Shepard_H

//---------------------------------------------------------
class CShepard2d
{
public:
	CShepard2d(void);
	~CShepard2d(void);

	int 	Interpolate	(double *X, double *Y, double *F, int N_Points, int Quadratic_Neighbors, int Weighting_Neighbors);
	void 	GetValue 	(double px, double py, double &q);
	void 	Set_Missing	(double missing);
 
protected:

	int		*m_cells, *m_next, m_nPoints, m_nr;

	double	*m_x, *m_y, *m_f, *m_rsq, *m_a, xmin, ymin, dx, dy, rmax;


	void	Remove		(void)
	{
		this->CShepard2d::~CShepard2d();
	}
};

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Shepard_H

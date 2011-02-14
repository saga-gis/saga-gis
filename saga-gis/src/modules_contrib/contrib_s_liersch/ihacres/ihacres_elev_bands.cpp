/**********************************************************
 * Version $Id$
 *********************************************************/
#define NULL 0

#include "ihacres_elev_bands.h"

Cihacres_elev_bands::Cihacres_elev_bands()
{
	m_p_pcp = NULL;
	m_p_tmp = NULL;
	m_p_ER = NULL;
	m_p_streamflow_sim = NULL;
	m_p_Tw = NULL;
	m_p_WI = NULL;
	m_p_MeltRate = NULL;
	m_p_SnowStorage = NULL;
}

Cihacres_elev_bands::~Cihacres_elev_bands()
{
	//if (m_p_pcp) delete[] m_p_pcp;
	//if (m_p_tmp) delete[] m_p_tmp;
	//if (m_p_ER) delete[] m_p_ER;
	//if (m_p_streamflow_sim) delete[] m_p_streamflow_sim;
	//if (m_p_Tw) delete[] m_p_Tw;
	//if (m_p_WI) delete[] m_p_WI;
	//if (m_p_MeltRate) delete[] m_p_MeltRate;
	//if (m_p_SnowStorage) delete[] m_p_SnowStorage;
}
#pragma once

#include <saga_api/saga_api.h>

class CCut_Lines : public CSG_Tool
{
public:

	CCut_Lines (void);
	virtual ~CCut_Lines (void);

protected:

	virtual bool On_Execute(void);

private:
};

#include <iostream>
#include "MLB_Interface.h"

extern CSG_String Get_Info(int i);
extern CSG_Tool *Create_Tool(int i);
int main()
{
	int i = 0;
	CSG_String info = Get_Info(i);
	std::cout << "tool info: " << info.b_str() << std::endl;
	CSG_Tool *tool = NULL;
	while((tool = Create_Tool(i)) != NULL)
	{
		if (tool != TLB_INTERFACE_SKIP_TOOL)
			std::cout << "tool->Get_Name(): " << tool->Get_Name().b_str() << std::endl;
		i++;
	}
	return 0;
}

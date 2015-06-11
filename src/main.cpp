#include <stdlib.h>
#include "Master.h"



string getExecutionPath(char* argv)
{
	string tmp;

	/* GET PATH */
	char *real_path = realpath(argv, NULL);
	tmp = real_path;
	free(real_path);

	/* FORMAT PATH */
	for(unsigned int i=tmp.size(); i>=0; --i)
		if(tmp[i] == '/')
		{
			tmp.erase(i,tmp.size()-1);
			break;
		}


	return tmp;
}




int main(int argc, char* argv[])
{
	string path = getExecutionPath(argv[0]);

	Master master(path);

	master.run();

    return 0;
}


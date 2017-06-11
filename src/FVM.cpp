#include <fvm/TargetRegistry.h>
#include <fvm/Utils/Cmdline.h>
#include <stdio.h>

std::vector<TargetRegistry*> TargetRegistry::TheTargetList;

#include <Target/TargetList.h>

int main(int argc, char ** argv)
{
	cmdlineParse(argc, argv);

	TargetRegistry::launchTarget(0);

	printf("\n--------------\n> Finished executing Virtual Machine");
	return 0;
}

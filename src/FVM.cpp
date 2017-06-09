#include <fvm/TargetRegistry.h>
#include <conio.h>
#include <stdio.h>

std::vector<TargetRegistry*> TargetRegistry::TheTargetList;

#include <Target/TargetList.h>

int main(int argc, char ** argv)
{
	TargetRegistry::launchTarget(0);

	printf("\n--------------\n> Finished executing Virtual Machine");
	_getch();
	return 0;
}

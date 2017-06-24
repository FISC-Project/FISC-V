import os
from glob import glob

targetregistryPattern = "targetregistry."
targetListFolder = "../../src/Target"
targetListFile = targetListFolder + "/TargetList.h" 
targetFolders = glob(targetListFolder + "/*/")

targetsFound = []

def updateTargetList():
	if len(targetsFound) > 0:
		# #include "FISC/FISCTargetRegistry.hpp"
		buildString = ""

		print "-- Found targets:"
		for i in range(len(targetsFound)):
			print "--  " + str(i+1) + "- " + targetsFound[i][0]
			buildString += "#include \"" + targetsFound[i][0] + "/" + targetsFound[i][1] +"\"\n"
		
		print "-- Updating target list",
		file = open(targetListFile, "w")
		file.write(buildString)
		file.close()
		print "- Done"

def foundTarget(registryFileFullpath):
	targetName = os.path.basename(os.path.dirname(registryFileFullpath))
	registryFile = os.path.basename(registryFileFullpath)
	targetsFound.append((targetName, registryFile))

def stringCleanup(string):
	string = string.replace("\\", "/")
	return string

for i in range(len(targetFolders)):
	targetFolders[i] = stringCleanup(targetFolders[i])

	# Get all files in this directory
	registryPath = glob(targetFolders[i] + "/*")

	# Search for the registry file. The format is:
	# xxxTargetRegistry.xxx
	# It can also be lower case, for example: fisctargetregistry.hpp
	# Or even FISCTargetRegistry.hpp (this is the proper way of naming it)
	for j in range(len(registryPath)):
		if(targetregistryPattern in registryPath[j].lower()):
			foundTarget(stringCleanup(registryPath[j]))

updateTargetList()

import platform, os, sys, commands
from os import listdir
from os.path import isfile, join

pwd = os.path.abspath(os.path.dirname(sys.argv[0]))

def plat_get():
	plat = platform.system()
	if plat == "Linux":
		# Which Linux is this?
		plat = commands.getstatusoutput("cat /etc/os-release | grep 'ID_LIKE.*'")[1][8:].capitalize()
	return plat

plat = plat_get()

def run_script(script_name):
	if plat == "Debian":
		# Run Debian's scripts
		os.system(pwd + "/toolchain/Debian/" + script_name + ".sh")

	if plat == "Windows":
		# Run Windows' scripts
		os.system(pwd + "/toolchain/Windows/" + script_name + ".bat")

def help_and_choose():
	print "Usage: setup_or_run.pyc script_name [no script file extension]\n\nAvailable scripts are: "
	script_path = pwd + "/toolchain/" + plat
	script_files = [f for f in listdir(script_path) if isfile(join(script_path, f))]
	script_files_len = len(script_files)
	
	while(True):
		try:
			# Show script files
			for i in range(script_files_len):
				script_files[i] = script_files[i].replace(".bat", "").replace(".sh", "")
				print str(i+1) + "> " + script_files[i]
			print "\n" + str(script_files_len + 1) + "> Exit"
			choose = int(raw_input("\nChoose the script (number): "))
			if(choose == script_files_len + 1):
				os._exit(1)
			if(choose > script_files_len or choose < 0):
				print "Invalid script selection number. Try again."
				continue
		except:
			print "Invalid script selection number. Try again."
			continue

		# Run script
		run_script(script_files[int(choose) - 1])
		sys.exit(0)

if len(sys.argv) > 1:
	if(sys.argv[1] == "-h"):
		help_and_choose()
	# Run script
	run_script(sys.argv[1])
else:
	help_and_choose()
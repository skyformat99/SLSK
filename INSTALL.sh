#!/bin/bash

# =========================================================================
# ---[SLSK - Build/Install Script]---
# Copyright (C) 2017 Supremist (aka supremesonicbrazil)
# This file is part of Steam Linux Swiss Knife (or SLSK for short).
# Steam Linux Swiss Knife is available under the GNU GPL v3.0 license. See the
# accompanying COPYING file for more details.
# =========================================================================

# Checking if running as sudo
if [ $UID = 0 ]
then
	# Compiling from source code in a temporary folder
	mkdir bin
	qmake -o ./bin/Makefile ./src/SteamLinuxSwissKnife.pro
	cd bin
	make
	cd ..
		
	# Moving and copying stuff to the right places
	rm -r /opt/SLSK
	mkdir /opt/SLSK
	mv ./bin/SteamLinuxSwissKnife /opt/SLSK/SteamLinuxSwissKnife
	cp ./src/SteamLinuxGames.db /opt/SLSK/SteamLinuxGames.db
	cp -r ./img /opt/SLSK/img
	rm -r ./bin
		
	# Creating desktop entry
	echo '[Desktop Entry]
	Version=1.0
	Name=Steam Linux Swiss Knife
	Comment=Backup automation for Steam games, saves and configs
	Exec=/opt/SLSK/SteamLinuxSwissKnife
	Icon=/opt/SLSK/img/SLSK_icon.png
	Terminal=false
	Type=Application
	Categories=Utility;Application;' > ./Steam\ Linux\ Swiss\ Knife.desktop

	# Setting desktop shortcut as executable
	chmod +x ./Steam\ Linux\ Swiss\ Knife.desktop
		
	# Finishing
	clear
	echo "Done! Steam Linux Swiss Knife was successfully installed in '/opt/SLSK'."
	echo "If you want to uninstall it anytime, just do a 'sudo rm -r /opt/SLSK'."
	echo "Enjoy!"
	echo "--------------------------------------------------------------------------------"
else
	echo "Please run this script as sudo."
	echo "Aborting..."
	echo "--------------------------------------------------------------------------------"
	exit
fi
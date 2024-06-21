#!/bin/bash

DIALOG_CANCEL=1
DIALOG_ESC=255
HEIGHT=0
WIDTH=0

if command -v dialog &> /dev/null; then
	DIAL="dialog"
elif command -v whiptail &> /dev/null; then
	DIAL="whiptail"
else
	echo "No dialog-compatible program found, please install one"
	exit 1
fi

isroot() {
	if [ "$EUID" -ne 0 ]; then
		display_result "Please run as root"
		return 1
	fi
	return 0
}

DIALOG() {
	$DIAL --clear --backtitle "Yocto Installation Wizard" "$@"
}

display_result() {
	DIALOG --title "$1" \
	--msgbox "$result" $HEIGHT $WIDTH
}

KEY_SAVE="ctrl('S')"
KEY_EXIT="ctrl('C')"
KEY_INFO="'i'"
KEY_REFRESH="ctrl('R')"
KEY_HOME="ctrl('A')"
KEY_END="ctrl('E')"

key_config() {
	exec 3>&1 # open stdout fd
	OUTPUT=$(DIALOG --title "Edit Keybindings" \
	--form "Edit the fields" $HEIGHT $WIDTH 0 \
	"Save:"		1 1 "$KEY_SAVE"		1 15 15 0 \
	"Exit:"		2 1 "$KEY_EXIT"		2 15 15 0 \
	"Home:"		3 1 "$KEY_HOME"		3 15 15 0 \
	"End:"		4 1 "$KEY_END"		4 15 15 0 \
	"Info:"		5 1 "$KEY_INFO"		5 15 15 0 \
	"Refresh:"	6 1 "$KEY_REFRESH"	6 15 15 0 \
	2>&1 1>&3)
	exit_status=$?
	case $exit_status in
	"$DIALOG_CANCEL")
		clear
		return
	    	;;
	"$DIALOG_ESC")
		clear
		return
		;;
	esac
	exec 3>&- # delete fd

	outputarray=($OUTPUT) # convert to array
	# Replace the key* with the output of dialog
	sed -i "8s/$KEY_SAVE/${outputarray[0]}/" headers/keybindings.h 
	sed -i "9s/$KEY_EXIT/${outputarray[1]}/" headers/keybindings.h 
	sed -i "16s/$KEY_HOME/${outputarray[2]}/" headers/keybindings.h 
	sed -i "17s/$KEY_END/${outputarray[3]}/" headers/keybindings.h 
	sed -i "18s/$KEY_INFO/${outputarray[4]}/" headers/keybindings.h 
	sed -i "19s/$KEY_REFRESH/${outputarray[5]}/" headers/keybindings.h
}


config_dialog() {
	high_st="Disable"
	tmp="$(cat Makefile | head -n 17 | tail -n 1)"
	if ! grep -q "\-DHIGHLIGHT" <<< "$tmp"; then
		high_st="Enable"
	fi

	while true; do
		exec 3>&1 # open stdout fd
		selection=$(DIALOG --title "$1" \
		    --menu "Please select:" $HEIGHT $WIDTH 0 \
		    "1" "Edit Keybindings" \
		    "2" "Install dependencies" \
		    "3" "$high_st syntax highlighting" \
		2>&1 1>&3) # redirect output streams
		exit_status=$?
		exec 3>&- # delete fd
		case $exit_status in
		"$DIALOG_CANCEL")
			clear
			break
			;;
		"$DIALOG_ESC")
			clear
			break
			;;
		esac
		case $selection in
		1 )
			if [ $DIAL = "dialog" ]; then
				key_config
			else
				display_result "dialog not installed, but you can still edit keybindings.h"
			fi
			;;
		2 )
			clear
			if isroot; then
				echo "#### Installing Dependencies"
				if command -v apt &> /dev/null; then
					apt install libncurses-dev gcc make dialog
				elif command -v dnf &> /dev/null; then
					dnf install ncurses-devel gcc make dialog
				elif command -v pacman &> /dev/null; then
					pacman -S ncurses gcc make dialog
				fi
			fi
			sleep 1
		   	;;
		3 )
			if [ $high_st = "Enable" ]; then
				sed -i 's/CXXFLAGS = -Ofast -fopenmp -march=native -flto -lncursesw/CXXFLAGS = -Ofast -fopenmp -march=native -flto -DHIGHLIGHT -lncursesw/g' Makefile
			else
				sed -i 's/CXXFLAGS = -Ofast -fopenmp -march=native -flto -DHIGHLIGHT -lncursesw/CXXFLAGS = -Ofast -fopenmp -march=native -flto -lncursesw/g' Makefile
			fi
			;;
		esac
    	done
}

while true; do
    	exec 3>&1
    	selection=$(DIALOG --title "Menu" \
		--menu "Please select:" $HEIGHT $WIDTH 0 \
		"1" "Configure" \
		"2" "Compile" \
		"3" "Install" \
		"4" "Uninstall" \
    	2>&1 1>&3)
    	exit_status=$?
    	exec 3>&-
    	case $exit_status in
		"$DIALOG_CANCEL")
		    clear
		    echo "Program terminated."
		    exit
		;;
		"$DIALOG_ESC")
		    clear
		    echo "Program aborted." >&2
		    exit 1
		;;
    	esac
    	case $selection in
	1 )
		config_dialog "Configuration Options"
		;;
	2 )
		if make build 2>&1 >/dev/null | grep -q Error; then
			result="Make sure to report the output of make at: https://github.com/ikozyris/yocto/issues"
			display_result "Failed to build yocto"
	    	else
			result="Now you can install!"
			display_result "Succesfully built yocto"
		fi
		;;
	3 )
		if [ -d "$HOME.local/bin" ]; then
			result="Installed on ~/.local/bin"
			make install
			display_result "Installed Yocto"
		else
			if isroot; then
				result="Installed in /usr/bin"
				cp yocto /usr/bin/
				display_result "Installed Yocto"
			fi
		fi
		;;
	4 )
		if [ -d "$HOME.local/bin/yocto" ]; then
			result="Uninstalled from ~/.local/bin"
			rm ~/.local/bin/yocto
		else
			result="Uninstalled from /usr/bin"
			rm /usr/bin/yocto
		fi
		display_result "We are sorry to see you go."
		;;
    	esac
done

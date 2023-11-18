#!/bin/bash

DIALOG_CANCEL=1
DIALOG_ESC=255
HEIGHT=0
WIDTH=0

if command -v dialog &> /dev/null; then
	DIAL="dialog"
else
	DIAL="whiptail"
fi

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
	"Save:" 	1 1 "$KEY_SAVE"     1 15 15 0 \
      "Exit:"    	2 1 "$KEY_EXIT"     2 15 15 0 \
	"Home:"	3 1 "$KEY_HOME"	  3 15 15 0 \
	"End:"	4 1 "$KEY_END"	  4 15 15 0 \
      "Info:"    	5 1 "$KEY_INFO"     5 15 15 0 \
      "Refresh:"  6 1 "$KEY_REFRESH"  6 15 15 0 \
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
	sed -i "9s/$KEY_SAVE/${outputarray[0]}/" keybindings.h 
	sed -i "10s/$KEY_EXIT/${outputarray[1]}/" keybindings.h 
	sed -i "17s/$KEY_HOME/${outputarray[2]}/" keybindings.h 
	sed -i "18s/$KEY_END/${outputarray[3]}/" keybindings.h 
	sed -i "19s/$KEY_INFO/${outputarray[4]}/" keybindings.h 
	sed -i "20s/$KEY_REFRESH/${outputarray[5]}/" keybindings.h
}

config_dialog() {
	while true; do
		exec 3>&1 # open stdout fd
		selection=$(DIALOG --title "$1" \
		    --menu "Please select:" $HEIGHT $WIDTH 0 \
		    "1" "Edit Keybindings" \
		    "2" "Install dependencies" \
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
				display_result "dialog not installed; But you can still edit keybindings.h"
			fi
			;;
		2 )
			clear
			echo "#### Installing Dependencies"
			sudo apt install libncurses-dev gcc make dialog
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
			result="Make sure to report at:
			https://github.com/ikozyris/yocto/issues"
			display_result "Failed to build yocto"
	    	else
			result="Now you can install!"
			display_result "Succesfully built yocto"
	    fi
	;;
	3 )
		if [ ! -d "$HOME.local/bin" ]; then
			result="Installed on ~/.local/bin"
			make install
			display_result "Installed Yocto"
		else
			result="Installed in /usr/bin"
			cp yocto /usr/bin/
			display_result "Installed Yocto"
		fi
	;;
	4)
		if [ ! -d "$HOME.local/bin" ]; then
			result="Uninstalled from ~/.local/bin"
			rm ~/.local/bin/yocto
			display_result "We are sorry to see you go!"
		else
			result="Uninstalled from /usr/bin"
			rm /usr/bin/yocto
			display_result "We are sorry to see you go!"
		fi
	;;
    	esac
done
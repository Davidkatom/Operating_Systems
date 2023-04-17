#!/bin/bash
#David Shnaiderov 209198308
score=0
player1=50
player2=50
UpdateScore(){
	case $score in
		3)
			board=" |       |       #       |       |O"
			;;
	   	2)	
			board=" |       |       #       |   O   | "
			;;
		1)
			board=" |       |       #   O   |       | "
			;;
		0)
			board=" |       |       O       |       | "
			;;
		-1)
			board=" |       |   O   #       |       | "
			;;
		-2)	
			board=" |   O   |       #       |       | "
			;;
		-3)
			board="O|       |       #       |       | "
			;;
	esac
	DrawBoard
}

DrawBoard(){
	echo " Player 1: $player1         Player 2: $player2 "
	echo " --------------------------------- "
	echo " |       |       #       |       | "
	echo " |       |       #       |       | "
	echo -e "$board"
	echo " |       |       #       |       | "
	echo " |       |       #       |       | "
	echo " --------------------------------- "
}
UpdateScore
while true; do
	draw1=0
	draw2=0
	

	valid=0
	while [ $valid -eq 0 ]; do
	        read -s -p "PLAYER 1 PICK A NUMBER: " draw1
		if ! [[ $draw1 =~ ^[0-9]+$ ]]; then
                        echo "NOT A VALID MOVE !"
		elif [ $draw1 -gt $player1 ]; then
			echo "NOT A VALID MOVE !"		
		else
			valid=1
		fi
	done
	echo
 	valid=0
        while [ $valid -eq 0 ]; do
                read -s -p "PLAYER 2 PICK A NUMBER: " draw2
                if ! [[ $draw2 =~ ^[0-9]+$ ]]; then
                        echo "NOT A VALID MOVE !"
                elif [ $draw2 -gt $player2 ]; then
                        echo "NOT A VALID MOVE !"
                else
                        valid=1
                fi
        done	
	echo
	player1=$((player1 - draw1))
	player2=$((player2 - draw2))

	
	if [ $draw1 -gt $draw2 ]; then
		if [ $score -le 0 ]; then
			score=1
		else
			score=$((score + 1))
		fi
	elif [ $draw1 -lt $draw2 ]; then
		if [ $score -ge 0 ]; then
			score=-1
		else
			score=$((score - 1))
		fi
	fi
	UpdateScore
	echo -e "       Player 1 played: ${draw1}\n       Player 2 played: ${draw2}\n\n"	
	if [ $score -eq 3 ]; then
                echo "PLAYER 1 WINS !"
                exit
        fi
        if [ $score -eq -3 ]; then
                echo "PLAYER 2 WINS !"
                exit
        fi

	if [ $player1 -eq 0 ] && [ $player2 -eq 0 ]; then
		if [ $score -lt 0 ]; then
                	echo "PLAYER 2 WINS !"
			exit
		elif [ $score -gt 0 ]; then
			echo "PLAYER 1 WINS !"
			exit
		else 
			"IT'S A DRAW !"
                	exit
		fi

        elif [ $player1 -eq 0 ]; then
                echo "PLAYER 2 WINS !"
                exit
        elif [ $player2 -eq 0 ]; then
                echo "PLAYER 1 WINS !"
                exit
        fi

done

#!/bin/bash
#David Shnaiderov 209198308
cd $1
if [ "$#" -lt 2 ]; then
  echo "Not enough parameters"
  exit 1
fi
if [ "$3" == "-r" ]; then
	#list=$(grep -i -w -l --include='*.c' -r "$2" .)
	list=$(find . -type f -name "*.c" -exec grep -i -w -l "$2" {} \; -print)
	find . -type f -name "*.out" -exec rm -f {} +
else
	#list=$(grep -i -w -l --include='*.c' "$2" .)
	list=$(find . -maxdepth 1 -type f -name "*.c" -exec grep -i -w -l "$2" {} \; -print)
	rm -f *.out
fi
for x in $list
do
	name=${x%.*}
	name+=.out
	gcc -w -o $name $x 
done

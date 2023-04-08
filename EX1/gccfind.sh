#!/bin/bash
cd $1
if [ "$#" -lt 2 ]; then
  echo "Not enough parameters"
  exit 1
fi
if [ "$3" == "-r" ]; then
	list=$(grep -i -w -l --include='*.c' -r "$2" .)
	find . -type f -name "*.out" -exec rm -f {} +
else
	list=$(grep -i -w -l --include='*.c' "$2" .)
	rm -f *.out
fi
for x in $list
do
	name=${x%.*}
	name+=.out
	gcc -w -o $name $x 
done

#!/bin/sh

# use ./tests/test_script.sh /tests/add/add (-l for less information) to run script

dir_=$PWD;
dir_to_search=$PWD$1;

if [ $1 = "-h" ]; then
    echo "./tests/test_script.sh /tests/add/add (-l for less information)"
else
    for file in "$dir_to_search"/*; do
        echo "   "
        if [ $2 = "-l" ]; then
            echo "$file"
            ./compiler < "$file" >/dev/null
            ret_val=$?
            line=$(head -n 1 $file)
            echo "return value: $ret_val"
            echo  "expected: $line" 
        else 
            echo "$file"
            ./compiler < "$file"
            ret_val=$?
            line=$(head -n 1 $file)
            echo "return value: $ret_val"
            echo  "expected: $line" 
        fi
        echo "  "
    done
fi
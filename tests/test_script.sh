#!/bin/sh

# use ./tests/test_script.sh /tests/add/add (-l for less information) to run script

dir_to_search=$PWD$1;
less_flag=$2

search() {
    for file in "$1"/*; do
        if [ -f "$file" ]; then
            test "$file"
        elif [ -d "$file" ]; then
        echo "  "
        echo "  "
        echo "  "
        echo "----$file----"
        echo "  "
        
            search "$file";
        fi
    done
}

test() {
        if [ $less_flag = "-l" ]; then
            echo "$1"
            ./compiler < "$1" >/dev/null
            ret_val=$?
            line=$(head -n 1 $1)
            echo "return value:       \e[0;36m $ret_val \e[0m   \e[0;36m ///// \e[0m"
            echo "expected:\e[0;33m $line \e[0m  \e[0;33m ///// \e[0m" 
            echo "                         /"
            echo "                        /"

        else 
            echo "$1"
            ./compiler < "$1"
            ret_val=$?
            line=$(head -n 1 $1)
            echo "return value:      \e[0;36m $ret_val \e[0m   \e[0;36m ///// \e[0m"
            echo "expected:\e[0;33m $line \e[0m  \e[0;33m ///// \e[0m" 
            echo "                        /"
            echo "                       /"
        fi
        echo "  "
}

if [ $1 = "-h" ]; then
    echo "./tests/test_script.sh /tests/add/add (-l for less information)"
elif [ -f $dir_to_search ]; then
    test $dir_to_search
else
    search "$dir_to_search"
fi
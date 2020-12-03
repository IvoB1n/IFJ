#!/bin/sh

# use ./tests/test_script.sh /tests/add/add (-l for less information) to run script

dir_to_search=$1;
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
        if [ "$less_flag" = "-l" ]; then
            echo "$1"
            ./compiler < "$1" >/dev/null 2>/dev/null
            ret_val=$?
            line=$(head -n 1 $1)
            echo "  "
            echo "return value:       \e[0;36m $ret_val \e[0m   \e[0;36m ///// \e[0m"
            echo "expected:\e[0;33m $line \e[0m  \e[0;33m ///// \e[0m" 
            echo "                         /"
            echo "                        /"
        elif [ "$less_flag" = "-v" ]; then
            echo "$1"
            rm valgrind.txt
            valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./compiler < "$1" >gen.code 2>valgrind.txt
            ret_val=$?
            line=$(head -n 1 $1)
            echo "  "
            echo "return value:        $ret_val"
            echo "expected $line " 
        elif [ "$less_flag" = "-lv" ]; then
            echo "$1"
            valgrind ./compiler < "$1" >/dev/null # 2>/dev/null
            ret_val=$?
            line=$(head -n 1 $1)
            echo "  "
            echo "return value:       \e[0;36m $ret_val \e[0m   \e[0;36m ///// \e[0m"
            echo "expected:\e[0;33m $line \e[0m  \e[0;33m ///// \e[0m" 
            echo "                         /"
            echo "                        /"
        elif [ "$less_flag" = "-gl" ]; then
            echo "$1"
            ./compiler < "$1"   2>/dev/null > output.out
            ret_val=$?
            # line=$(head -n 1 $1)
            # echo "  "
            # echo "return value:       \e[0;36m $ret_val \e[0m   \e[0;36m ///// \e[0m"
            # echo "expected:\e[0;33m $line \e[0m  \e[0;33m ///// \e[0m" 
            # echo "                         /"
            # echo "                        /"
        elif [ "$less_flag" = "-g" ]; then
            echo "$1"
            ./compiler < "$1" #> output.out  2>/dev/null
            ret_val=$?
            line=$(head -n 1 $1)
            echo "  "
            echo "return value:       \e[0;36m $ret_val \e[0m   \e[0;36m ///// \e[0m"
            echo "expected:\e[0;33m $line \e[0m  \e[0;33m ///// \e[0m" 
            echo "                         /"
            echo "                        /"
        elif [ "$less_flag" = "-go" ]; then
            rm gen.code
            ./compiler < "$1" >gen.code #2>/dev/null
            ret_valcom=$?
            echo " ~~~~~~~~~ "
            ./ic20int gen.code
            echo ""
            echo " ~~~~~~~~~ "
            ret_valint=$?
            
            line=$(head -n 1 $1)
            echo "return value comp:   $ret_valcom "
            echo "expected: $line "
            echo "return value inter:  $ret_valint "
            echo "$1"

        else 
            echo "$1"
            ./compiler < "$1"
            ret_val=$?
            line=$(head -n 1 $1)
            echo "  "
            echo "return value:       \e[0;36m $ret_val \e[0m   \e[0;36m ///// \e[0m"
            echo "expected:\e[0;33m $line \e[0m  \e[0;33m ///// \e[0m" 
            echo "                         /"
            echo "                        /"
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
#!/bin/bash
build_name="";
compile() {
    echo "compiling $1."
    cmake . -DBUILD=$1
    build_name=$1 
    make 
}

while getopts ":b:d:t" option; do
    case $option in
        d)
            compile "debug";;
        t) 
            compile "tests";;
        b)  
            compile "build";;
    esac
done
if [ "$build_name" = "" ]; then compile "build" ; fi

OPTIND=1
while getopts ":r" option; do
    case $option in
        r) 
            echo "running $build_name."
            cd bin
            ./${build_name};
            ;;
    esac
done

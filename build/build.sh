#!/bin/bash

build_target=""; build_bin=""; build_status=0;
run_build=false;

compile() {
    echo "compiling $build_target."
    cmake .. -DCMAKE_BUILD_TYPE=$build_target && make
    build_status=$?
    if [ $build_status == 0 ]; then
        echo "compiled $build_target succesfully." 
    fi
}

run() {
    if [ "$build_bin" == "" ]; then
        build_bin="build"
    fi
    if [ $build_status == 0 ]; then
        cd bin
        ./$build_bin
        cd ..
    fi
}

while getopts "bdtr" option; do
    case $option in
        b)  
            build_target="Release"
            build_bin="build"
            ;;
        d)
            build_target="Debug"
            build_bin="debug"
            ;;
        t) 
            build_target="Test"
            build_bin="test"
            ;;
        r)
            run_build=true
            ;;
    esac
done

if [ "$build_target" != "" ]; then compile ; fi
if [ $run_build == true ]; then run ; fi

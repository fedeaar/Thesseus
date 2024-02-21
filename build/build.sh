#!/bin/bash

build_name=""; build_status=0;
scene_path="two_cubes.scene.cpp";
run_build=false;

compile() {
    echo "compiling $build_name with scene $scene_path."
    cmake .. -DBUILD=$build_name -DSCENE=$scene_path && make
    build_status=$?
    if [ $build_status == 0 ]; then
        echo "compiled $build_name with scene $scene_path." 
    fi
}

run() {
    if [ "$build_name" == "" ]; then
        build_name="build"
    fi
    if [ $build_status == 0 ]; then
        cd bin
        ./$build_name
        cd ..
    fi
}

while getopts "bdtrs:" option; do
    case $option in
        b)  
            build_name="build"
            ;;
        d)
            build_name="debug"
            ;;
        t) 
            build_name="tests"
            ;;
        r)
            run_build=true
            ;;
        s)
            scene_path=${OPTARG}
            ;;
    esac
done

if [ "$build_name" != "" ]; then compile ; fi
if [ $run_build == true ]; then run ; fi

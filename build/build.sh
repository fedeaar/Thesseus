#!/bin/bash

build_name="";
scene_path="two_cubes.scene.cpp";
run_build=false;

compile() {
    echo "compiling $build_name with scene $scene_path."
    cmake .. -DBUILD=$build_name -DSCENE=$scene_path
    make
    echo "compiled $build_name with scene $scene_path." 
}

run() {
    cd bin
    ./$build_name
    cd ..
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
if [ $run_build = true ]; then run ; fi

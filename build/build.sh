#!/bin/bash
cwd="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")";
bin="bin" # todo: use in cmake
build_target=""; run_cmd=""; build_status=0;
run_build=false;

compile() {
    echo "compiling $build_target."
    cd $cwd
    cmake $cwd/.. -DCMAKE_BUILD_TYPE=$build_target && make
    build_status=$?
    if [ $build_status == 0 ]; then
        echo "compiled $build_target succesfully." 
    fi
}

run() {
    if [ "$run_cmd" == "" ]; then
        run_cmd="build"
    fi
    if [ $build_status == 0 ]; then
        cd $cwd/$bin
        $run_cmd
    fi
}

while getopts "bdtr" option; do
    case $option in
        b)  
            build_target="Release"
            run_cmd="./build"
            ;;
        d)
            build_target="Debug"
            run_cmd="gdb debug"
            ;;
        t) 
            build_target="Test"
            run_cmd="./test"
            ;;
        r)
            run_build=true
            ;;
    esac
done

if [ "$build_target" != "" ]; then compile ; fi
if [ $run_build == true ]; then run ; fi
cd $PWD

#!/bin/bash

cmake . ;
make ;

while getopts ":r" option; do
    case $option in
        r)  # run build
            ./out
            exit;;
    esac
done

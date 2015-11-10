#!/bin/bash

# ./docker/cleanup.sh 
# ./docker/start.sh

run() {
    number=$1
    shift
    for i in `seq $number`; do
      $@
    done
}

# (cd ./build/ && cmake .. && make -j && make check -j)
run $1 "gnome-terminal -x zsh -c './build/NetLayer' &"
# (gnome-terminal -x zsh -c "./build/NetLayer") &
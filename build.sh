#!/bin/sh

if [ ! -n "$1" ];then
    cppFiles=(cpp_src/*.cpp)

    for f in ${cppFiles[@]}; do
        arr=(${f//\// });
        LAST_PART=${arr[${#arr[@]} - 1]}
        echo g++ $f -o ${LAST_PART//\.cpp/'.out'};
        g++ $f -o ${LAST_PART//\.cpp/'.out'}
    done
else
    arr=(${1//\// });
    LAST_PART=${arr[${#arr[@]} - 1]}
    echo g++ $1 -o ${LAST_PART//\.cpp/'.out'};
    g++ $1 -o ${LAST_PART//\.cpp/'.out'};
fi

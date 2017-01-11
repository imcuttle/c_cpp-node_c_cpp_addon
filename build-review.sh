#!/bin/sh

run() {
    f=$1
    LAST_PART=$2
    if grep -q pthread.h "$f"; then
        echo pthread
        g++ $f -o ${LAST_PART//\.cpp/'.out'} -pthread 1>/dev/null
    else
        g++ $f -o ${LAST_PART//\.cpp/'.out'} 1>/dev/null
    fi
}

if [ ! -n "$1" ];then
    cppFiles=(tcp-ip-review/*.cpp)

    for f in ${cppFiles[@]}; do
        arr=(${f//\// });
        LAST_PART=${arr[${#arr[@]} - 1]}
        run $f $LAST_PART
    done
else
    arr=(${1//\// });
    LAST_PART=${arr[${#arr[@]} - 1]}
    run $1 $LAST_PART
fi

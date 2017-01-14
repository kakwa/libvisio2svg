#!/bin/sh

cd `dirname $0`
mkdir -p ../out

# Once a file is seen as corrupted by vss2svg
# we alter if $MAX more times to try to make
# vss2svg crash
MAX=100

burn_in_hell(){
    counter1=0
    while [ $counter1 -lt $MAX ]
    do
        tmp_vss=`mktemp -p ../out/`
        cp vss/`ls vss |shuf -n 1` ${tmp_vss} 
        counter2=0
        while [ $counter2 -lt $MAX ]
        do
            ../../vss2svg -p -w 800 -h 600 -i ${tmp_vss} -o ../out/test.svg -p
            ret=$?
            if [ $ret -eq 1 ]
            then
                counter2=$MAX
            elif [ $ret -gt 1 ]
            then
                ts=`date +"%F-%H%M%S"`
                printf "[ERROR] corrupted file 'bad_corrupted_${ts}.vss' caused something wrong\n"
                cp ${tmp_vss} ../out/bad_corrupted_${ts}.vss
                exit $ret
            fi
            size=`du -b ${tmp_vss} |sed "s/\t.*//"`
            count=$(( `od -vAn -N4 -tu4 < /dev/urandom` % $size  + 1 ))
            tmp_vss2=`mktemp -p ../out/`
            dd if=${tmp_vss} of=${tmp_vss2} count=$count bs=1 >/dev/null 2>&1
            mv ${tmp_vss2} ${tmp_vss}
            counter2=$(( $counter2 + 1 ))
        done
        rm "${tmp_vss}"
        counter1=$(( $counter1 + 1 ))
    done
}

if [ "$1" = "inf" ]
then
    while true
    do
        burn_in_hell
    done
else
    burn_in_hell
fi

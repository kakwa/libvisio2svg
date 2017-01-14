#!/bin/sh


cd `dirname $0`
if ! [ "$1" = "nocolor" ]
then
. ./colors.sh
fi

percent(){
    printf "$(( $1 * 100 / $2  ))"
}

EMFC="../../src/lib/vss2svg_rec_*"
PMFC="../../src/lib/pmf2svg_print.c"
count_vss_SUPPORTED=`cat $EMFC | grep -c FLAG_SUPPORTED`
count_vss_IGNORED=`cat $EMFC | grep -c FLAG_IGNORED`
count_vss_UNUSED=`cat $EMFC | grep -c FLAG_UNUSED`
count_vss_PARTIAL=`cat $EMFC | grep -c FLAG_PARTIAL`
total_vss=$(( $count_vss_SUPPORTED + $count_vss_IGNORED + $count_vss_UNUSED + $count_vss_PARTIAL  ))

printf "EMF/EMF+ record type coverage\n"
printf "%s\n\n" "-----------------------------"
printf "EMF RECORDS:\n\n"

printf "|   Status  | Count | Percent |\n"
printf "|:---------:|:-----:|:-------:|\n"
printf "| ${BGre}Supported ${RCol}|  %3d  | [${BCya}%4d%%${RCol}] |\n"  "$count_vss_SUPPORTED" "`percent $count_vss_SUPPORTED $total_vss`"
printf "| ${BYel}Partial   ${RCol}|  %3d  | [${BCya}%4d%%${RCol}] |\n"  "$count_vss_PARTIAL"   "`percent $count_vss_PARTIAL   $total_vss`"
printf "| ${BBlu}Unused    ${RCol}|  %3d  | [${BCya}%4d%%${RCol}] |\n"  "$count_vss_UNUSED"    "`percent $count_vss_UNUSED    $total_vss`"
printf "| ${BRed}Ignored   ${RCol}|  %3d  | [${BCya}%4d%%${RCol}] |\n"  "$count_vss_IGNORED"   "`percent $count_vss_IGNORED   $total_vss`"
printf "| Total     |  %3d  |         |\n" $total_vss


printf "\n"

count_vss_SUPPORTED=`grep -c FLAG_SUPPORTED $PMFC`
count_vss_IGNORED=`grep -c FLAG_IGNORED     $PMFC`
count_vss_UNUSED=`grep -c FLAG_UNUSED       $PMFC`
count_vss_PARTIAL=`grep -c FLAG_PARTIAL     $PMFC`
total_vss=$(( $count_vss_SUPPORTED + $count_vss_IGNORED + $count_vss_UNUSED + $count_vss_PARTIAL  ))
printf "EMF+ RECORDS:\n\n"

printf "|   Status  | Count | Percent |\n"
printf "|:---------:|:-----:|:-------:|\n"
printf "| ${BGre}Supported ${RCol}|  %3d  | [${BCya}%4d%%${RCol}] |\n"  "$count_vss_SUPPORTED" "`percent $count_vss_SUPPORTED $total_vss`"
printf "| ${BYel}Partial   ${RCol}|  %3d  | [${BCya}%4d%%${RCol}] |\n"  "$count_vss_PARTIAL"   "`percent $count_vss_PARTIAL   $total_vss`"
printf "| ${BBlu}Unused    ${RCol}|  %3d  | [${BCya}%4d%%${RCol}] |\n"  "$count_vss_UNUSED"    "`percent $count_vss_UNUSED    $total_vss`"
printf "| ${BRed}Ignored   ${RCol}|  %3d  | [${BCya}%4d%%${RCol}] |\n"  "$count_vss_IGNORED"   "`percent $count_vss_IGNORED   $total_vss`"
printf "| Total     |  %3d  |         |\n" $total_vss


#!/bin/sh

if [ "`uname`" = "Darwin" ]
then
    RL=greadlink
else
    RL=readlink
fi

OUTDIR="../out"
EMFDIR="./vss"
ret=0
VAGRIND_CMD="valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes --error-exitcode=42"

VERBOSE=1
ABSPATH=$($RL -f "$(dirname $0)")
STOPONERROR="no"

help(){
    cat <<EOF
usage: `basename $0` [-h] [-v] [-e <vss dir>] [-s] [-n] 

Script checking memleaks, segfault and svg correctness of vss2svg-conv

arguments:
  -h: diplays this help
  -v: verbose mode, print vss records
  -e: alternate vss dir (default '`$RL -f "$ABSPATH/$EMFDIR"`')
  -s: stop on first error
  -r: resize to 800x600
  -x: disable xmllint check (svg integrity)
  -n: disable valgrind (memleaks checks)
  -N: ignore return code of vss2svg-conv (useful for checks on corrupted files)
EOF
    exit 1
}


while getopts ":hnNxrvse:" opt; do
  case $opt in

    h) 
        help
        ;;
    n)
        VAGRIND_CMD=""
        ;;
    v)
        VERBOSE=0
        VERBOSE_OPT='--verbose'
        ;;
    e)
        EMFDIR=`$RL -f "$OPTARG" |sed "s%$ABSPATH%.%"`
        ;;
    s)
        STOPONERROR="yes"
        ;;
    x)
        XMLLINT="no"
        ;;
    r)
        RESIZE_OPTS="-w 800 -h 600"
        ;;
    N)
        IGNORECONVERR="yes"
        ;;
    \?)
        echo "Invalid option: -$OPTARG" >&2
        help
        exit 1
        ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
        help
        exit 1
        ;;
  esac
done

verbose_print(){
    msg="$1"
    if [ $VERBOSE -eq 0 ]
    then
       printf "$msg\n"
    fi
}

cd $ABSPATH
. ./colors.sh
rm -rf $OUTDIR
mkdir -p $OUTDIR
CMD="`$RL -f ../../vss2svg-conv`"
OUTDIR=`$RL -f $OUTDIR`
DTD=`$RL -f ./svg11-flat.dtd`

mkdir -p "$EMFDIR"
cd "$EMFDIR"
if ! [ -f HP-Classic-ProLiant.zip ]
then
    wget http://www.visiocafe.com/downloads/hp/HP-Classic-ProLiant.zip
    unzip HP-Classic-ProLiant.zip
fi
cd -

for vss in `find $EMFDIR -type f -name "*.vss" |sort`
do
    EMF="`$RL -f $vss`"
    verbose_print "\n############## `basename "${vss}"` ####################"
    verbose_print "Command: $CMD $RESIZE_OPTS -i \"$EMF\" -o \"${OUTDIR}\""
    $VAGRIND_CMD $CMD -i "$EMF" -o ${OUTDIR} $VERBOSE_OPT
    tmpret=$?
    if [ $tmpret -ne 0 ]
    then
        printf "[${BYel}ERROR${RCol}] vss2svg-conv exited on error or memleaked or crashed converting vss '$EMF'\n"
        [ $tmpret -eq 42 ] || [ $tmpret -eq 139 ] || ! [ "$IGNORECONVERR" = "yes" ] && ret=1
    fi
    if ! [ "$XMLLINT" = "no" ]
    then
	for SVG in `find ${OUTDIR} -type f -name "*.svg" |sort`
	do
        xmllint --dtdvalid ./svg11-flat.dtd  --noout ${SVG} >/dev/null 2>&1
        if [ $? -ne 0 ]
        then
            printf "[${BYel}ERROR${RCol}] vss2svg-conv generate bad svg\n"
            printf "source vss:  $EMF\n"
            printf "out svg   :  $SVG\n\n"
            printf "xmllint result:\n"
            xmllint --dtdvalid ./svg11-flat.dtd  --noout ${SVG} 2>&1 >/dev/null
            printf "\n"
            printf "Convert and xmllint commands:\n"
            printf "$CMD -i \"$EMF\" -o \"${OUTDIR}\"\n"
            printf "xmllint --dtdvalid $DTD --noout ${SVG}\n\n"
            ret=1
        fi
	done
	rm -f "${OUTDIR}/*.svg"
    fi
    verbose_print "\n#####################################################\n"
    [ "${STOPONERROR}" = "yes" ] && [ $ret -eq 1 ] && exit 1
done
if [ $ret -ne 0 ]
then
    printf "[${BRed}FAIL${RCol}] Check exited with error(s)\n"
else
    printf "[${BGre}SUCCESS${RCol}] Check Ok\n"
fi
exit $ret

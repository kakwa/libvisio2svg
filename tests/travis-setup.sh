#!/bin/sh

help(){
  cat <<EOF
usage: `basename $0` <args>

<description>

arguments:
  <options>
EOF
  exit 1
}

while getopts ":hn:" opt; do
  case $opt in

    h) 
        help
        ;;
    n)
        NAME="$OPTARG"
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

OUT=`dirname $0`
OUT=`readlink -f $OUT`
mkdir -p ${OUT}/deps_build
cd ${OUT}/deps_build

RVNG_VERSION=0.0.4
VISIO_VERSION=0.1.5
EMF_VERSION=master

! [ -e "${EMF_VERSION}.tar.gz" ] && wget https://github.com/kakwa/libemf2svg/archive/${EMF_VERSION}.tar.gz
rm -rf libemf2svg-${EMF_VERSION}
tar -xf ${EMF_VERSION}.tar.gz || exit 1
cd libemf2svg-${EMF_VERSION} && CC=clang CXX=clang++ cmake . -DUSE_CLANG=ON && make && make install DESTDIR=$OUT && cd - || exit 1

if ! [ "`uname`" = "Darwin" ]
then
    ! [ -e "librevenge-${RVNG_VERSION}.tar.xz" ] && wget http://ks1.kakwalab.ovh/src/librevenge-${RVNG_VERSION}.tar.xz
    rm -rf librevenge-${RVNG_VERSION}
    tar -xf librevenge-${RVNG_VERSION}.tar.xz || exit 1
    cd librevenge-${RVNG_VERSION} && ./configure --disable-tests --without-docs && make && make install DESTDIR=$OUT && cd - || exit 1
    find $OUT -name "*.pc" -exec sed -i "s|^prefix=|prefix=$OUT|" {} \;
    
    export PKG_CONFIG_PATH="$OUT/usr/local/lib/pkgconfig"
    ! [ -e "libvisio-${VISIO_VERSION}.tar.xz" ] && wget http://ks1.kakwalab.ovh/src/libvisio-${VISIO_VERSION}.tar.xz
    rm -rf libvisio-${VISIO_VERSION}
    tar -xf libvisio-${VISIO_VERSION}.tar.xz || exit 1
    cd libvisio-${VISIO_VERSION} && ./configure --disable-tests --disable-tools --without-docs && make && make install DESTDIR=$OUT && cd - || exit 1
fi

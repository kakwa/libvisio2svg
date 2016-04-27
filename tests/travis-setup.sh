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

RVNG_VERSION=0.0.4
VISIO_VERSION=0.1.5
EMF_VERSION=0.5.1

! [ -e "${EMF_VERSION}.tar.gz" ] && wget https://github.com/kakwa/libemf2svg/archive/${EMF_VERSION}.tar.gz
rm -rf libemf2svg-${EMF_VERSION}
tar -xf ${EMF_VERSION}.tar.gz || exit 1
cd libemf2svg-${EMF_VERSION} && cmake . && make && make install DESTDIR=$OUT && cd - || exit 1

! [ -e "librevenge-${RVNG_VERSION}.tar.xz" ] && wget http://netix.dl.sourceforge.net/project/libwpd/librevenge/librevenge-${RVNG_VERSION}/librevenge-${RVNG_VERSION}.tar.xz
rm -rf librevenge-${RVNG_VERSION}
tar -xf librevenge-${RVNG_VERSION}.tar.xz || exit 1
cd librevenge-${RVNG_VERSION} && ./configure --disable-tests && make && make install DESTDIR=$OUT && cd - || exit 1
find $OUT -name "*.pc" -exec sed -i "s|^prefix=|prefix=$OUT|" {} \;

export PKG_CONFIG_PATH="$OUT/usr/local/lib/pkgconfig"
! [ -e "libvisio-${VISIO_VERSION}.tar.xz" ] && wget http://dev-www.libreoffice.org/src/libvisio/libvisio-${VISIO_VERSION}.tar.xz
rm -rf libvisio-${VISIO_VERSION}
tar -xf libvisio-${VISIO_VERSION}.tar.xz || exit 1
cd libvisio-${VISIO_VERSION} && ./configure --disable-tests --disable-tools && make && make install DESTDIR=$OUT && cd - || exit 1

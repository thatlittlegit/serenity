#!/bin/sh
set -e

script_path=$(cd -P -- "$(dirname -- "$0")" && pwd -P)
cd "$script_path"

fast_mode=1
make_flags=

make_mode=
while [ "$1" != "" ]; do
    if [ "$make_mode" = "true" ]; then
        make_flags="$make_flags $1"
        shift
        continue
    fi

    case $1 in
        -f | --fast )           fast_mode=1
                                ;;
        -j | --multicore)       make_flags="$make_flags -j4"
                                ;;
        --)                     make_mode=true
                                ;;
        -h | --help )           printf -- "-f or --fast: build fast without cleaning or running tests\n"
                                printf -- "-j or --multicore: call make with -j4\n"
                                printf -- "--: all following arguments go to make\n"
                                exit 0
                                ;;
    esac
    shift
done

sudo id

MAKE="make"

if [ "$(uname -s)" = "OpenBSD" ]; then
    MAKE="gmake"
fi

echo "make will be called as:\t$MAKE $make_flags"

if [ "$fast_mode" = "1" ]; then
    $MAKE -C ../ $make_flags && \
        $MAKE -C ../ install $make_flags &&
        sudo -E PATH="$PATH" ./build-image-qemu.sh
else
    $MAKE -C ../ clean $make_flags && \
        $MAKE -C ../ $make_flags && \
        $MAKE -C ../ test $make_flags && \
        $MAKE -C ../ install $make_flags &&
        sudo -E PATH="$PATH" ./build-image-qemu.sh
fi

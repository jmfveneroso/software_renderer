set -e

if [ $# -eq 0 ]; then
  key=""
else
  key="$1"
fi

case $key in
  -q|--quick)
    cd build
    make
    cd ..
    ;;
  -f|--full)
    rm -r build/*
    cd build
    cmake ..
    make
    cd ..
    ./build/main
    ;;
  *) # unknown option
    cd build
    make
    cd ..
    ./build/main
    ;;
esac

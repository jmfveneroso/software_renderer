if [ $# -eq 0 ]; then
  key=""
else
  key="$1"
fi

case $key in
  -f|--full)
    rm -r build/*
    cd build
    cmake ..
    make
    cd ..
    ./build/main
    ;;
  *)    # unknown option
    cd build
    make
    cd ..
    # ./build/main
    ;;
esac

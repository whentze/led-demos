g++ $1 -I ../rpi-rgb-led-matrix/include/ -lpthread -L ../rpi-rgb-led-matrix/lib -l rgbmatrix -o "$1.bin"

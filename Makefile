LED_BASE_DIR=../rpi-rgb-led-matrix

LED_LIB_DIR=${LED_BASE_DIR}/lib
LED_INCLUDE_DIR=${LED_BASE_DIR}/include

CXXFLAGS+=-std=c++11
LDFLAGS+=-lpthread -L ${LED_LIB_DIR} -l rgbmatrix 

binaries = wanja marcel best rainbow

%: %.cc
	$(CXX) $< ${CXXFLAGS} -I ${LED_INCLUDE_DIR} ${LDFLAGS} -o $@

all: ${binaries}

clean:
	${RM} ${binaries}

.PHONY: all clean

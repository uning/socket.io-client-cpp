

#BOOST_PREFIX ?= /Users/playcrab/local/src/cmoss/bin/ios
#BOOST_LIB_PATH		?= $(BOOST_PREFIX)/lib
#BOOST_INCLUDE_PATH ?= $(BOOST_PREFIX)/include
BOOST_PREFIX ?= /usr/local/boost_1_48_0

BOOST_LIB_PATH		?= $(BOOST_PREFIX)/stage/lib
BOOST_INCLUDE_PATH ?= $(BOOST_PREFIX)/

JSONCPP_PATH = ../../jsoncpp

CFLAGS = -Wall -O2 $(CPP11)  -I$(BOOST_INCLUDE_PATH) -I$(JSONCPP_PATH) #-I../src
LDFLAGS = -L$(BOOST_LIB_PATH)

CXX		?= c++

LDFLAGS := $(LDFLAGS) $(BOOST_LIB_PATH)/lib*.a


LDFLAGS := $(LDFLAGS) -lcrypto -lssl -lpthread $(JSONCPP_INCLUDE_PATH) $(JSONCPP_PATH)/libs/linux-gcc-4.2.1/libjson_linux-gcc-4.2.1_libmt.a ../libsioclientpp.a

sio_client: sio_client.o
	$(CXX) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c $(CFLAGS) -o $@ $^

# cleanup by removing generated files
#
.PHONY:		clean
clean:
		rm -f *.o sio_client

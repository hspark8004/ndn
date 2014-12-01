CC := g++ -std=c++11
CFLAGS := -lpcap -lpthread -I./include
TARGET := main
SRCS := send_interest.cpp Data.cpp Interest.cpp LinkLayer.cpp EthernetLayer.cpp Name.cpp NdnLayer.cpp Container.cpp ReqInformation.cpp Fragment.cpp FaceMap.cpp
#SRCS := send_interest.cpp
#OPTIONS := -Wdeprecated-declarations
OPTIONS := -Wno-write-strings

all:
	$(CC) -o $(TARGET) $(SRCS) $(CFLAGS) $(OPTIONS)

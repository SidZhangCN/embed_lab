CXX = g++
LIBS +=`pkg-config --libs opencv zbar`
INC +=`pkg-config --cflags opencv zbar`
# CPPFLAGS += -g -std=c++11 -Wall
CPPFLAGS += -g -std=c++11

SRCS = SocketClientMat.cpp SocketMatTransmissionClient.cpp
TARGET = client
OBJS = SocketClientMat.o SocketMatTransmissionClient.o

$(TARGET):$(OBJS)
	$(CXX) $(INC) $(CPPFLAGS) $(OBJS) -o $(TARGET) $(LIBS)
$(OBJS):%.o:%.cpp
	$(CXX) $(INC) $(CPPFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -r *.o $(TARGET)
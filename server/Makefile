CXX = g++
LIBS +=`pkg-config --libs opencv`
INC +=`pkg-config --cflags opencv`
CPPFLAGS += -g -std=c++11
# CPPFLAGS += -g -std=c++11 -Wall

SRCS = SocketServerMat.cpp SocketMatTransmissionServer.cpp cJSON.cpp CJsonObject.cpp
TARGET = server
OBJS = SocketServerMat.o SocketMatTransmissionServer.o cJSON.o CJsonObject.o

$(TARGET):$(OBJS)
	$(CXX) $(INC) $(CPPFLAGS) $(OBJS) -o $(TARGET) $(LIBS)
$(OBJS):%.o:%.cpp
	$(CXX) $(INC) $(CPPFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -r *.o $(TARGET)
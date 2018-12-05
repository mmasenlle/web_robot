OBJS = wrobot.o \
 pages/host.o pages/page.o pages/pagecontainer.o \
 robot/finder.o robot/strautom.o robot/matchacum.o robot/robot.o \
 utils/poller.o utils/resolver.o utils/crc32.o

INC = -I. -Ipages -Irobot -Iutils

FLAGS = -O3 -fPIC #-g
CXXFLAGS = $(FLAGS) $(INC) -MMD -MP

all:	wrobot

wrobot:	$(OBJS)
	$(CXX) $(FLAGS) -pthread -o wrobot $(OBJS)

clean:
	rm -f pages/*.d robot/*.d utils/*.d *.d
	rm -f pages/*.o robot/*.o utils/*.o *.o

# Include automatically generated dependency files
-include $(wildcard *.d robot/*.d pages/*.d utils/*.d)

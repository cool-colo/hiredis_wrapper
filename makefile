DEBUG = -g -ggdb
;DEBUG = -g -ggdb -DDISABLEPROFILER
DEBUG = -O2
BIN=`date +HLF`
CC= g++
M=MM

INCLUDES=-I/usr/include -I./hiredis

LIBS=-L/lib64 -L/usr/lib64 -L/usr/local/lib64 -L./lib \
		-lrt -lpthread -lcrypt -lnsl -lm -lssl -lcrypto -lhiredis
CPPFLAGS= $(DEBUG) -std=c++0x -rdynamic -m64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 
;CPPFLAGS= $(DEBUG) -rdynamic -m64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 

vpath %.cpp
vpath %.c

SOURCES1=main.cpp redis_wrapper.cpp  redis_client.cpp 

OBJS1= $(SOURCES1:.cpp=.o)

$(BIN):$(OBJS1)
	$(CC) $(CPPFLAGS) -o $(BIN) $(OBJS1)  $(INCLUDES) $(LIBS)

%.d:%.cpp
	@set -e; rm -f $@; \
$(CC) -$(M) $(CPPFLAGS) $< > $@.$$$$; \
sed 's/$*.o/$@ $*.o/g' < $@.$$$$ > $@;\
rm -f $@.$$$$;echo $@;

include $(SOURCES1:.cpp=.d)

clean:
	-\rm *.o *.d *.d.*

CFLAGS  = -g -Wall -fno-strict-aliasing 
LDFLAGS = -lpthread -lm
DEPDIR  = 
INCLUDE = -I./ -I../deps/lua-5.3.0/src -I../deps -I../
DEFINE  = -DUSE_BUFFER_POOL
LIBNAME = libchuck.a
MAKE    =
CC      =
LIBRARY = -L../deps/lua-5.3.0/src
HTTP_PARSER = ../deps/http-parser/libhttp_parser.a
# Platform-specific overrides
uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
ifeq ($(uname_S),Linux)
	MAKE += make
	CC += gcc
	DEFINE += -D_LINUX
	LDFLAGS += -lrt
	SHAREFLAGS += -shared
endif

ifeq ($(uname_S),Darwin)
	MAKE += make
	CC += clang
	DEFINE += -D_MACH
	SHAREFLAGS += -bundle -undefined dynamic_lookup	
endif

source.lib =  util/chk_log.c\
			  util/chk_timer.c\
			  util/chk_exception.c\
			  util/chk_bytechunk.c\
			  util/chk_string.c\
			  util/lookup8.c\
			  util/chk_signal.c\
			  util/chk_util.c\
			  http/chk_http.c\
			  lua/chk_lua.c\
			  socket/chk_stream_socket.c\
			  socket/chk_socket_helper.c\
			  socket/chk_acceptor.c\
			  socket/chk_connector.c\
			  event/chk_event_loop.c\
			  redis/chk_client.c\
			  thread/chk_thread.c

source.so =   util/chk_log.c\
			  util/chk_timer.c\
			  util/chk_exception.c\
			  util/chk_bytechunk.c\
			  util/chk_string.c\
			  util/lookup8.c\
			  util/chk_util.c\
			  util/chk_signal.c\
			  http/chk_http.c\
			  lua/chk_lua.c\
			  socket/chk_stream_socket.c\
			  socket/chk_socket_helper.c\
			  socket/chk_acceptor.c\
			  socket/chk_connector.c\
			  event/chk_event_loop.c\
			  redis/chk_client.c\
			  thread/chk_thread.c

$(LIBNAME):$(source.lib)
	$(CC) -fpic $(CFLAGS) -c $(source.lib) $(INCLUDE) $(DEFINE)
	ar -rc $(LIBNAME) *.o
	rm *.o

$(HTTP_PARSER):
	cd ../deps/http-parser/;$(MAKE) package

chuck.so:$(source.so) $(HTTP_PARSER)
	$(CC) -fpic $(CFLAGS) -c luabind/chuck.c $(source.so) $(INCLUDE) $(DEFINE)
	$(CC) $(CFLAGS) $(SHAREFLAGS) -o chuck.so *.o  $(HTTP_PARSER) $(JEMALLOC) $(LDFLAGS) $(LIBRARY)
	rm *.o

testcase:
	$(CC) $(CFLAGS) -o test/testlog test/testlog.c $(LIBNAME) $(INCLUDE) $(DEFINE) $(LDFLAGS)
	$(CC) $(CFLAGS) -o test/testexception test/testexception.c $(LIBNAME) $(INCLUDE) $(DEFINE) $(LDFLAGS)
	$(CC) $(CFLAGS) -o test/testhttppacket test/testhttppacket.c $(LIBNAME) ../deps/http-parser/libhttp_parser.a $(INCLUDE) $(DEFINE) $(LDFLAGS)	
	$(CC) $(CFLAGS) -o test/testtimer test/testtimer.c $(LIBNAME) $(INCLUDE) $(DEFINE) $(LDFLAGS)
	$(CC) $(CFLAGS) -o test/tcpecho test/tcpecho.c $(LIBNAME) $(INCLUDE) $(DEFINE) $(LDFLAGS)	
	$(CC) $(CFLAGS) -o test/test_objpool test/test_objpool.c $(LIBNAME) $(INCLUDE) $(DEFINE) $(LDFLAGS)	
	$(CC) $(CFLAGS) -o test/teststring test/teststring.c $(LIBNAME) $(INCLUDE) $(DEFINE) $(LDFLAGS)		
	$(CC) $(CFLAGS) -o test/test_bytebuffer test/test_bytebuffer.c $(LIBNAME) $(INCLUDE) $(DEFINE) $(LDFLAGS)	
	$(CC) $(CFLAGS) -o test/echobrocast test/echobrocast.c $(LIBNAME) $(INCLUDE) $(DEFINE) $(LDFLAGS)
	$(CC) $(CFLAGS) -o test/echoclient test/echoclient.c $(LIBNAME) $(INCLUDE) $(DEFINE) $(LDFLAGS)	
	$(CC) $(CFLAGS) -o test/testconnect test/testconnect.c $(LIBNAME) $(INCLUDE) $(DEFINE) $(LDFLAGS)
	$(CC) $(CFLAGS) -o test/testredis test/testredis.c $(LIBNAME) $(INCLUDE) $(DEFINE) $(LDFLAGS)						
	$(CC) $(CFLAGS) -o test/testlua test/testlua.c $(LIBNAME) $(INCLUDE) $(DEFINE) $(LDFLAGS) -llua -lm -ldl $(LIBRARY)

clean:
	rm *.o
	rm *.a
	rm *.so	
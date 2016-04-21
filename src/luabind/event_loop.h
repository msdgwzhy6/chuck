#include "event/chk_event_loop_define.h"

int32_t chk_loop_init(chk_event_loop *loop);

void chk_loop_finalize(chk_event_loop *loop);

#define EVENT_LOOP_METATABLE "lua_event_loop"

#define lua_checkeventloop(L,I)	\
	(chk_event_loop*)luaL_checkudata(L,I,EVENT_LOOP_METATABLE)

static int32_t lua_event_loop_gc(lua_State *L) {
	chk_event_loop *event_loop = lua_checkeventloop(L,1);
	chk_loop_finalize(event_loop);
	return 0;
}

static int32_t lua_new_event_loop(lua_State *L) {
	chk_event_loop *event_loop;
	event_loop = LUA_NEWUSERDATA(L,chk_event_loop);
	if(!event_loop) {
		return 0;
	}
	if(0 != chk_loop_init(event_loop)) return 0;
	luaL_getmetatable(L, EVENT_LOOP_METATABLE);
	lua_setmetatable(L, -2);
	return 1;
}

static int32_t lua_event_loop_run(lua_State *L) {
	chk_event_loop *event_loop;
	int32_t         ms,ret;
	event_loop = lua_checkeventloop(L,1);
	ms = (int32_t)luaL_optinteger(L,2,-1);
	if(ms == -1) ret = chk_loop_run(event_loop);
	else ret = chk_loop_run_once(event_loop,ms);
	if(ret != 0) {
		lua_pushinteger(L,ret);
		return 1;
	}
	return 0;
}

static int32_t lua_event_loop_end(lua_State *L) {
	chk_event_loop *event_loop = lua_checkeventloop(L,1);
	chk_loop_end(event_loop);
	return 0;
}

static int32_t lua_event_loop_addtimer(lua_State *L) {
	uint32_t       	ms;
	lua_timer      *luatimer;
	chk_luaRef      cb;
	chk_event_loop *event_loop = lua_checkeventloop(L,1);
	ms = (uint32_t)luaL_optinteger(L,2,1);
	if(!lua_isfunction(L,3)) 
		return luaL_error(L,"argument 3 of event_loop_addtimer must be lua function"); 
	cb = chk_toluaRef(L,3);
	luatimer = (lua_timer*)lua_newuserdata(L, sizeof(*luatimer));
	luatimer->cb = cb;
	luatimer->timer = chk_loop_addtimer(event_loop,ms,lua_timeout_cb,luatimer);
	if(luatimer->timer) chk_timer_set_ud_cleaner(luatimer->timer,timer_ud_cleaner);
	else chk_luaRef_release(&luatimer->cb);
	luaL_getmetatable(L, TIMER_METATABLE);
	lua_setmetatable(L, -2);	
	return 1;
}

static void signal_ud_dctor(void *ud) {
	chk_luaRef *cb = (chk_luaRef*)ud;
	chk_luaRef_release(cb);
	free(cb);
}

static void signal_callback(void *ud) {
	chk_luaRef *cb = (chk_luaRef*)ud;
	const char   *error; 
	if(NULL != (error = chk_Lua_PCallRef(*cb,":"))) {
		CHK_SYSLOG(LOG_ERROR,"error on signal_cb %s",error);
	}	
}

static int32_t lua_watch_signal(lua_State *L) {
	chk_event_loop *event_loop = lua_checkeventloop(L,1);
	int32_t signo = (int32_t)luaL_checkinteger(L,2);
	if(!lua_isfunction(L,3))
		return luaL_error(L,"argument 3 must be lua function");

	chk_luaRef *cb = calloc(1,sizeof(*cb));
	if(!cb) {
		lua_pushstring(L,"no memory");
		return 1;
	}
	*cb = chk_toluaRef(L,3);
	if(0 != chk_watch_signal(event_loop,signo,signal_callback,cb,signal_ud_dctor)) {
		signal_ud_dctor(cb);
		printf("call chk_watch_signal failed\n");
		return 0;
	}

	return 0; 
}

static int32_t lua_unwatch_signal(lua_State *L) {
	int32_t signo = (int32_t)luaL_checkinteger(L,2);
	chk_unwatch_signal(signo);	
	return 0;
}

static void register_event_loop(lua_State *L) {
	luaL_Reg event_loop_mt[] = {
		{"__gc", lua_event_loop_gc},
		{NULL, NULL}
	};

	luaL_Reg event_loop_methods[] = {
		{"WatchSignal",  lua_watch_signal},
		{"UnWatchSignal",lua_unwatch_signal},
		{"Run",    	     lua_event_loop_run},
		{"Stop",         lua_event_loop_end},
		{"AddTimer",     lua_event_loop_addtimer},
		{NULL,     NULL}
	};

	luaL_newmetatable(L, EVENT_LOOP_METATABLE);
	luaL_setfuncs(L, event_loop_mt, 0);

	luaL_newlib(L, event_loop_methods);
	lua_setfield(L, -2, "__index");
	lua_pop(L, 1);

	lua_newtable(L);
	SET_FUNCTION(L,"New",lua_new_event_loop);
}








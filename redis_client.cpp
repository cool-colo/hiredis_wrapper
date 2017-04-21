#include "common.h"
#include "redis_client.h"

//RedisClient g_redis_client_list;
//RedisClient g_redis_client_queue;


void RedisClient::delete_client(void *p) {
    delete ((RedisClient*)p);
}

bool RedisClient::init(const std::string& host, const int port, const std::string& auth, const int dbnum) {
    redis_->SetRedisAddr(host, port, auth, dbnum);
    if (redis_->connect()) {
        AC_ERROR("connect redis error, host: %s, port: %d, dbnum: %d", host.c_str(), port, dbnum);
        return false;
    }
    _host = host;
    _port = port;
    _auth = auth;
    _dbnum = dbnum;


    return true;
}



RedisClient::RedisDriver RedisClient::get_driver()
{
    return redis_;
}

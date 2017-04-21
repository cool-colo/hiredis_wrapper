#pragma once
#include <stdlib.h>
#include <string.h>
#include "hiredis/hiredis.h"
#include <string>
#include <memory>
#include <vector>
//using namespace::std;

class RedisWrapper
{
    public:
    using RedisReply = std::shared_ptr<redisReply>;
        RedisWrapper(const std::string& host, int port, const std::string& auth="");
        RedisWrapper();
        virtual ~RedisWrapper();
        int connect();
        RedisReply command(const std::vector<std::string>& cmd);
        int pipeline_append_command(const std::vector<std::string>& cmd);
        RedisReply pipeline_get_reply();
        int SetRedisAddr(const std::string& host, int port, const std::string& auth, int dbnum);
        int generate_redis_command(const std::vector<std::string>& cmd, std::vector<const char *>& argv, std::vector<size_t>& argvlen);
        int get_port();
        std::string get_host();

    private:
        std::string host_;
        int port_;
        int dbnum_;
        std::string auth_;
        redisContext* context_;
};



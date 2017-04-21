#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "redis_wrapper.h"
#include "lexical_cast_wrapper.h"
#include "redis_reply_inner_get.h"
#include "redis_pipeline.h"
#include "redis_command_executor.h"
#include "redis_command.h"


class RedisClient
{
    public:
        using RedisDriver = std::shared_ptr<RedisWrapper>;
        RedisClient() : redis_(std::make_shared<RedisWrapper>()) { }

        RedisClient(const std::string& host, int port, const std::string& auth="")
            : redis_(std::make_shared<RedisWrapper>(host, port, auth))
        {

        }
        virtual ~RedisClient()
        {
        }

    public:
        static void delete_client(void *p);
        bool init(const std::string& host, const int port, const std::string& auth, const int dbnum);
//        bool init(const RedisConfig& config);
        RedisClient *get_client();
        RedisDriver get_driver();
        


    private:
        RedisDriver redis_;

        std::string         _host;
        int                 _port;
        std::string         _auth;
        int                 _dbnum;
};

//multiple single instance for same class
template <typename T, int I>
struct MultipleInstance : public T
{
};

/*
#define g_redis_client_list talus::Singleton<MultipleInstance<RedisClient,0>>::instance()
#define g_redis_client_queue talus::Singleton<MultipleInstance<RedisClient,1>>::instance()
#define g_redis_client_comment talus::Singleton<MultipleInstance<RedisClient,2>>::instance()
*/

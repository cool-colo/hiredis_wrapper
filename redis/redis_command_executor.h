#pragma once
#include <memory>
#include "redis_wrapper.h"
#include "redis_pipeline.h"
#include "common.h"

class RedisCommandExecutor
{
    using RedisDriver = std::shared_ptr<RedisWrapper>;
    RedisDriver driver_;
    
    std::vector<RCommands> commands_;
    RedisPipeline pipeline_;
public:
    template<typename ...Args>
    RedisCommandExecutor(RedisDriver driver, Args... args): driver_(driver), pipeline_(driver)
    {
    }

    RedisPipeline&  pipeline()
    {
         return pipeline_;
    } 

    template<typename Command, bool Reply, typename ReplyType, typename ...Args>
    typename std::enable_if<Reply, int>::type execute( ReplyType& reply, Args... args) 
    {
        auto cmd = Command()(std::forward<Args>(args)...);  
        
        RedisWrapper::RedisReply redisReply= driver_->command(cmd);
        if (RedisReplyGet<ReplyType>::get(redisReply, reply) < 0)
        {
            return -1;
        }
        return 0;
    }

    template<typename Command, bool Reply, typename ...Args>
    typename std::enable_if<!Reply, int>::type execute(Args... args) 
    {
        auto cmd = Command()(std::forward<Args>(args)...);  
        RedisWrapper::RedisReply redisReply= driver_->command(cmd);
        if (RedisReplyGet<NoReply>::get(redisReply, NoReply()) < 0)
        {
            return -1;           
        }
        return 0;
    }
};

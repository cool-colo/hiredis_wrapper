#pragma once
#include <memory>
#include "redis_wrapper.h"
#include "common.h"

class RedisPipeline
{
    using self = RedisPipeline;
    using RedisDriver = std::shared_ptr<RedisWrapper>;
    RedisDriver driver_;
    std::vector<RCommands> commands_;
public:
    template<typename ...Args>
    RedisPipeline(RedisDriver driver, Args... args): driver_(driver)
    {
    }
    
    template<typename Command, typename ...Args>
    self& append(Args... args) {
	auto cmd = Command()(std::forward<Args>(args)...);
        commands_.push_back(cmd);
        return *this;
    }   

    template<typename ReplyType>
    int execute(std::vector<ReplyType> &replies) 
    {
        int ret = 0;
        ret = build_commands();
        IF_ERROR_RETURN(ret);
        
        ret = get_replies(replies);
        IF_ERROR_RETURN(ret);        
        
        return 0;
    }

    int execute() 
    {
        int ret = 0;
        ret = build_commands();
        IF_ERROR_RETURN(ret);
        
        ret = get_replies();
        IF_ERROR_RETURN(ret);        
        
        return 0;
    }

private:
    int build_commands()
    {
        for (auto & cmd : commands_)
        {
            if (driver_->pipeline_append_command(cmd) < 0)
            {   
                return -1;
            }  
        }
        return 0;
    }

    int get_replies()
    {
        std::vector<NoReply> replies;
        return get_replies(replies);
    }

    template<typename ReplyType>
    int get_replies(std::vector<ReplyType> &replies)
    {
        replies.clear();
        replies.resize(commands_.size());
        for (int i = 0; i < commands_.size(); i++)
        {
            RedisWrapper::RedisReply reply= driver_->pipeline_get_reply();
            if (RedisReplyGet<ReplyType>::get(reply, replies[i]) < 0)
            {
                return -1;
            }
        }
	commands_.clear();
        return 0;
    }
};

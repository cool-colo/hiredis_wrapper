#include "redis_wrapper.h"
#include "common.h"

RedisWrapper::RedisWrapper() : context_(nullptr)
{
}
RedisWrapper::RedisWrapper(const std::string& host, int port, const std::string& auth)
    :host_(host), port_(port), auth_(auth), context_(nullptr) 
{
    host_ = host,
    port_ = port, 
    auth_ = auth;
}

RedisWrapper::~RedisWrapper()
{
    if(context_)
    {
        redisFree(context_);
        context_ = nullptr;
    }
}

int RedisWrapper::SetRedisAddr(const std::string& host, int port, const std::string& auth, int dbnum)
{
    host_ = host,
    port_ = port, 
    auth_ = auth;
    dbnum_ = dbnum;
    return 0;
}

int RedisWrapper::connect()
{
    if(context_)
    {
        redisFree(context_);
        context_ = nullptr;
    }

    redisReply *reply = nullptr;
    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    context_ = redisConnectWithTimeout(host_.c_str(), port_, timeout);
    if (context_ == nullptr || context_->err )
    {
        if (context_)
        {
            printf("RedisWrapper Connection error: %s\n", context_->errstr);
            redisFree(context_);
            context_ = nullptr;
        }
        else
        {
            printf("RedisWrapper Connection error: can't allocate redis context\n");
        }
        return -1;
    }
    redisSetTimeout(context_, timeout);
    redisEnableKeepAlive(context_);
    if(!auth_.empty())
    {
        reply = (redisReply *)redisCommand(context_, "AUTH %s", auth_.c_str());
        if (reply) {
            freeReplyObject(reply);
        } else {
            redisFree(context_);
            context_ = NULL;
            return -1;
        }
    }

    reply = (redisReply *)redisCommand(context_, "SELECT %d", dbnum_);
    if (reply) {
        freeReplyObject(reply);
    } else {
        redisFree(context_);
        context_ = NULL;
        return -1;
    }
    
    return 0;
}
RedisWrapper::RedisReply RedisWrapper::command(const std::vector<std::string>& cmd)
{
    redisReply* reply= nullptr;
    if(!context_) connect();
    if(!context_) return std::shared_ptr<redisReply>(nullptr);
    //AC_DEBUG("execute command:[%s] on [%s:%d:%d]", cmd.c_str(), host_.c_str(), port_, dbnum_);

	std::vector<const char *> argv;
	std::vector<size_t> argvlen;
	if (generate_redis_command(cmd, argv, argvlen) < 0)
	{
	    return std::shared_ptr<redisReply>(nullptr);
	}
		

    reply = (redisReply*)redisCommandArgv(context_, static_cast<int>(cmd.size()), &(argv[0]), &(argvlen[0]));
    if (!reply)
    {
        if (!connect() )
            reply = (redisReply*)redisCommandArgv(context_, static_cast<int>(cmd.size()), &(argv[0]), &(argvlen[0]));
    }

    return std::shared_ptr<redisReply>(reply, [](redisReply* reply){ if (reply != nullptr) {freeReplyObject(reply);} });
}

int RedisWrapper::pipeline_append_command(const std::vector<std::string>& cmd)
{
    if (!context_) connect();
    if (!context_) return -1;

	std::vector<const char *> argv;
	std::vector<size_t> argvlen;
	RETURN_IF_ERROR(generate_redis_command(cmd, argv, argvlen));
	
    if (REDIS_OK != redisAppendCommandArgv(context_, static_cast<int>(cmd.size()), &(argv[0]), &(argvlen[0])))
    {
        return -1;
    }
    //AC_DEBUG("pipeline append command: %s", cmd.c_str());
    return 0;
}

RedisWrapper::RedisReply RedisWrapper::pipeline_get_reply()
{
    redisReply* reply = NULL;
    if (REDIS_OK != redisGetReply(context_,(void**)&reply)) 
    {
        freeReplyObject(reply);
        std::shared_ptr<redisReply>(nullptr);
    }
    return std::shared_ptr<redisReply>(reply, [](redisReply* reply){ if (reply != nullptr) {freeReplyObject(reply);} });
}

int RedisWrapper::generate_redis_command(const std::vector<std::string>& cmd, std::vector<const char *>& argv, std::vector<size_t>& argvlen)
{
    if (cmd.empty())
	{
        AC_ERROR("empty redis command");
		return -1;
    }

    for (const auto& element : cmd)
    {
        argv.push_back(element.c_str());
		argvlen.push_back(element.size());
    }
	return 0;
}
int RedisWrapper::get_port()
{
    return port_;
}
std::string RedisWrapper::get_host()
{
    return host_;

}



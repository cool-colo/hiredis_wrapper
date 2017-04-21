# hiredis_wrapper
c++ wrapper for hiredis

用C++对hiredis进行简单封装，以方便使用, 支持pipeline.
所有命令定义在redis/redis_command.h中,目前只写了几个项目中用到的。

使用示例如下：

#include "redis_client.h"
#include "redis_command_executor.h"
int main()
{
    RedisClient client;
    client.init("127.0.0.1", 6379, "", 0);

    RedisCommandExecutor cmd(client.get_driver());

    RedisPipeline& pipeline = cmd.pipeline();

    std::string listKey = "list";
    std::string setKey = "set";
    std::string zsetKey = "zset";

    std::unordered_map<std::string, long> keyScoreMap = {{"key1", 400}, {"key2", 500}};

    pipeline.append<zadd>(zsetKey, "str1", 100);
    pipeline.append<zadd>(zsetKey, 200, 200);
    pipeline.append<zadd>(zsetKey, "member1", "300");
    pipeline.append<zadd>(zsetKey, keyScoreMap);

    pipeline.append<lpush>(listKey, "list1");
    pipeline.append<lpush>(listKey, 100);
    pipeline.append<lpush>(listKey, 2.001);

    pipeline.append<sadd>(setKey, "set1");
    pipeline.append<sadd>(setKey, std::string("set2"));
    pipeline.append<sadd>(setKey, 100);

    pipeline.execute();

    std::vector<std::pair<std::string,uint64_t>> result;
    int ret = cmd.execute<zrevrangebyscore<true>, true>(result, zsetKey, "+inf", "0", 0, 20);
    for (const auto& data : result)
    {
        std::cout<<"member: "<<data.first<<"; "<<"score: "<<data.second<<std::endl;
    }
    
    //上面结果输出如下：
    //member: key2; score: 500
    //member: key1; score: 400
    //member: member1; score: 300
    //member: 200; score: 200
    //member: str1; score: 100

////////////////////////////////////////////////////////////////////////////////////////////////
    pipeline.append<sismember>(setKey, "set1");
    pipeline.append<sismember>(setKey, "set2");
    pipeline.append<sismember>(setKey, "100");
    pipeline.append<sismember>(setKey, "200");


    std::vector<int> result2;
    pipeline.execute(result2);
    for (const auto& data : result2)
    {
        std::cout<<data<<";";
    }
    std::cout<<std::endl;
    //上面结果输出如下
    //1;1;1;0;

////////////////////////////////////////////////////////////////////////////////////////////////
    std::string lua_script = "return {KEYS[1],KEYS[2],ARGV[1],ARGV[2]}";
    std::vector<std::pair<std::string, std::string>> result3;
    ret = cmd.execute<eval, true>(result3, lua_script, 2, "KEY1", "KEY2", "VAL1","VAL2");
    for (const auto& data : result3)
    {
        std::cout<<"first: "<<data.first<<"; "<<"second: "<<data.second<<std::endl;;
    }
    //上面结果输出如下    
    //first: KEY1; second: KEY2
    //first: VAL1; second: VAL2

////////////////////////////////////////////////////////////////////////////////////////////////
    std::string lua_script2 = "return {KEYS[1],KEYS[2],KEYS[3], ARGV[1],ARGV[2],ARGV[3]}";
    std::vector<std::string> result4;
    ret = cmd.execute<eval, true>(result4, lua_script2, 3, "KEY1", "KEY2", "KEY3", "VAL1","VAL2", "VAL3");
    for (const auto& data : result4)
    {
        std::cout<<"data: "<<data<<";";
    }
    std::cout<<std::endl;
    //上面结果输出如下
    //data: KEY1;data: KEY2;data: KEY3;data: VAL1;data: VAL2;data: VAL3;
    
////////////////////////////////////////////////////////////////////////////////////////////////
    pipeline.append<eval>(lua_script, 2, "11", "22", "33","44");
    pipeline.append<eval>(lua_script2, 3, 1, 2, 3, 4, 5, 6);
    std::vector<std::vector<int>> result5;
    pipeline.execute(result5);

    for (const auto& vec : result5)
    {
        for (const auto& data : vec)
        {
            std::cout<<"data: "<<data<<";";
        }
        std::cout<<std::endl;
    }
    //上面结果输出如下    
    //data: 11;data: 22;data: 33;data: 44;
    //data: 1;data: 2;data: 3;data: 4;data: 5;data: 6;
}




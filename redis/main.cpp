#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <tuple>
#include <type_traits>
#include <utility>
#include <atomic>
#include <future>
#include <type_traits>
#include <vector>
#include <iostream>
#include <functional>
#include <unordered_map>
#include "redis_client.h"
#include "redis_command_executor.h"

template<typename F, typename S>
void display(const std::vector<std::pair<F, S>>& container)
{
    for (const auto& element : container)
    {
        std::cout<<"first:"<<element.first<<";"<<"second:"<<element.second<<std::endl;
    }
}



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

////////////////////////////////////////////////////////////////////////////////////////////////
    std::string lua_script = "return {KEYS[1],KEYS[2],ARGV[1],ARGV[2]}";
    std::vector<std::pair<std::string, std::string>> result3;
    ret = cmd.execute<eval, true>(result3, lua_script, 2, "KEY1", "KEY2", "VAL1","VAL2");
    for (const auto& data : result3)
    {
        std::cout<<"first: "<<data.first<<"; "<<"second: "<<data.second<<std::endl;;
    }

////////////////////////////////////////////////////////////////////////////////////////////////
    std::string lua_script2 = "return {KEYS[1],KEYS[2],KEYS[3], ARGV[1],ARGV[2],ARGV[3]}";
    std::vector<std::string> result4;
    ret = cmd.execute<eval, true>(result4, lua_script2, 3, "KEY1", "KEY2", "KEY3", "VAL1","VAL2", "VAL3");
    for (const auto& data : result4)
    {
        std::cout<<"data: "<<data<<";";
    }
    std::cout<<std::endl;

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

}




#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <type_traits>
#include "common.h"


using RCommands = std::vector<std::string>;

struct CommandBuilder
{
    template <typename T>
    CommandBuilder& operator << (const T& data)
    {
        stringStream.str("");
        stringStream <<data;
        commands.push_back(stringStream.str());
        return *this;   
    }

    RCommands get()
    {
        return std::move(commands);
    }

    std::ostringstream stringStream;
    RCommands commands;
};



struct NoReply{};

template< class T >
using remove_reference_t = typename std::remove_reference<T>::type;


#define RealType(v) remove_reference_t<decltype(v)>


struct NX{};
struct XX{};
struct PX{};
struct EX{};
struct WITHSCORES{};
struct set
{ 
    template<typename T>
    RCommands operator()(const std::string& key, const T& value)
    {
          CommandBuilder command_builder;
          command_builder << "SET" << key << value;
          return std::move(command_builder.get());
    }

    template<typename T, typename ET>
    RCommands operator()(const std::string& key, const T& value, EX&&, ET expires_in)
    {
          CommandBuilder command_builder;
          command_builder << "SET" << key << "EX" << value << expires_in;
          return std::move(command_builder.get());
    }

    template<typename T, typename PT>
    RCommands operator()(const std::string& key, const T& value, PX&&, PT expires_in)
    {
          CommandBuilder command_builder;
          command_builder << "SET" << key << "PX" << value << expires_in;
          return std::move(command_builder.get());
    }

    template<typename T>
    RCommands operator()(const std::string& key, const T& value, NX&&)
    {
          CommandBuilder command_builder;
          command_builder << "SET" << key << value << "NX";
          return std::move(command_builder.get());
    }

    template<typename T>
    RCommands operator()(const std::string& key, const T& value, XX&&)
    {
          CommandBuilder command_builder;
          command_builder << "SET" << key << value << "XX";
          return std::move(command_builder.get());
    }

    template<typename T, typename ET>
    RCommands operator()(const std::string& key, const T& value, EX&&, ET expires_in, NX&&)
    {
          CommandBuilder command_builder;
          command_builder << "SET" << key << value << "EX" << expires_in << "NX";
          return std::move(command_builder.get());
    }

    template<typename T, typename ET>
    RCommands operator()(const std::string& key, const T& value, EX&&, ET expires_in, XX&&)
    {
          CommandBuilder command_builder;
          command_builder << "SET" << key << value << "EX" << expires_in << "XX";
          return std::move(command_builder.get());
    }

    template<typename T, typename PT>
    RCommands operator()(const std::string& key, const T& value, PX&&, PT expires_in, NX&&)
    {
          CommandBuilder command_builder;
          command_builder << "SET" << key << value << "PX" << expires_in << "NX";
          return std::move(command_builder.get());
    }

    template<typename T, typename PT>
    RCommands operator()(const std::string& key, const T& value, PX&&, PT expires_in, XX&&)
    {
          CommandBuilder command_builder;
          command_builder << "SET" << key << value << "PX" << expires_in << "XX";
          return std::move(command_builder.get());
    }
};

struct get
{
    template<typename T>
    RCommands operator()(const std::string& key)
    {
          CommandBuilder command_builder;
          command_builder << "get" << key;
          return std::move(command_builder.get());
    }
};

struct sadd
{
    template<typename T>
    RCommands operator()(const std::string& key, const std::vector<T>& members)
    {
        CommandBuilder command_builder;
        command_builder << "SADD" << key;
        for (const auto& member : members)
        {
          command_builder << member;
        }
        return std::move(command_builder.get());
    }

    template<typename T>
    RCommands operator()(const std::string& key, const T& val)
    {
        std::vector<T> members{{val}};
        return operator()(key, members);
    }
  
};


struct sismember
{
    template<typename T>
    RCommands operator()(const std::string& key, const T& member)
    {
        CommandBuilder command_builder;
        command_builder << "SISMEMBER" << key << member;
        return std::move(command_builder.get());
    }
};


struct zrem
{
    template<typename T>
    RCommands operator()(const std::string& key, const std::vector<T>& container)
    {
        CommandBuilder command_builder;
        command_builder << "ZREM" << key;
        for (const auto& member : container)
        {
          command_builder << member;
        }
        return std::move(command_builder.get());
    }

    template<typename T>
    RCommands operator()(const std::string& key, const T& val)
    {
        std::vector<T> container{{val}};
        return operator()(key, container);
    }
  
};


struct zcard
{
    RCommands operator()(const std::string& key)
    {
        CommandBuilder command_builder;
        command_builder << "ZCARD" << key;
        return std::move(command_builder.get());
    }
};

struct zadd
{
    template<typename MemberType, typename ScoreType>
    RCommands operator()(const std::string& key, const std::vector<std::pair<MemberType, ScoreType>>& members)
    {
        CommandBuilder command_builder;
        command_builder << "ZADD" << key;
        for (const auto& member : members)
        {
            command_builder << member.second << member.first;
        }
        return std::move(command_builder.get());
    }
    template<typename MemberType, typename ScoreType>
    RCommands operator()(const std::string& key, const std::unordered_map<MemberType, ScoreType>& members)
    {
        CommandBuilder command_builder;
        command_builder << "ZADD" << key;
        for (const auto& member : members)
        {
            command_builder << member.second << member.first;
        }
        return std::move(command_builder.get());
    }

    template<typename MemberType, typename ScoreType>
    RCommands operator()(const std::string& key, const MemberType& value, const ScoreType& score)
    {       
        std::vector<std::pair<MemberType, ScoreType>> members = {{value, score}};
        return operator()(key, members);

    }
};


struct lpush
{
    template<typename T>
    RCommands operator()(const std::string& key, const std::vector<T>& values)
    {
        CommandBuilder command_builder;
        command_builder << "LPUSH" << key;
        for (const auto& value : values) {
            command_builder << value;
        }
        return std::move(command_builder.get());
    }

    template<typename T>
    RCommands operator()(const std::string& key, const T& value)
    {       
        std::vector<T> container{{value}};
        return operator()(key, container);

    }
};


template <bool WithScores>
struct zrevrange{};

template <>
struct zrevrange<false>
{
    RCommands operator()(const std::string& key, int start, int stop)
    {
        CommandBuilder command_builder;
        command_builder << "ZREVRANGE" << key << start << stop;
        return std::move(command_builder.get());
    }
};


template <>
struct zrevrange<true>
{
    RCommands operator()(const std::string& key, int start, int stop)
    {
        CommandBuilder command_builder;
        command_builder << "ZREVRANGE" << key << start << stop << "WITHSCORES";
        return std::move(command_builder.get());
    }

};

template <bool WithScores>
struct zrevrangebyscore {};

template <>
struct zrevrangebyscore<false>
{
    
    RCommands operator()(const std::string& key, const std::string& max, const std::string& min)
    {
        CommandBuilder command_builder;
        command_builder << "ZREVRANGEBYSCORE" << key << max <<min;
        return std::move(command_builder.get());
    }

    RCommands operator()(const std::string& key, const std::string& max, const std::string& min, int offset, int count)
    {
        CommandBuilder command_builder;
        command_builder << "ZREVRANGEBYSCORE" << key << max << min;
        command_builder << "LIMIT" << offset << count;
        return std::move(command_builder.get());
    }
};

template <>
struct zrevrangebyscore<true>
{
    
    RCommands operator()(const std::string& key, const std::string& max, const std::string& min)
    {
        CommandBuilder command_builder;
        command_builder << "ZREVRANGEBYSCORE" << key << max << min << "WITHSCORES";
        return std::move(command_builder.get());
    }

    RCommands operator()(const std::string& key, const std::string& max, const std::string& min, int offset, int count)
    {
        CommandBuilder command_builder;
        command_builder << "ZREVRANGEBYSCORE" << key << max << min << "WITHSCORES";
        command_builder << "LIMIT" << offset << count;
        return std::move(command_builder.get());
    }
};


template <bool WithScores>
struct zrangebyscore{};

template <>
struct zrangebyscore<false>
{
    RCommands operator()(const std::string& key, const std::string& min, const std::string& max)
    {
        CommandBuilder command_builder;
        command_builder << "ZRANGEBYSCORE" << key << min <<max;
        return std::move(command_builder.get());
    }

    RCommands operator()(const std::string& key, const std::string& min, const std::string& max, int offset, int count)
    {
        CommandBuilder command_builder;
        command_builder << "ZRANGEBYSCORE" << key << min << max;
        command_builder << "LIMIT" << offset << count;
        return std::move(command_builder.get());
    }
};

template <>
struct zrangebyscore<true>
{
    RCommands operator()(const std::string& key, const std::string& min, const std::string& max)
    {
        CommandBuilder command_builder;
        command_builder << "ZRANGEBYSCORE" << key << min << max << "WITHSCORES";
        return std::move(command_builder.get());
    }

    RCommands operator()(const std::string& key, const std::string& min, const std::string& max, int offset, int count)
    {
        CommandBuilder command_builder;
        command_builder << "ZRANGEBYSCORE" << key << min << max << "WITHSCORES";
        command_builder << "LIMIT" << offset << count;
        return std::move(command_builder.get());
    }
};

    

struct zincrby
{
    template<typename ElemType, typename ScoreType>
    RCommands operator()(const std::string& key, const ElemType& memeber, const ScoreType& increment)
    {
        CommandBuilder command_builder;
        command_builder << "ZINCRBY" << key << increment <<memeber;
        return std::move(command_builder.get());
    }

};

struct hmset
{ 
    template<typename FieldType, typename ValueType>
    RCommands operator()(const std::string& key, const std::vector<std::pair<FieldType,ValueType>>& avps)
    {
        CommandBuilder command_builder;
        command_builder << "HMSET" << key;
        for (const auto& avp : avps)
        {
            command_builder << avp.first << avp.second;
        }
        return std::move(command_builder.get());
    }

    template<typename FieldType, typename ValueType>
    RCommands operator()(const std::string& key, const std::unordered_map<FieldType,ValueType>& avps)
    {
        CommandBuilder command_builder;
        command_builder << "HMSET" << key;
        for (const auto& avp : avps)
        {
            command_builder << avp.first <<avp.second;
        }
        return std::move(command_builder.get());
    }
};


struct hmget
{ 
    template<typename FieldType>
    RCommands operator()(const std::string& key, const std::vector<FieldType>& fields)
    {
        CommandBuilder command_builder;
        command_builder << "HMGET" << key;
        for (const auto& field : fields)
        {
          command_builder << field;
        }
        return std::move(command_builder.get());
    }
};


struct expire
{
    RCommands operator()(const std::string& key, long seconds)
    {
        CommandBuilder command_builder;
        command_builder << "EXPIRE" << key << seconds;;
        return std::move(command_builder.get());
    }
};

struct expireat
{
    RCommands operator()(const std::string& key, long timestamp)
    {
        CommandBuilder command_builder;
        command_builder << "EXPIREAT" << key << timestamp;
        return std::move(command_builder.get());
    }
};


struct eval
{
    template <typename ... ARGS>
    RCommands operator()(const std::string& script, ARGS&& ... args)
    {   
        CommandBuilder command_builder;
        [](...){} ((command_builder<<std::forward<ARGS>(args),0)...);
        command_builder << script << "EVAL";
        auto cmd = command_builder.get();
        std::reverse(cmd.begin(), cmd.end());
        return std::move(cmd);
    }
};


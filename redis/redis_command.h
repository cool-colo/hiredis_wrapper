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

template <typename T>
struct CommandBuildHelper{
    static void Build (RCommands& commands, const T& data){
        std::ostringstream stringStream;
        stringStream << data;
        commands.push_back(stringStream.str());
    }
};

template <typename T>
struct CommandBuildHelper<std::vector<T>>{
    static void Build (RCommands& commands, const std::vector<T>& data){
        std::ostringstream stringStream;
        std::for_each(std::begin(data), std::end(data), [&](const T& element){ 
                             stringStream.str("");
                             stringStream << element;
                             commands.push_back(stringStream.str());
                         }
                     );
    }
};

template<typename FirstType, typename SecondType>
struct CommandBuildHelper<std::vector<std::pair<FirstType, SecondType>>>{
    static void Build (RCommands& commands, const std::vector<std::pair<FirstType, SecondType>>& data){
        std::ostringstream stringStream;
        std::for_each(std::begin(data), std::end(data), [&](const std::pair<FirstType, SecondType>& element){ 
                             stringStream.str("");
                             stringStream << element.first;
                             commands.push_back(stringStream.str());

                             stringStream.str("");
                             stringStream << element.second;
                             commands.push_back(stringStream.str());
                         }
                     );
    }
};

template<typename KeyType, typename ValueType>
struct CommandBuildHelper<std::unordered_map<KeyType,ValueType>>{
    static void Build (RCommands& commands, const std::unordered_map<KeyType,ValueType>& data){
        std::ostringstream stringStream;
        std::for_each(std::begin(data), std::end(data), [&](const std::pair<KeyType, ValueType>& element){ 
                             stringStream.str("");
                             stringStream << element.first;
                             commands.push_back(stringStream.str());

                             stringStream.str("");
                             stringStream << element.second;
                             commands.push_back(stringStream.str());
                         }
                     );
    }
};

struct CommandBuilder
{
    template <typename ... ARGS>
    CommandBuilder(ARGS&& ... args){
        std::vector<bool> list = {(this->operator<<(std::forward<ARGS>(args)), true)...};
        (void)list;
    }

    template <typename T>
    CommandBuilder& operator << (const T& data)
    { 
        CommandBuildHelper<T>::Build(commands, data);
        return *this;
    }

    RCommands get()
    {
        return std::move(commands);
    }
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
        return CommandBuilder{"SET", key, value}.get();
    }

    template<typename T, typename ET>
    RCommands operator()(const std::string& key, const T& value, EX&&, ET expires_in)
    {
        return CommandBuilder{"SET", key, "EX", value, expires_in}.get();
    }

    template<typename T, typename PT>
    RCommands operator()(const std::string& key, const T& value, PX&&, PT expires_in)
    {
        return CommandBuilder{"SET", key, "PX", value, expires_in}.get();
    }

    template<typename T>
    RCommands operator()(const std::string& key, const T& value, NX&&)
    {
        return CommandBuilder{"SET", key, value, "NX"}.get();
    }

    template<typename T>
    RCommands operator()(const std::string& key, const T& value, XX&&)
    {
        return CommandBuilder{"SET", key, value, "XX"}.get();
    }

    template<typename T, typename ET>
    RCommands operator()(const std::string& key, const T& value, EX&&, ET expires_in, NX&&)
    {
        return CommandBuilder{"SET", key, value, "EX", expires_in, "NX"}.get();
    }

    template<typename T, typename ET>
    RCommands operator()(const std::string& key, const T& value, EX&&, ET expires_in, XX&&)
    {
        return CommandBuilder{"SET", key, value, "EX", expires_in, "XX"}.get();
    }

    template<typename T, typename PT>
    RCommands operator()(const std::string& key, const T& value, PX&&, PT expires_in, NX&&)
    {
        return CommandBuilder{"SET", key, value, "PX", expires_in, "NX"}.get();
    }

    template<typename T, typename PT>
    RCommands operator()(const std::string& key, const T& value, PX&&, PT expires_in, XX&&)
    {
        return CommandBuilder{"SET", key, value, "PX", expires_in, "XX"}.get();
    }
};

struct get
{
    template<typename T>
    RCommands operator()(const std::string& key)
    {
        return CommandBuilder{"GET", key}.get();
    }
};

struct sadd
{
    template<typename T>
    RCommands operator()(const std::string& key, const std::vector<T>& members)
    {
        return CommandBuilder{"SADD", key, members}.get();
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
        return CommandBuilder{"SISMEMBER", key, member}.get();
    }
};


struct zrem
{
    template<typename T>
    RCommands operator()(const std::string& key, const std::vector<T>& members)
    {
        return CommandBuilder{"ZREM", key, members}.get();
    }

    template<typename T>
    RCommands operator()(const std::string& key, const T& val)
    {
        std::vector<T> members{{val}};
        return operator()(key, members);
    }
  
};


struct zcard
{
    RCommands operator()(const std::string& key)
    {
        return CommandBuilder{"ZCARD", key}.get();
    }
};

struct zadd
{
    template<typename ScoreType, typename MemberType>
    RCommands operator()(const std::string& key, const std::vector<std::pair<ScoreType, MemberType>>& members)
    {
        return CommandBuilder{"ZADD", key, members}.get();
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

    template<typename ScoreType, typename MemberType>
    RCommands operator()(const std::string& key, const ScoreType& score, const MemberType& value)
    {       
        std::vector<std::pair<ScoreType, MemberType>> members = {{score, value}};
        return operator()(key, members);
    }
};


struct lpush
{
    template<typename T>
    RCommands operator()(const std::string& key, const std::vector<T>& values)
    {
        return CommandBuilder{"LPUSH", key, values}.get();
    }

    template<typename T>
    RCommands operator()(const std::string& key, const T& value)
    {       
        std::vector<T> members{{value}};
        return operator()(key, members);
    }
};


template <bool WithScores>
struct zrevrange{};

template <>
struct zrevrange<false>
{
    RCommands operator()(const std::string& key, int start, int stop)
    {
        return CommandBuilder{"ZREVRANGE", key, start, stop}.get();
    }
};


template <>
struct zrevrange<true>
{
    RCommands operator()(const std::string& key, int start, int stop)
    {
        return CommandBuilder{"ZREVRANGE", key, start, stop, "WITHSCORES"}.get();
    }

};

template <bool WithScores>
struct zrevrangebyscore {};

template <>
struct zrevrangebyscore<false>
{
    
    RCommands operator()(const std::string& key, const std::string& max, const std::string& min)
    {
        return CommandBuilder{"ZREVRANGEBYSCORE", key, max, min}.get();
    }

    RCommands operator()(const std::string& key, const std::string& max, const std::string& min, int offset, int count)
    {
        return CommandBuilder{"ZREVRANGEBYSCORE", key, max, min, "LIMIT", offset, count}.get();
    }
};

template <>
struct zrevrangebyscore<true>
{
    
    RCommands operator()(const std::string& key, const std::string& max, const std::string& min)
    {
        return CommandBuilder{"ZREVRANGEBYSCORE", key, max, min, "WITHSCORES"}.get();
    }

    RCommands operator()(const std::string& key, const std::string& max, const std::string& min, int offset, int count)
    {
        return CommandBuilder{"ZREVRANGEBYSCORE", key, max, min, "WITHSCORES", "LIMIT", offset, count}.get();
    }
};


template <bool WithScores>
struct zrangebyscore{};

template <>
struct zrangebyscore<false>
{
    RCommands operator()(const std::string& key, const std::string& min, const std::string& max)
    {
        return CommandBuilder{"ZRANGEBYSCORE", key, min, max}.get();
    }

    RCommands operator()(const std::string& key, const std::string& min, const std::string& max, int offset, int count)
    {
        return CommandBuilder{"ZRANGEBYSCORE", key, min, max, "LIMIT", offset, count}.get();
    }
};

template <>
struct zrangebyscore<true>
{
    RCommands operator()(const std::string& key, const std::string& min, const std::string& max)
    {
        return CommandBuilder{"ZRANGEBYSCORE", key, min, max, "WITHSCORES"}.get();
    }

    RCommands operator()(const std::string& key, const std::string& min, const std::string& max, int offset, int count)
    {
        return CommandBuilder{"ZRANGEBYSCORE", key, min, max, "WITHSCORES", "LIMIT", offset, count}.get();
    }
};

    

struct zincrby
{
    template<typename ElemType, typename ScoreType>
    RCommands operator()(const std::string& key, const ElemType& memeber, const ScoreType& increment)
    {
        return CommandBuilder{"ZINCRBY", key, increment, memeber}.get();
    }
};

struct hmset
{ 
    template<typename FieldType, typename ValueType>
    RCommands operator()(const std::string& key, const std::vector<std::pair<FieldType,ValueType>>& avps)
    {
        return CommandBuilder{"HMSET", key, avps}.get();
    }

    template<typename FieldType, typename ValueType>
    RCommands operator()(const std::string& key, const std::unordered_map<FieldType,ValueType>& avps)
    {
        return CommandBuilder{"HMSET", key, avps}.get();
    }
};


struct hmget
{ 
    template<typename FieldType>
    RCommands operator()(const std::string& key, const std::vector<FieldType>& fields)
    {
        return CommandBuilder{"HMSET", key, fields}.get();
    }
};


struct expire
{
    RCommands operator()(const std::string& key, long seconds)
    {
        return CommandBuilder{"EXPIRE", key, seconds}.get();
    }
};

struct expireat
{
    RCommands operator()(const std::string& key, long timestamp)
    {
        return CommandBuilder{"EXPIREAT", key, timestamp}.get();
    }
};


struct eval
{
    template <typename ... ARGS>
    RCommands operator()(const std::string& script, ARGS&& ... args)
    {   
        return CommandBuilder{"EVAL", script, std::forward<ARGS>(args)...}.get();
    }
};


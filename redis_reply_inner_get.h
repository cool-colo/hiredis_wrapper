#pragma once
#include <type_traits>
#include "common.h"
#include "lexical_cast_wrapper.h"
#include "redis_command.h"

template <typename Reply, typename DataType, bool IsPod> 
struct RedisReplyInnerGet{};

template <typename Reply, typename DataType> 
struct RedisReplyInnerGet<Reply, DataType, true>
{
    static inline int get(const Reply& reply, DataType& data)
    {
        if(reply->type == REDIS_REPLY_NIL)
        { 
            return 0;
        }   
        else if(reply->type == REDIS_REPLY_STRING)
        {   
            std::string str;
            str.append(reply->str,reply->len);
            data = LexicalCastWrapper<DataType>()(std::move(str)); 
            return 0;
        }   
        else if (reply->type == REDIS_REPLY_INTEGER)
        {
            data = LexicalCastWrapper<DataType>()(std::to_string(reply->integer));
            return 0;
        }
        else if(reply->type == REDIS_REPLY_STATUS) 
        { 
            if (strcasecmp(reply->str,"OK") == 0)
            {
                data = LexicalCastWrapper<DataType>()("1"); 
                return 0;
            
            }
            else
            {
                data = LexicalCastWrapper<DataType>()("0"); 
                return 0;
            }
        }   
        else
        {
            return -1;
        }
    }

};

template <typename Reply> 
struct RedisReplyInnerGet<Reply, std::string, false>
{
    static inline int get(const Reply& reply, std::string& data)
    {
        if(reply->type == REDIS_REPLY_NIL)
        { 
            return 0;
        }   
        else if(reply->type == REDIS_REPLY_STRING)
        {   
            data.append(reply->str,reply->len);
            return 0;
        }   
        else if (reply->type == REDIS_REPLY_INTEGER)
        {
            data = std::to_string(reply->integer);
            return 0;
        }
        else if(reply->type == REDIS_REPLY_STATUS) 
        { 
            if (strcasecmp(reply->str,"OK") == 0)
            {
                data = "1"; 
                return 0;
            
            }
            else
            {
                data = "0"; 
                return 0;
            }
        }   
        else
        {
            return -1;
        }
    }

};

template <typename Reply, typename MemberType, typename ScoreType>
struct RedisReplyInnerGet<Reply, std::vector<std::pair<MemberType,ScoreType>>, false> 
{
    static inline int get(const Reply& reply, std::vector<std::pair<MemberType,ScoreType>>& data)
    {

        if(reply->type == REDIS_REPLY_NIL)
        { 
            return 0;
        }   
        else if (reply->type == REDIS_REPLY_ARRAY)
        {
            std::string val;
            std::pair<MemberType,ScoreType> kv; 
            for (int j = 0; j < reply->elements; j++)
            {
                val.clear();
                val.append(reply->element[j]->str,reply->element[j]->len);
                if (j%2 == 0) //memeber
                {
                    kv.first = LexicalCastWrapper<MemberType>()(std::move(val)); 
                }
                if (j%2 == 1) //score
                {
                    kv.second = LexicalCastWrapper<ScoreType>()(std::move(val)); 
                    data.push_back(std::move(kv));
                }
            }
            return 0;
        }
        else
        {
            return -1;
        }
    }
};

template <typename Reply, typename DataType>
struct RedisReplyInnerGet<Reply, std::vector<DataType>, false> 
{
    static inline int get(const Reply& reply, std::vector<DataType>& data)
    {
        if(reply->type == REDIS_REPLY_NIL)
        { 
            return 0;
        }   
        else if (reply->type == REDIS_REPLY_ARRAY)
        {
            std::string val;
            for (int j = 0; j < reply->elements; j++)
            {
                val.clear();
                val.append(reply->element[j]->str,reply->element[j]->len);
                auto&& element = LexicalCastWrapper<DataType>()(std::move(val));  
                data.push_back(std::move(element));
            }
            return 0;
        }
        else
        {
            return -1;
        }
    }

};


template <typename DataType>
struct RedisReplyGet
{
    template<typename Reply>
    static inline int get(const Reply& reply, DataType& data)
    {
        if (reply.get() == nullptr)
        {
            AC_ERROR("null reply from redis server");
            return -1;
        } 
        switch(reply->type)
        {
            case REDIS_REPLY_STRING:
            case REDIS_REPLY_INTEGER:    
            case REDIS_REPLY_ARRAY:    
            case REDIS_REPLY_NIL:    
            case REDIS_REPLY_STATUS:    
                return RedisReplyInnerGet<Reply, typename std::remove_reference<DataType>::type, std::is_pod<DataType>::value>::get(reply, data);
                break;

            case REDIS_REPLY_ERROR:
                AC_ERROR("error reply from redis server, msg: %s", reply->str);
                return -1;
                break;
        }
        return -1;
    }
};

template <>
struct RedisReplyGet<NoReply>
{
    template<typename Reply>
    static inline int get(const Reply& reply, const NoReply& data)
    {
        if (reply.get() == nullptr)
        {
            AC_ERROR("null reply from redis server");
            return -1;
        } 
        switch(reply->type)
        {
            case REDIS_REPLY_STRING:
            case REDIS_REPLY_INTEGER:    
            case REDIS_REPLY_ARRAY:    
            case REDIS_REPLY_NIL:    
            case REDIS_REPLY_STATUS:    
                return 0;
                break;

            case REDIS_REPLY_ERROR:
                AC_ERROR("error reply from redis server, msg: %s", reply->str);
                return -1;
                break;
        }
        return -1;
    }    
};


#include <iostream>
#include <string>
#include <vector>
#include "redis_wrapper.h"

class RedisWrapper;

class RedLock {
public:
                                RedLock(const std::string& resource, const int ttl);
    virtual                     ~RedLock();
public:
    bool                        Initialize();
    bool                        AddServer(std::shared_ptr<RedisWrapper> driver);
    void                        SetRetry(const int count, const int delay);
    bool                        Lock();

    bool                        Unlock();
private:
    bool                        LockInstance(std::shared_ptr<RedisWrapper> driver, const std::string& resource, const std::string& val, const int ttl);
    void                        UnlockInstance(std::shared_ptr<RedisWrapper> driver, const std::string& resource, const std::string& val);

    std::string                 GetUniqueLockId();
private:
    static int                  m_defaultRetryCount;    
    static int                  m_defaultRetryDelay;    
    static float                m_clockDriftFactor;    
private:
    std::string                 m_unlockScript;
    std::string                 resource;
    std::string                 val;
    int                         ttl;
    int                         m_retryCount;           // try count
    int                         m_retryDelay;           // try delay
    int                         m_quoRum;               // majority nums
    std::vector<std::shared_ptr<RedisWrapper>>  m_redisDriver;      // redis master servers
};


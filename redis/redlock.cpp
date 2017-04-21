#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include "redlock.h"
#include "redis_client.h"



int RedLock::m_defaultRetryCount = 3;
int RedLock::m_defaultRetryDelay = 200;
float RedLock::m_clockDriftFactor = 0.01;

RedLock::RedLock(const std::string& res, const int time) 
   :resource(res), ttl(time)
{
    Initialize();
}


RedLock::~RedLock() {
}


bool RedLock::Initialize() {
    m_unlockScript       = "if redis.call('get', KEYS[1]) == ARGV[1] then return redis.call('del', KEYS[1]) else return 0 end";
    m_retryCount = m_defaultRetryCount;
    m_retryDelay = m_defaultRetryDelay;
    m_quoRum     = 0;

    return true;
}

bool RedLock::AddServer(std::shared_ptr<RedisWrapper> driver) 
{
    m_redisDriver.push_back(driver);
    m_quoRum = (int)m_redisDriver.size() / 2 + 1;
    return true;
}

void RedLock::SetRetry(const int count, const int delay) {
    m_retryCount = count;
    m_retryDelay = delay;
}


bool RedLock::Lock() {
    val = GetUniqueLockId();
    if (val.empty()) {
        return false;
    }

    printf("Get the unique id is %s\n", val.c_str());
    int retryCount = m_retryCount;
    do {
        int n = 0;
        int startTime = (int)time(NULL) * 1000;
        int slen = (int)m_redisDriver.size();
        for (int i = 0; i < slen; i++) {
            if (LockInstance(m_redisDriver[i], resource, val, ttl)) {
                n++;
            }
        }
        //Add 2 milliseconds to the drift to account for Redis expires
        //precision, which is 1 millisecond, plus 1 millisecond min drift
        //for small TTLs.
        int drift = (ttl * m_clockDriftFactor) + 2;
        int validityTime = ttl - ((int)time(NULL) * 1000 - startTime) - drift;
        printf("The resource validty time is %d, n is %d, quo is %d\n",
               validityTime, n, m_quoRum);
        if (n >= m_quoRum && validityTime > 0) {
            return true;
        } else {
            Unlock();
        }
        // Wait a random delay before to retry
        int delay = rand() % m_retryDelay + floor(m_retryDelay / 2);
        usleep(delay * 1000);
        retryCount--;
    } while (retryCount > 0);
    return false;
}


bool RedLock::Unlock() {
    int slen = (int)m_redisDriver.size();
    for (int i = 0; i < slen; i++) {
        UnlockInstance(m_redisDriver[i], resource, val);
    }
    return true;
}

bool RedLock::LockInstance(std::shared_ptr<RedisWrapper> driver, const std::string& resource, const std::string& val, const int ttl) 
{
	bool lock = false;
	RedisCommandExecutor<set<bool>> cmd(driver);
	cmd.execute(lock, resource, val, PX(), ttl, NX());
	return lock;
}

void RedLock::UnlockInstance(std::shared_ptr<RedisWrapper> driver, const std::string& resource, const std::string& val)
{
	RedisCommandExecutor<eval<NoReply>> cmd(driver);
	cmd.execute_noreply(m_unlockScript, "1", resource, val);

}
std::string RedLock::GetUniqueLockId() 
{
    return std::to_string(rand());
}

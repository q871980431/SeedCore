#include "RedisHelper.h"
#include "hiredis.h"

RedisResult::~RedisResult()
{
	if (_free)
		freeReplyObject(_reply);
}

bool RedisResult::GetInteger(s64 &val)
{
	if (_reply->type == REDIS_REPLY_INTEGER)
	{
		val = _reply->integer;
		return true;
	}
	return false;
}

bool RedisResult::GetFloat(double &val)
{
	if (_reply->type == REDIS_REPLY_DOUBLE)
	{
		val = _reply->dval;
		return true;
	}
	return false;
}
bool RedisResult::GetStr(const char *&val, s32 &len)
{
	if (_reply->type == REDIS_REPLY_STRING)
	{
		val = _reply->str;
		len = (s32)_reply->len;
		return true;
	}

	return false;
}

bool RedisResult::ForeachArray(const std::function<void(RedisResult&)> &eachFun)
{
	if (_reply->type == REDIS_REPLY_ARRAY)
	{
		for (size_t i = 0; i < _reply->elements; i++)
		{
			RedisResult tmp(_reply->element[i], false);
			eachFun(tmp);
		}
		return true;
	}

	return false;
}

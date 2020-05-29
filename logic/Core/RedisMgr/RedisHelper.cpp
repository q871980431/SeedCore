#include "RedisHelper.h"
#include "hiredis.h"
#include "win32.h"

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

bool RedisResult::GetString(std::string &strVal)
{
	if (_reply->type == REDIS_REPLY_STRING)
	{
		if (_reply->len > 0)
			strVal.assign(_reply->str, _reply->len);
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

bool RedisResult::IsNil()
{
	return _reply->type == REDIS_REPLY_INTEGER;
}

bool RedisResult::IsOK()
{
	const char *retStr = nullptr;
	s32 len = 0;
	if (GetStr(retStr, len))
	{
		if (strcmp(retStr, "OK") == 0)
			return true;
	}

	return false;
}

bool RedisResult::ParseStringArray(std::vector<std::string> &vals)
{
	if (_reply->type == REDIS_REPLY_ARRAY)
	{
		for (size_t i = 0; i < _reply->elements; i++)
		{
			RedisResult tmp(_reply->element[i], false);
			std::string tmpVal;
			tmp.GetString(tmpVal);
			vals.push_back(std::move(tmpVal));
		}
		return true;
	}
	return false;
}

bool RedisResult::ParseStringMap(std::map<std::string, std::string> &vals)
{
	if (_reply->type == REDIS_REPLY_ARRAY)
	{
		for (size_t i = 0; i < (_reply->elements - 1); )
		{
			RedisResult keyResult(_reply->element[i], false);
			RedisResult valResult(_reply->element[i + 1], false);
			std::string key;
			std::string val;
			keyResult.GetString(key);
			valResult.GetString(val);

			vals.emplace(std::move(key), std::move(val));
			i += 2;
		}
		return true;
	}
	return false;
}

int RedisWraaper::redisBufferWriteInner(redisContext *c)
{

	/* Return early when the context has seen an error. */
	if (c->err)
		return REDIS_ERR;

	int32_t *len = ((int32_t *)c->obuf - 1);
	while (*len > 0)
	{
		int32_t nwritten = send(c->fd, c->obuf, *len, 0);
		if (nwritten < 0) {
			if ((errno == EWOULDBLOCK && !(c->flags & REDIS_BLOCK)) || (errno == EINTR)) {
				/* Try again later */
			}
			else {
				__redisSetErrorInner(c, REDIS_ERR_IO, NULL);
				return REDIS_ERR;
			}
		}
		*len -= nwritten;
		c->obuf = c->obuf + nwritten;
	}
	return REDIS_OK;
}


int RedisWraaper::redisGetReplyInner(redisContext *c, void **reply)
{
	void *aux = NULL;
	if (redisBufferWriteInner(c) == REDIS_ERR)
		return REDIS_ERR;

	/* Read until there is a reply */
	do {
		if (redisBufferRead(c) == REDIS_ERR)
			return REDIS_ERR;
		if (redisGetReplyFromReader(c, &aux) == REDIS_ERR)
			return REDIS_ERR;
	} while (aux == NULL);

	/* Set reply or free it if we were passed NULL */
	if (reply != NULL) {
		*reply = aux;
	}
	else {
		freeReplyObject(aux);
	}

	return REDIS_OK;
}

void RedisWraaper::__redisSetErrorInner(redisContext *c, int type, const char *str)
{
	size_t len;

	c->err = type;
	if (str != NULL) {
		len = strlen(str);
		len = len < (sizeof(c->errstr) - 1) ? len : (sizeof(c->errstr) - 1);
		memcpy(c->errstr, str, len);
		c->errstr[len] = '\0';
	}
	else {
		/* Only REDIS_ERR_IO may lack a description! */
		assert(type == REDIS_ERR_IO);
		strerror_r(errno, c->errstr, sizeof(c->errstr));
	}
}

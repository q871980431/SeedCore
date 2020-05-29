#ifndef _RedisHelper_h_ 
#define _RedisHelper_h_

#include "MultiSys.h"
#include <string>
#include <functional>
#include <vector>
#include <map>

const s32   HEAD_CONTENT_BUFF = 20;	//len buff
class RedisCmdBuilder
{
public:
	class StrView
	{
	public:
		StrView(const std::string &val) { _buff = val.c_str(); _len = val.length(); };
		StrView(const char *buff) :_buff(buff) { _len = strlen(buff); };
		StrView(const char *buff, size_t len) :_buff(buff), _len(len) {};

		inline const char *data() const { return _buff; };
		inline size_t len() const { return _len; };
	private:
		const char *_buff;
		size_t _len;
	};

	RedisCmdBuilder(s32 buffSize) :_cmd(HEAD_CONTENT_BUFF, (char)0){
		_cmd.resize(buffSize); 
	};
	RedisCmdBuilder() :_cmd(HEAD_CONTENT_BUFF, (char)0) {};
	~RedisCmdBuilder() {};

	RedisCmdBuilder & operator << (const std::string &val)
	{
		char buff[32];
		SafeSprintf(buff, sizeof(buff), "$%d\r\n", val.length());
		_cmd.append(buff);
		_cmd.append(val);
		_cmd.push_back('\r');
		_cmd.push_back('\n');
		_count++;
		return *this;
	}

	RedisCmdBuilder & operator << (const char *val)
	{
		char buff[32];
		SafeSprintf(buff, sizeof(buff), "$%d\r\n", strlen(val));
		_cmd.append(buff);
		_cmd.append(val);
		_cmd.push_back('\r');
		_cmd.push_back('\n');
		_count++;
		return *this;
	}

	RedisCmdBuilder & operator << (const StrView &val)
	{
		char buff[32];
		SafeSprintf(buff, sizeof(buff), "$%d\r\n", (s32)val.len());
		_cmd.append(buff);
		_cmd.append(val.data(), (s32)val.len());
		_cmd.push_back('\r');
		_cmd.push_back('\n');
		_count++;
		return *this;
	}

	template<typename T>
	RedisCmdBuilder & operator <<(T &val)
	{
		*this << (std::to_string(val));
		return *this;
	}


	void Commit()
	{
		if (!_commit)
		{
			char buff[HEAD_CONTENT_BUFF];
			char *cmdHead = buff + sizeof(int32_t);
			SafeSprintf( cmdHead, sizeof(buff) - sizeof(int32_t), "*%d\r\n", _count);

			s32 headLen = strlen(cmdHead);
			headLen += sizeof(int32_t);
			_seek = HEAD_CONTENT_BUFF - headLen;
			int32_t * len = (int32_t *)buff;
			*len = _cmd.size() - _seek - sizeof(int32_t);
			_cmd.replace(_seek, headLen, buff, headLen);
			_seek += sizeof(int32_t);

			_commit = true;
		}
	}

	inline const char * GetSendBuff() const { return _cmd.c_str() + _seek; };
	inline bool IsCommit() const { return _commit; };
protected:
private:
	s32 _count{ 0 };
	bool _commit{ false };
	std::string _cmd;
	s32 _seek{0};
};

class redisReply;

class RedisResult
{
	RedisResult(redisReply *reply, bool free) :_reply(reply), _free(free) {};
public:
	RedisResult(redisReply *reply) :_reply(reply),_free(true) {};
	~RedisResult();

	bool GetInteger(s64 &val);
	bool GetFloat(double &val);
	bool GetStr(const char *&val, s32 &len);
	bool GetString(std::string &strVal);
	bool ForeachArray(const std::function<void(RedisResult&)> &eachFun);
	bool IsNil();

public:
	bool IsOK();
	bool ParseStringArray(std::vector<std::string> &vals);
	bool ParseStringMap(std::map<std::string, std::string> &vals);

protected:

private:
	redisReply *_reply;
	bool _free;
};

struct redisContext;
class RedisWraaper
{
public:
	static int redisBufferWriteInner(redisContext *c);
	static int redisGetReplyInner(redisContext *c, void **reply);

private:
	static void __redisSetErrorInner(redisContext *c, int type, const char *str);
};


#endif

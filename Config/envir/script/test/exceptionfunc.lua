local scripttest = require "serverd.scripttest"
local print=print
local type=type
local a = 0;
_ENV = {}

function testEcho(x)
	print("test lua call x: "..x)
	local y = nil
	print("x/0 == "..y)
	return x/0
end

function testCallExcptionLuaFunc(x)
	print("testCallExcptionLuaFunc x: "..x)
	local ret = scripttest.callExceptionLuaFunc(x);
	return ret;
end

return _ENV
local scripttest = require "serverd.scripttest"
local print=print
local type=type
local a = 0;
_ENV = {}

function testEcho(x, y)
	print("test lua call x: "..x)
	return x
end

function testFibNum(x)
	return scripttest.callFibNumer(x - 1, x) +  scripttest.callFibNumer(x - 2, x)
end

function testPoint(pPlayer)
	print("pPlayer type:"..type(pPlayer))
	local playerId = scripttest.getPlayerId(pPlayer)
	print("Player id:"..playerId)
	return playerId
end

function testReload(x)
	print("testReload"..x)
	return x
	--return isReload(x);

return _ENV
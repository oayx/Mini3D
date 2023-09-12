-- @author hannibal
-- @date   2015/11/20
-- @brief  游戏对象基类(NOTE:不用使用3层或以上继承，否则可能会导致super调用异常)

local GameObject = Class.instance("GameObject")

function GameObject:init()
    --初始化成员变量
    self.object_id = 1			
	self.active = true
    Log.debug("GameObject:init")
end

function GameObject:destroy()
    Log.debug("GameObject:destroy")
    self.active = false
    self.object_id = -1
end

function GameObject:setup(info)
    Log.debug("GameObject:setup")
end

return GameObject
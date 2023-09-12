-- @author hannibal
-- @date   2015/11/23
-- @brief  玩家

local Player = Class.instance("Player", CommonUtil.require_reload("object.Role"))

function Player:init()
    Log.debug("Player:init")
    self.super:init()
end

function Player:destroy()
    Log.debug("Player:destroy")
    self.super:destroy()
end

function Player:setup(info)
    Log.debug("Player:setup")
    self.super:setup(info)
end

return Player

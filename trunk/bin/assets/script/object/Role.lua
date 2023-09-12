-- @author hannibal
-- @date   2015/11/23
-- @brief  角色基类

local Role = Class.instance("Role", CommonUtil.require_reload("object.GameObject"))

function Role:init()
    Log.debug("Role:init")
    self.super:init()
end

function Role:destroy()
    Log.debug("Role:destroy")
    self.super:destroy()
end

function Role:setup(info)
    Log.debug("Role:setup")
    self.super:setup(info)
end

return Role
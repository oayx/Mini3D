-- @author hannibal
-- @date   2015/11/18
-- @brief  公共

CommonUtil = CommonUtil or {}

-- 需要多次加载
function CommonUtil.require_reload( filename) 
    local full_name = filename
    local obj = require( full_name )
    package.loaded[ full_name] = nil 
    return obj
end

function CommonUtil.require( filename) 
    local obj = require( filename )
    return obj
end

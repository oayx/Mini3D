-- @author hannibal
-- @date   2015/11/18
-- @brief  日志输出

Log = Log or {}

local function print_log(tag, fmt, ...)
    local t = 
    {
        "[",
        string.upper(tostring(tag)),
        "] ",
        string.format(tostring(fmt), ...)
    }
    print(table.concat(t))
end
--[[调试]]
function Log.debug(fmt, ...)
    print_log("debug", fmt, ...)
end
--[[信息]]
function Log.info(fmt, ...)
    print_log("info", fmt, ...)
end
--[[警告]]
function Log.warning(fmt, ...)
    print_log("warning", fmt, ...)
end
--[[错误]]
function Log.error(fmt, ...)
    print_log("error", fmt, ...)
    print(debug.traceback("", 2))
end
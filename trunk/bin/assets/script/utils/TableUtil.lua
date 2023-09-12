-- @author hannibal
-- @date   2015/11/18
-- @brief  table

TableUtil = TableUtil or {}

function TableUtil.nums(t)
    if type(t) ~= "table" then return end

    local count = 0
    for k, v in pairs(t) do
        count = count + 1
    end
    return count
end

function TableUtil.clear(t)
    if type(t) ~= "table" then return end

    for key, value in pairs(t) do 
        if type(key) == "table" then
            TableUtil.clear(value)
        end 
        if type(value) == "table" then
            TableUtil.clear(value)
        end
        t[key] = nil
        key = nil
    end
end

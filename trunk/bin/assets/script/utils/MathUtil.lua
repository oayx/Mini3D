-- @author hannibal
-- @date   2015/11/18
-- @brief  数学函数

MathUtil = MathUtil or {}

--字节转换M
MathUtil.BYTE_TO_M = 1/(1024*1024)
--字节转换K
MathUtil.BYTE_TO_K = 1/(1024)

function MathUtil.newrandomseed()
    local ok, socket = pcall(function()
        return require("socket")
    end)

    if ok then
        math.randomseed(socket.gettime() * 1000)
    else
        math.randomseed(os.time())
    end
    math.random()
    math.random()
    math.random()
    math.random()
end

-- 设置随机种子，可以用os.time()设置
function MathUtil.randomSeek(n)
    return math.random(n)
end


-- 返回[0,1)
function MathUtil.random(n)
    return math.random(n)
end

-- 返回整数[1,n]
function MathUtil.randomInt(n)
    return math.random(n)
end

-- 返回整数[min, max]
function MathUtil.randomRange(min, max)
    return math.random(min, max)
end

-- 限制范围
function MathUtil.clamp(n, min, max)
	if min > max then
		min, max = max, min
	end	
    if n < min then 
        return min 
    elseif n > max then
        return max
    else
        return n
    end
end

-- 角度转换0-360度
function MathUtil.clamp0_360(num)
	num = math.fmod(num, 360);
    if num < 0 then num = num + 360 end
	
	return num;
end

local pi_div_180 = math.pi / 180
-- 角度转弧度
function MathUtil.toRadian(angle)
    return angle * pi_div_180
end

local pi_mul_180 = math.pi * 180
-- 弧度转角度
function MathUtil.toDegree(radian)
    return radian / pi_mul_180
end

-- 求两点距离
function MathUtil.distance(x1, y1, x2, y2)
    return math.sqrt(math.pow(y2-y1,2), math.pow(x2-x1,2))
end

-- 两点距离平方
function MathUtil.squareDistance(x1, y1, x2, y2)
    return math.pow(y2-y1,2), math.pow(x2-x1,2)
end

-- 两点间的角度 
function MathUtil.pointAngle(x1, y1, x2, y2)
    return MathUtil.toDegree(math.pow(y2-y1,2), math.pow(x2-x1,2))
end

-- 两点间的弧度
function MathUtil.pointRadians(x1, y1, x2, y2)
    return math.atan2(y2 - y1, x2 - x1);
end
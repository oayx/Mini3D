-- @author hannibal
-- @date   2015/11/23
-- @brief  类

Class = Class or {}

-- lua类，支持单继承
function Class.instance(classname, super)
    local cls = {__cname = classname}

    if super then
        local superType = type(super)
        assert(superType == "nil" or superType == "table" or superType == "function", string.format("class() - create class \"%s\" with invalid super class type \"%s\"", classname, superType))

        if superType == "function" then
            assert(cls.__create == nil, string.format("class() - create class \"%s\" with more than one creating function", classname));
            -- if super is function, set it to __create
            cls.__create = super
        elseif superType == "table" then
            if super[".isclass"] then
                -- super is native class
                assert(cls.__create == nil,string.format("class() - create class \"%s\" with more than one creating function or native class", classname));
                cls.__create = function() return super:create() end
            else
                -- super is pure lua class
                if not cls.super then
                    cls.super = super
                end
            end
        else
            error(string.format("class() - create class \"%s\" with invalid super type", classname), 0)
        end
    end

    cls.__index = cls
    if cls.super then
        setmetatable(cls, {__index = cls.super})
    end

    -- leaf 叶子节点
    if super then
        local _super = super
        repeat
            _super.leaf = cls
            _super = _super.super
        until not _super;
    end
    cls.leaf = cls

    if not cls.ctor then
        -- add default constructor
        cls.ctor = function() end
    end
    cls.new = function(...)
        local instance
        if cls.__create then
            instance = cls.__create(...)
        else
            instance = {}
        end
        setmetatableindex(instance, cls)
        instance.class = cls
        instance:ctor(...)
        return instance
    end
    cls.create = function(_, ...)
        return cls.new(...)
    end

    return cls
end

-- lua简单类，不含继承关系
-- instance 类的一个实例
function Class.new(instance)
    cls = {}  
    setmetatable(cls,instance)  
    instance.__index = instance  
    return cls 
end
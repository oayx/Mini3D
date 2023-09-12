-- @author hannibal
-- @date   2015/11/17
-- @brief  数据结构

-------------------------------------------StringBuilder--------------------------------------------

-------------------------------------------数组---------------------------------------------
--索引连续的数组
Array = Array or {}

function Array:new()
    local cls = Class.new(Array) 
    cls.length = 0
    cls.data = {}
    return cls
end

--插入元素
--param index 插入位置,1开头
function Array:insert(index, value)
    local len = self.length
    if index < 1 or index > len +1 then --加1是为了支持在数组末尾插入元素
        Log.error("索引错误%d", index)
        return 
    end
    --前面的依次往后面移动
    table.insert(self.data, index, value)
    
    self.length = self.length + 1
end

--添加元素
function Array:add(value)
    self.length = self.length + 1
    self.data[self.length] = value
end

--移除元素
--param index 位置,1开头
function Array:remove(index)
    local len = self.length
    if index < 1 or index > len then return end

    --后面的依次往前面移动
    table.remove(self.data, index)
    self.length = self.length - 1
end

function Array:find(value)
    local len = self.length
    local index = -1;
    for i = 1, len, 1 do
        if self.data[i] == value then
            index = i
            break
        end
    end
    return index
end

function Array:size()
    return self.length
end

function Array:empty()
    if self.length == 0 then
        return true
    else
        return false
    end
end

function Array:clear()
    local len = self.length
    for i = 1, len, 1 do
        self.data[i] = nil
    end
end

-- 输出  
function Array:dump()
    local len = self.length
    Log.debug("Array size:%d", len)
    for i = 1, len, 1 do
        Log.debug("value:%s", self.data[i])
    end
end

-------------------------------------------双向循环链表---------------------------------------------
List = List or {}
-- 创建链表
function List.new()
	local t =  {next = nil, prev = nil, value = nil, length = 0}
	t.next = t
	t.prev = t
	return t;
end

-- 插入头部
function List.push_front(list, val)
	local t = {next = list.next, prev = list, value = val}
	list.next.prev = t
	list.next = t
    list.length = list.length + 1;
end

-- 插入尾部
function List.push_back(list,val)
	local t = {next = list, prev = list.prev, value = val}
	list.prev.next = t
	list.prev = t
    list.length = list.length + 1;
end

-- 删除头部		
function List.pop_front(list)
    if List.empty(list) then
        Log.error("List is empty")
        return nil
    end
	local t = list.next
	list.next = t.next
	t.next.prev = list
    list.length = list.length - 1;
	t = nil
end

-- 删除尾部
function List.pop_back(list)
    if List.empty(list) then
        Log.error("List is empty")
        return nil
    end
	local t = list.prev
	list.prev = t.prev
	t.prev.next = list
    list.length = list.length - 1;
	t = nil
end

function List.find(list , val)
	local t = list.next
	while not (t == list) do
		if t.value == val then
			return t
		end
		t = t.next
	end
	return nil
end

function List.insert(list , val)
	List.push_front(list, val)
end

--  删除元素  
function List.remove(list, val)
	local t = List.find(list, val)
	if t then
		t.next.prev = t.prev
		t.prev.next = t.next
		t = nil
	else
		print("元素不存在")
	end
end

--  判空  
function List.empty(list)
    if list.length == 0 then
        return true;
    else
        return false;
    end
end

--  判空  
function List.size(list)
    return list.length;
end

--  输出链表  
function List.dump(list)
	local t = list.next
	while not (t == list) do
		Log.debug(t.value)
		t = t.next
	end
end

--  链表反转  
function List.reverse(list)
	local t = list.next
	local curnode = list
	repeat 
		curnode.next = curnode.prev
		curnode.prev = t
		curnode = t
		t = t.next
	until curnode == list 
end

--  查找最小值  
function List.find_min(beglist, endlist )
	local t = beglist
	local m = beglist
	while not (t == endlist) do
		if t.value < m.value then
			m = t
		end
		t = t.next
	end
	return m
end

--  排序  
function  List.sort(list)
	local t = list.next
	while not (t == list) do	
		--  从剩余节点中查找最小值  
		local m = List.find_min(t, list)
		--  如果找到的是剩余节点的开始节点，则将开始节点后移  
		if m == t then
			t = t.next
		end
		List.pop_front(m.prev)
		List.push_front(list, m.value)
	end
	List.reverse(list)
end

-- 定义链表迭代器 
function ListIter( list )
	local t = list.next
    return function ()  
	    if not (t == list) then
		    local ret = t.value
		    t = t.next
            return ret
        else
            return nil
        end
	end
end
--local iter = ListIter(list)
--while true do
--	local value = iter()
--	if not value then break end
--	print(value)
--end

-------------------------------------------栈---------------------------------------------
Stack = {}

function Stack:new()
    local cls = Class.new(Stack) 
    cls.last = 0
    cls.data = {}
    return cls
end

function Stack:push(value)
    local last = self.last + 1
    self.last = last
    self.data[last] = value
end

--弹出栈顶，同时返回栈顶元素
function Stack:pop()
    if self:empty() then
        Log.error("Stack is empty")
        return nil
    end
    local last = self.last
    local value = self.data[last]
    self.data[last] = nil
    self.last = last - 1
    return value
end

function Stack:empty()
    if self.last == 0 then
        return true
    end
    return false
end

function Stack:size()
    return self.last
end

--只是清空元素，本身存在
function Stack:clear()
    local t = self.last
	while t ~= 0 do
        local obj = self.data[t]
        if type(obj) == "table" then
            TableUtil.clear(obj)
        end
        self.data[t] = nil
        t = t - 1
	end
    self.last = 0
end

--  输出  
function Stack:dump()
    Log.debug("Stack size:%d", self:size())
	local t = self.last
	while t ~= 0 do
        local value = self.data[t]
		Log.debug(value)
		t = t - 1
	end
end

-------------------------------------------队列---------------------------------------------
Queue = {}

function Queue:new()
    local cls = Class.new(Queue) 
    cls.first = 1
    cls.last = 0
    cls.data = {}
    return cls 
end

function Queue:push(value)
    local last = self.last + 1
    self.last = last
    self.data[last] = value
end

--弹出队列头，同时返回队列头元素
function Queue:pop()
    if self:empty() then
        Log.error("Queue is empty")
        return nil
    end
    local first = self.first
    local value = self.data[first]
    self.data[first] = nil
    self.first = first + 1
    return value
end

function Queue:empty()
    if self.first > self.last then
        return true
    end
    return false
end

function Queue:size()
    return self.last - self.first + 1
end

--只是清空元素，本身存在
function Queue:clear()
    local first = self.first
    local last = self.last
	while first <= last do
        local obj = self.data[first]
        if type(obj) == "table" then
            TableUtil.clear(obj)
        end
        self.data[first] = nil
        first = first + 1
	end
    self.first = 1
    self.last = 0
end

--  输出  
function Queue:dump()
    Log.debug("Queue size:%d", self:size())
    local first = self.first
    local last = self.last
	while first <= last do
        local value = self.data[first]
		Log.debug(value)
		first = first + 1
	end
end

-------------------------------------------Map---------------------------------------------
--[键-值]对
Map = {}

function Map:new()
    local cls = Class.new(Map)
    cls.length = 0
    cls.data={}
    return cls 
end

function Map:insert(key, value)
    if self.data[key] ~= nil then return false end

    self.data[key] = value
    self.length = self.length + 1
    return true
end

function Map:remove(key)
    if self.data[key] == nil then return end

    self.data[key] = nil
    self.length = self.length - 1
end

function Map:find(key)
    return self.data[key]
end

function Map.size()
    return self.length
end

function Map:empty()
    if self.length == 0 then
        return true
    else
        return false
    end
end

-- 返回开始位置的迭代器
-- 方法是把你所需要遍历的table里的key按照遍历顺序放到另一个临时的table中去，这样只需要遍历这个临时的table按顺序取出原table中的key就可以了
function Map:begin(sort)
    local a = {}  
    for key in pairs(self.data) do  
        a[#a+1] = key  
    end  

    if sort then
        table.sort(a)  
    end

    local i = 0  
    return function()  
        i = i + 1 
        if i > #a then return nil end 
        return {first = a[i], second = t[a[i]]} 
    end 
end

-- 第一个元素，不一定是插入的第一个值
function Map:front()
    for _, value in pairs(self.data) do
        return value
    end
    return nil
end

function Map:clear()
    for key in pairs(self.data) do
        self.data[key] = nil
    end
end

--  输出  
function Map:dump()
    Log.debug("Map size:%d", self.length)
    for key, value in pairs(self.data) do
        Log.debug("key:%s, value:%s", key, value)
    end
end

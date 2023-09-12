g_var = "123"

--设置搜索路径
package.path = package.path .. ";../../assets/script/?.lua"
package.path = package.path .. ";../../assets/internal/script/?.lua"
print(package.path)

--框架
require("Framework")

function TestBase()
	print("----------MathUtil----------")    
	print(MathUtil.clamp0_360(100))
	print(MathUtil.clamp0_360(360))
	print(MathUtil.clamp0_360(0))
	print(MathUtil.clamp0_360(-100))
	
	print("----------Array----------")
	local arr = Array:new()
	arr:add(1)
	arr:insert(2, 2)
	arr:insert(2, 3)
	arr:remove(2)
	arr:dump()

	print("----------List----------")
	local list = List.new();
	List.push_front(list,1);
	List.push_front(list,2);
	List.push_front(list,3);
	List.push_back(list,4);
	List.push_back(list,5);
	List.pop_back(list);
	List.dump(list);

	print("----------Stack----------")
	local stack = Stack:new()
	stack:push(1)
   	stack:push(2)
   	stack:push(3)
   	stack:pop()
   	stack:dump()
   	stack:clear()
   	stack:dump()

	print("----------Queue----------")
   	local q = Queue:new()
   	q:push(1)
   	q:push(2)
   	q:push(3)
   	q:pop()
   	q:dump()
   	q:clear()
	q:dump()

	local t = {1,2,3,4}
	local num = TableUtil.nums(t)
	print("num:" .. num)
end

--类及继承
function TestClass()
	local player = CommonUtil.require_reload("object.Player")
	if player == nil then
		print("player is nil")
	end
	player:init()
	player:setup()
end

Framework.init()
TestBase()
TestClass()

function InitUI (param)
	print("InitUI" .. param)
	local add = lua_Add(5,5)
	return add, param
end

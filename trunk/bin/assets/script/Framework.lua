-- @author hannibal
-- @date   2015/11/18
-- @brief  逻辑框架

Framework = Framework or {}

function Framework.init()
	require("utils.CommonUtil")
    Framework.loadFile()
end

function Framework.release()
	
end

function Framework.loadFile()
	CommonUtil.require("base.Bits")
	CommonUtil.require("base.Class")
	CommonUtil.require("base.Log")
	CommonUtil.require("base.Struct")
	
	CommonUtil.require("utils.DateUtil")
	CommonUtil.require("utils.MathUtil")
	CommonUtil.require("utils.NumberUtil")
	CommonUtil.require("utils.StringUtil")
	CommonUtil.require("utils.TableUtil")
end
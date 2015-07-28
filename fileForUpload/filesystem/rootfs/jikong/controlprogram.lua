require("editevent")

--[[
	该文件会被动态库中的代码直接调用，其中的四个函数名称是固定、不可变的
	__jikong_initialize()
	__jikong_uninitialize()
	__jikong_ui_event(ui_id, event, data)
	__jikong_response_data(port_type, port_no, data)
--]]


bit={data32={}}
for i=1,32 do
    bit.data32[i]=2^(32-i)
end

function bit:d2b(arg)
    local   tr={}
    for i=1,32 do
        if arg >= self.data32[i] then
          tr[i]=1
          arg=arg-self.data32[i]
        else
          tr[i]=0
        end
    end
    return   tr
end   --bit:d2b

function    bit:b2d(arg)
    local   nr=0
    for i=1,32 do
        if arg[i] == 1 then
          nr=nr + 2^(32-i)
        end
    end
    return  nr
end   --bit:b2d

function    bit:_and(a,b)
    local   op1=self:d2b(a)
    local   op2=self:d2b(b)
    local   r={}
    
    for i=1,32 do
        if op1[i]==1 and op2[i]==1  then
            r[i]=1
        else
            r[i]=0
        end
    end
    return  self:b2d(r)
end --bit:_and

function    bit:_or(a,b)
    local   op1=self:d2b(a)
    local   op2=self:d2b(b)
    local   r={}
    
    for i=1,32 do
        if op1[i]== 1 or op2[i]== 1 then
            r[i]=1
        else
            r[i]=0
        end
    end
    return  self:b2d(r)
end --bit:_or

function    bit:_xor(a,b)
    local   op1=self:d2b(a)
    local   op2=self:d2b(b)
    local   r={}

    for i=1,32 do
        if op1[i]==op2[i] then
            r[i]=0
        else
            r[i]=1
        end
    end
    return  self:b2d(r)
end --bit:xor

function    bit:_not(a)
    local   op1=self:d2b(a)
    local   r={}

    for i=1,32 do
        if  op1[i]==1   then
            r[i]=0
        else
            r[i]=1
        end
    end
    return  self:b2d(r)
end --bit:_not

function    bit:_lshift(a, n)
  if n < 0 or n > 32 then
    return nil;
  end
  
    local op1 = self:d2b(a)
    for k = 1, (32-n) do
      op1[k] = op1[k+n];
    end
    
    for k = (32-(n-1)), 32 do
      op1[k] = 0;
    end
    return  self:b2d(op1)
end --bit:_lshift

function    bit:_rshift(a, n)
    if n < 0 or n > 32 then
    return nil;
  end
  
  local op1 = self:d2b(a);
  for k = 32, (1+n), -1 do
      op1[k] = op1[k-n];
    end
    
    for k = 1, n do
      op1[k] = 0;
    end
    return  self:b2d(op1)
end --bit:_rshift


-- ------------------控件的事件类型-----------------------------
EVENT_BUTTON_CLICK               = 0x1010
EVENT_BUTTON_PRESSED             = 0x1011
EVENT_BUTTON_RELEASED            = 0x1012

EVENT_IMAGE_CLICK                = 0x1020
EVENT_IMAGE_PRESSED              = 0x1021
EVENT_IMAGE_RELEASED             = 0x1022
EVENT_IMAGE_DOUBLECLICK          = 0x1023

EVENT_SLIDER_VALUE_CHANGED       = 0x1080
EVENT_COMBOBOX_SEL_INDEX_CHANGED = 0x1090
EVENT_LISTBOX_SEL_INDEX_CHANGED  = 0x10D0

EVENT_TIMER_TICK                 = 0x10C0
-- ------------------end 控件的事件类型------------------------



-- 编程接口中的动作，
-- 由控件的事件类型转换而来
UI_EVENT_PAGE_SHOW          = 0x1000;                   -- 页面显示
UI_EVENT_CLICK              = 0x1010                    -- 使用了原来按钮的事件编号
UI_EVENT_PRESS              = 0x1011
UI_EVENT_RELEASE            = 0x1012
UI_EVENT_DOUBLE_CLICK       = 0x1013
UI_EVENT_VALUE_CHANGED      = 0x1080


-- 由其他赋值
local l_func_initialize     = nil;
local l_func_uninitialize   = nil;
local l_func_ui_event       = nil;
local l_func_response_data  = nil;

-- 初始化
function __jikong_initialize()
	l_func_initialize = initialize;
	
	if l_func_initialize ~= nil then
		l_func_initialize();
	else
		print("jikong lua function \"initialize\" undefined");
	end;
end
-- 反初始化
function __jikong_uninitialize()
	l_func_uninitialize = uninitialize;
	
	if l_func_uninitialize ~= nil then
		l_func_uninitialize();
	else
		print("jikong lua function \"uninitialize\" undefined");
	end;
end

-- 控件的动作，会被多次调用
function __jikong_ui_event(ui_id, event, data)
	l_func_ui_event = ui_event;
	
	if l_func_ui_event ~= nil then
	
	    -- 图像和按钮控件的事件都统一为一种
	    if     EVENT_IMAGE_CLICK                == event then event = UI_EVENT_CLICK;
	    elseif EVENT_IMAGE_PRESSED              == event then event = UI_EVENT_PRESS;
	    elseif EVENT_IMAGE_RELEASED             == event then event = UI_EVENT_RELEASE;
	    elseif EVENT_IMAGE_DOUBLECLICK          == event then event = UI_EVENT_DOUBLE_CLICK;
	    elseif EVENT_SLIDER_VALUE_CHANGED       == event then event = UI_EVENT_VALUE_CHANGED; 
	    elseif EVENT_COMBOBOX_SEL_INDEX_CHANGED == event then event = UI_EVENT_VALUE_CHANGED; 
	    elseif EVENT_LISTBOX_SEL_INDEX_CHANGED  == event then event = UI_EVENT_VALUE_CHANGED; 
	    end
	    
	    -- 值改变事件，会携带一个整数值
	    if UI_EVENT_VALUE_CHANGED == event then
	       if data ~= nil and string.len(data) >= 12 then
            local srcstr = string.sub(data, 9, 12);
            
            -- 四字节数据转换为一个整数
            local result = 0;
            for k = 1, string.len(srcstr) do                    
              local lv = bit:_lshift(string.byte(srcstr, k), 8 * (4 - k));                    
              result = bit:_or(result, lv);
            end
            data = result;
          end
  	  end
	    
		  l_func_ui_event(ui_id, event, data);
	else
		  print("jikong lua function \"ui_event\" undefined");
	end;
end

-- 设备的回码，会被多次调用
function __jikong_response_data(port_type, port_no, data)
	l_func_response_data = response_data;
	
	if l_func_response_data ~= nil then
		l_func_response_data(port_type, port_no, data);
	else
		--print("jikong lua function \"response_data\" undefined");
	end;
end
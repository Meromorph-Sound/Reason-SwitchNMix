format_version = "1.0"

local Axes = { r=180,g=180,b=180 }
local Rect = { r=80,g=80,b=80,a = 64 }
local Graph = { r=0,g=255,b=0  }

function Clamp(value)
  return math.max(0,math.min(1,value))
end

function ValueOf(sampleRate,alpha,exponent,freq)
  return alpha*math.pow(sampleRate/freq,exponent)
end

function changeTable(freq,name)
  jbox.trace("Setting freq = " .. freq .. "for function " .. name)
  
  local changes = {}
  changes[1] = freq
  
  return {
    gesture_ui_name = jbox.ui_text(name),
    property_changes = changes
  }
end


function drawTextBox(property_values,last_property_values,display_info)
  local f=property_values[1]
  local w=display_info.width
  local h=display_info.height

  local template=jbox.ui_text("simple")
  local fStr=jbox.format_number_as_string(f,0)
  local text=jbox.expand_template_string(template,fStr)
  
  jbox_display.draw_text(
    { left = 0, top = h-1, right = w-1, bottom = 0 },
    "left",
    text,
    "Small label font",
    {255,255,255}
    )

end


function actionTextBox(property_values,display_info,gesture_start_point)
  local y=gesture_start_point.y;
  local f=property_values[1];
  assert(y ~= nil)
  assert(f ~= nil)

  local gestureDefinition = {
    custom_data = { freq = f, y = y },
    handlers = {
      on_tap = "didTap",
      on_update = "didMove",
      on_release = "didMove",
      on_cancel = "didReset"
    }
  }
  return gestureDefinition
end

function didTap(property_values,display_info,gesture_info,custom_data)
  local f = property_values[1]
  local y = gesture_info.current_point.y
  assert(y ~= nil)
  assert(f ~= nil)

  custom_data = { freq = f, y = y }
  return ChangeTable(f,"DidTap")
end

function didMove(property_values,display_info,gesture_info,custom_data) 

  local f = property_values[1]
  local y = gesture_info.current_point.y
  local lastY = custom_data.y
  
  assert(y ~= nil)
  assert(f ~= nil)
  assert(lastY ~= nil)

  local diff = y-lastY
  local mag = math.abs(diff)
  
  if mag>2 then
    if diff > 0 then
      f = math.min(749,f+1)
    elseif diff < 0 then
      f = math.max(0,f-1)
    end
    custom_data.y = y
    
    return ChangeTable(f,"didMove")
  else
    return {}
  end
end

function didReset(property_values,display_info,gesture_info,custom_data)
  local f = custom_data.freq
  assert(f ~= nil)
  
  return ChangeTable(f,"didReset")
end
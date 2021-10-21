format_version = "1.0"

local Axes = { r=180,g=180,b=180 }
local Rect = { r=80,g=80,b=80,a = 64 }
local Green = { r=0,g=255,b=0  }
local Red = { r=255,g=0,b=0 }
local Yellow = {r=255,g=255,b=0 }

  

function clip(value)
  return math.max(0,math.min(1,value))
end


function drawVolume(property_values,display_info,dirty_rect)
  local volume=clip(property_values[1])
  local w = display_info.width
  local h = display_info.height
  
  local colour = Green
  if volume > 0.9 then
    colour = Red
  elseif volume > 0.7 then
    colour = Yellow
  end
  
  local width = w*volume
  jbox_display.draw_rect({left= 0, top= 0, right= width, bottom= h-1},colour)
  
end
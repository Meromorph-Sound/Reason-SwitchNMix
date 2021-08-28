format_version = "2.0"

local LightBlue = {26,130,196}
local White = {255,255,255}



function prop_display(node,N)
  local name = node..N
  return {
    graphics = { node = name },
    value = "/custom_properties/"..name,
  }
end

local w = {
  jbox.device_name { graphics = {node = "deviceName" }},
  jbox.analog_knob {
      graphics = { node = "GAIN" },
      value = "/custom_properties/GAIN"
    }
  }
for N = 1, 4 do
  table.insert(w,jbox.analog_knob(prop_display('DRY_WET',N)))
  table.insert(w,jbox.toggle_button(prop_display('BYPASS',N)))
  if N < 4 then
    table.insert(w,jbox.toggle_button(prop_display('CONNECT',N)))
    end
end

front = jbox.panel { graphics = { node = "Bg" }, widgets = w }

function input(name)
  return jbox.audio_input_socket {graphics = { node = name },socket = "/audio_inputs/"..name }
end
function output(name)
  return jbox.audio_output_socket {graphics = { node = name },socket = "/audio_outputs/"..name }
end

local bw = {
  jbox.placeholder { graphics = { node = "Placeholder" }},
  jbox.device_name {graphics = { node = "deviceName" }},
  input("AudioInL"),
  input("AudioInR"),
  output("AudioOutL"),
  output("AudioOutR")
}

for N=1,4 do
  table.insert(bw,input("AudioInL"..N))
  table.insert(bw,input("AudioInR"..N))
  table.insert(bw,output("AudioOutL"..N))
  table.insert(bw,output("AudioOutR"..N))
end

back = jbox.panel { graphics = { node = "Bg" }, widgets = bw }

folded_front = jbox.panel { 
  graphics = { node = "Bg" },
  widgets = { jbox.device_name { graphics = { node = "deviceName" }}}
}

folded_back = jbox.panel { 
  graphics = { node = "Bg" },
  cable_origin = { node = "CableOrigin" },
  widgets = {}
}

format_version = "3.0"

function append(to,extras)
  for k,v in pairs(extras) do
    to[k]=v 
  end
  return to
end  


  local connections = { 
    "serial", 
    "parallel"
  }

function  apply(fn,list)
  local out={}
  for idx,value in pairs(list) do
    out[idx]=fn(value)
  end
  return out
end

local N_UNITS = 4
local BASES = {
  ["DRY_WET"] = 10,
  ["BYPASS"] = 20,
  ["CONNECT"] = 30
}

function tagFor(unit,N)  return BASES[unit]+N end
function nameFor(unit,N) return unit..N end


local properties = {}

for N = 1, N_UNITS do
  properties[nameFor('DRY_WET',N)] = jbox.number {
    default = 1.0,
    ui_name=jbox.ui_text('DRY_WET'),
    property_tag=tagFor('DRY_WET',N),
    ui_type=jbox.ui_percent { decimals = 2 }
  }
  
 
  properties[nameFor('BYPASS',N)] = jbox.number {
    default=0,
    steps=2,
    ui_name = jbox.ui_text('BYPASS'),  
    property_tag=tagFor('BYPASS',N),
    ui_type = jbox.ui_selector({jbox.UI_TEXT_OFF,jbox.UI_TEXT_ON})
  }
  
  if N < N_UNITS then
    properties[nameFor('CONNECT',N)] = jbox.number {
      default = 0,
      steps = 2,
      ui_name = jbox.ui_text('CONNECT'),
      property_tag = tagFor('CONNECT',N),
      ui_type = jbox.ui_selector(apply(jbox.ui_text,connections))
    }
  end
end

properties['GAIN'] = jbox.number {
  default=1,
  ui_name = jbox.ui_text("GAIN"),  
  property_tag=1,
  ui_type = jbox.ui_percent{decimals=1}
}


custom_properties = jbox.property_set{
  document_owner = {['properties']  = properties },
	rtc_owner = { properties = { instance = jbox.native_object{} } },
	rt_owner = { properties = {} }
}

function remote(name) 
return {
  internal_name = name,
  short_ui_name = jbox.ui_text(name.."_short"),
  shortest_ui_name = jbox.ui_text(name.."_shortest")
}
end

local midi_cc = {}
local remotes = {}

function setMidiRemote(name,N) 
  local prop=nameFor(name,N)
  local pName = "/custom_properties/"..prop
  
  midi_cc[101+tagFor(name,N)]=pName
  remotes[pName]=remote(prop)
end

midi_cc[102]='/custom_properties/GAIN'
remotes['/custom_properties/GAIN']=remote('GAIN')

for N=1,N_UNITS do
  setMidiRemote('DRY_WET',N)
  setMidiRemote('BYPASS',N)
  if N < N_UNITS then
    setMidiRemote('CONNECT',N)
  end
end


-- midi_implementation_chart = { midi_cc_chart = midi_cc }
-- remote_implementation_chart = remotes

ui_groups = {}

cv_inputs = {}
cv_outputs = {}

local int=jbox.ui_text("audioIn")
local outt=jbox.ui_text("audioOut")
local nin = jbox.ui_text("return")
local nout = jbox.ui_text("send")

audio_inputs = {
  AudioInL = jbox.audio_input{ ui_name = int },
	AudioInR = jbox.audio_input{ ui_name = int }
  }
audio_outputs = {
	AudioOutL = jbox.audio_output{ ui_name = outt },
	AudioOutR = jbox.audio_output{ ui_name = outt }
}

for N = 1 , N_UNITS do
  audio_inputs['AudioInL'..N]=jbox.audio_input{ ui_name = nin }
  audio_inputs['AudioInR'..N]=jbox.audio_input{ ui_name = nin }
  audio_outputs['AudioOutL'..N]=jbox.audio_output{ ui_name = nout }
  audio_outputs['AudioOutR'..N]=jbox.audio_output{ ui_name = nout }
end

















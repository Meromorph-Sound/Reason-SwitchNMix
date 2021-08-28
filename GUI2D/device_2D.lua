format_version = "2.0"

local N_UNITS = 4



front = { 
  Bg = {{ path = "Panel_Front_1U" }},
  deviceName = { offset = { 460, 20}, { path = "Tape_Horizontal_1frames", frames = 1 }},
  GAIN = { offset = {3380,90}, { path = "Knob_01_63frames", frames = 63}},
}
for N = 1, N_UNITS do
  local offset=600*(N-1)
  front['DRY_WET'..N] = { offset = { 1040+offset,90 }, { path = "Knob_46_63frames", frames = 63}}
  front['BYPASS'..N] = { offset = { 970+offset,250 }, { path = "Button_53_2frames", frames = 2}}
  if N<N_UNITS then
    front['CONNECT'..N] = { offset = { 1325+offset,150 }, { path = "serial-parallel-2frames", frames = 2}}
  end
end

back = { 
  Bg = {{ path = "Panel_Back_1U" }},
  Placeholder = { offset = { 100, 100 },{ path = "Placeholder" }},
  deviceName = { offset = { 180,230}, { path = "Tape_Horizontal_1frames", frames = 1 }},
  --
  AudioInL = { offset = {1000,80}, { path = "SharedAudioJack", frames = 3}},
  AudioInR = { offset = {1000,200},{ path = "SharedAudioJack", frames = 3}},
  AudioOutL = { offset = {2550,80}, { path = "SharedAudioJack", frames = 3}},
  AudioOutR = { offset = {2550,200},{ path = "SharedAudioJack", frames = 3}}
}
for N=1,N_UNITS do
  local offset=(N-1)*300
  back['AudioOutL'..N] = { offset = {1250+offset,80}, { path = "SharedAudioJack", frames = 3}}
  back['AudioOutR'..N] = { offset = {1250+offset,200}, { path = "SharedAudioJack", frames = 3}}
  back['AudioInL'..N] = { offset = {1370+offset,80}, { path = "SharedAudioJack", frames = 3}}
  back['AudioInR'..N] = { offset = {1370+offset,200}, { path = "SharedAudioJack", frames = 3}}
end

folded_front = { 
  Bg = {{ path = "Panel_Folded_Front" }},
  deviceName = { offset = { 2800, 42}, { path = "Tape_Horizontal_1frames", frames = 1 }},
}
folded_back = { 
  Bg = {{ path = "Panel_Folded_Back" }},
  CableOrigin = { offset = { 1885, 75 }}
}

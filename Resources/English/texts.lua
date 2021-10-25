format_version = "1.0"

texts = {
  ["audioIn"] = "Audio Input",
  ["audioOut"] = "Audio Output",
  ["return"] = "Audio Return",
  ["send"] = "Audio Send",
  
  ["DRY_WET"] = "DryWet",
  ["BYPASS"] = "Bypass",
  ["CONNECT"] = "Connect",
  ["DELAY"] = "Delay",
  
  ['IN_LEDS'] = 'Input',
  ['OUT_LEDS'] = 'Output',
  
  ["GAIN"] = "Gain",
  ["GAIN_short"] = "gain",
  ["GAIN_shortest"] = "GAIN",
  
  ["serial"] = "serial",
  ["parallel"] = "parallel",
  ["VOLUME"] = "volume",
  ["min"] = "min",
  ["max"] = "max",
  ["linear_template"] = "^0"
}

function makeFor(root,short,N)
  local tag=root..N
  local base=texts[root]..N
  texts[tag] = base
  texts[tag.."_short"]=short..N
  texts[tag.."_shortest"]=short..N
end

for N = 1,4 do
  makeFor("DRY_WET","DW",N)
  makeFor("BYPASS","BP",N)
  makeFor("CONNECT","CON",N)
  makeFor('IN_LEDS',"IN",N)
  makeFor('OUT_LEDS',"OUT",N)  
end
  
  
 
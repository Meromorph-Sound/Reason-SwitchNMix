format_version = "1.0"

rtc_bindings = { 
  { source = "/environment/system_sample_rate", dest = "/global_rtc/init_instance" },
  { source = "/environment/instance_id", dest = "/global_rtc/init_instance" }
}

global_rtc = { 

  init_instance = function(source_property_path, instance_id)
    local new_no = jbox.make_native_object_rw("Instance", {instance_id})
    jbox.store_property("/custom_properties/instance", new_no);
  end
}

sample_rate_setup = { 
native = {
    22050,
    44100,
    48000,
    88200,
    96000,
    192000
  },
}

local notes = {
  "/custom_properties/GAIN",
  "/transport/request_reset_audio",
  "/environment/system_sample_rate",
  "/audio_inputs/AudioInL/connected",
  "/audio_inputs/AudioInR/connected",
  "/audio_outputs/AudioOutL/connected",
  "/audio_outputs/AudioOutR/connected",
}
for N = 1,4 do
  table.insert(notes,"/custom_properties/BYPASS"..N)
  table.insert(notes,"/custom_properties/DELAY"..N)
  table.insert(notes,"/custom_properties/DRY_WET"..N)
  table.insert(notes,"/custom_properties/CONNECT"..N)
  table.insert(notes,"/audio_inputs/AudioInL"..N.."/connected")
  table.insert(notes,"/audio_inputs/AudioInR"..N.."/connected")
  table.insert(notes,"/audio_outputs/AudioOutL"..N.."/connected")
  table.insert(notes,"/audio_outputs/AudioOutR"..N.."/connected")
end
 for k,v in pairs(notes) do
  jbox.trace("Making RT notifier for "..v)
end
rt_input_setup = { 
  notify = notes
}


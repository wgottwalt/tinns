#pragma once

static const char *const PatchConfigTemplate[][2] = {
  // {option_name, default_value} if default_value is empty string, it means option is mandatory
  // List ends with empty string for option_name
  {"server_version", "200"},
  {"patchserver_port", "8040"},
  {"patches_path", "./patches"},
  {"file_path", "./files"},
  {"max_file_xfers", "5"},
  {"patch_packet_size", "512"},
  {"maxclients", "5"},
  {"gm_slots", "2"},

  {"", ""} // do not change this line (end mark)
};

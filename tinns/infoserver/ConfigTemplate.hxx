#pragma once

static const char *const InfoConfigTemplate[][2] = {
  // {option_name, default_value} if default_value is empty string, it means option is mandatory
  // List ends with empty string for option_name
  {"sql_host", "127.0.0.1"}, // should be renanmed to info_sql_host
  {"sql_port", "3306"}, // should be renanmed to info_sql_port
  {"sql_username", ""}, // should be renanmed to info_sql_username
  {"sql_password", ""}, // should be renanmed to info_sql_password
  {"global_sql_database", "infoserver"}, // should be renanmed to info_sql_database
  {"mysql_wait_timeout", "28800"}, // value of the wait_timout system variable from the MySQL server (same for game & info DB atm). 0 to disable keepalive.
  {"auto_accounts", "0"},
  {"infoserver_port", "7000"},
  {"gameserver_livecheck", "60"},
  {"minlevel", "0"},
  {"maxclients", "50"},
  {"require_validation", "0"},
  {"isc_method", "1"},
  {"isc_infoserverport", "9991"},
  {"isc_connect_pw", "changeme"},
  {"username_filter", "^[a-z][\\w\\-]{2,14}$"},
  {"password_filter", "^[[:graph:]]{3,15}$"},
  {"sqlite_databasefile", "infoDB.s3db"},
  {"database_type", "sqlite"},

// For futur use:
//  {"max_chars_per_account", "4"},

  {"", ""} // do not change this line (end mark)
};

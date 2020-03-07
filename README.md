TinNS - is not a Neocron Server

Tinns is an old project by the linux-addicted.net community, an attempt to
reverse engineer the Neocron 1 network protocol to write a free Neocron server,
mostly made for Linux (or other unixoide operating systems like BSD).

The project got quite far and some stuff works surprisingly well for a reverse
engineer attempt. This server is meant for Neocron 1 patchlevel 200, the last
version of Necoron 1. The Retail client and the missing patches up to version
200 can be found here http://files.linux-addicted.net/neocron1/.

This is the last version from the linux-addicted.net page, reworked to work with
current compilers and prepared for github. I also started to update and fix the
code because current compilers with enhanced diagnostics show quite some issues.

In this repository I also will add all the tools/docs we used to work on this
project.

Requirements:
- c++17 compiler (gcc 9.x prefered)
- MySQL/MariaDB server OR SQLite3
- PCRE lib
- cmake for building

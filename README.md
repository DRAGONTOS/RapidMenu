# RapidMenu
Utilize configuration files to create runner scripts.

# Requirements
- tomlplusplus
- a runner like rofi, dmenu etc

# Installing
Just run this.
```
make all
sudo make install
```
# Usage
## Help message
```
usage:  RapidMenu [flags] [<command> [args]]
LISTING COMMANDS:
    -c:           To specify which config to use.
    -b:           Make a executable out of a config.
```
### How to use (-c) 
If you want to open a configuration, just use this command.
```
RapidMenu -c [config]
```
### How to use (-b)
Just use this command, it will throw an error if incorrect.
Also, it will ask what you want to name the executable, and if it already exists.
```
RapidMenu -b [config]
```
## Configuration
It is required to have the [runner] section in the configuration.
Example:
```
[runner]
rname = "Dashboard:" (name if needed)
rtheme = "-show-icons -theme ~/.config/rofi/themes/rounded-purple-dark.rasi" (theme if needed)
rcommand = "rofi -dmenu -p" (dmenu, rofi, bemenu etc)
```
An example of something to add to the configuration.
```
[Disk] (To set the name)
names = "Disk" (To set the in the runner)
description = "You chose Disk." (No required but can be set)
command = "kitty -e /home/$USER/.config/RapidMenu/hdd.bash" (bash command)
```
# Contributors
- DRAGONTOS

# Advanced motor control

## Format of data

'd' - Toggle debug
'p' - Print motors status
'slf100e' - Example of sending motor speed setting

Motor speed settings:

| char # | option | comment |
|--------|--------|---------|
| 1      | "s"      | marks beginning of transmission |
| 2      | "l" or "r" | sets motor to control, "l" - left, "r" - right |
| 3      | "f" or "b" or "r" | sets direction to rotate, "f" - forward, "b" - backward, "r" - release |
| 4, 5, 6 | 0 - 255 | sets speed value, leading zeroes are NOT required, e.g. "slf1e" will work just fine |
| 7 | "e" | marks end of transmission |

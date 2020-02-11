# Painter Vector Format

PVF - simple ASCII-based vector graphics format.

## Draw model

Format is not bound to any distance units.
The only requirement is linear scale.

Each command draws on virtual canvas of given size.

Initially canvas is filled with black transparent color.

Each command has following format:

C[:A0[:A1...]];

Where C - single letter command identifier, tokens in square brackets
are optional and provides arguments for command. Each command ends
with ;

If argument is ommited, command uses default value for given argument,
which equals to 0, if command description don't declare opposite.

All numbers are defined with intergral and optional fractional part
after dot.

## Commands

 * f:{X}:{Y}; - defines size of virtual canvas first command in PVF
   X - width of canvas
   Y - height of canvas
 * b:{W}; - set brush settings
   W - width of brush
 * m:{X}:{Y}; - move brush to new position
   X - x-coordinate of new position
   Y - y-coordinate of new position
 * r:{X}:{Y}; - move brush relative to current position
   X - addition to x-coordinate of current position
   Y - addition to y-coordinate of current position
 * l:{X}:{Y}; - draw line to new position
   X - x-coordinate of new position
   Y - y-coordinate of new position
 * c:{R}; - draw circle with center at current position
   R - radius of new circle
 * s:{S}; - circle total side count
   S - circle total side count

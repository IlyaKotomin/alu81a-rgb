# Fish shell completions for alu81a-rgb
# Dark Project Terra Nova / Nostra (ALU81A) RGB controller

# Don't complete files by default
complete -c alu81a-rgb -f

# Flags
complete -c alu81a-rgb -s h -l help         -d 'Show help and exit'
complete -c alu81a-rgb -s v -l verbose      -d 'Show device path and HID payload'
complete -c alu81a-rgb      -l off          -d 'Turn off all LEDs'
complete -c alu81a-rgb      -l list-modes   -d 'List all effect modes and exit'
complete -c alu81a-rgb      -l list-colors  -d 'List all named colors and exit'

# Colors!!!!!
complete -c alu81a-rgb -l color -d 'Set color by name' -r -a "
    red\t'#ff0000'
    green\t'#00ff00'
    blue\t'#0000ff'
    white\t'#ffffff'
    black\t'Off / #000000'
    cyan\t'#00ffff'
    yellow\t'#ffff00'
    magenta\t'#ff00ff'
    orange\t'#ff8000'
    purple\t'#8000ff'
    pink\t'#ff0080'
    teal\t'#008080'
    lime\t'#80ff00'
    gold\t'#ffd700'
    coral\t'#ff6040'
    indigo\t'#4b0082'
    violet\t'#ee82ee'
    turquoise\t'#40e0d0'
    navy\t'#000080'
    maroon\t'#800000'
    olive\t'#808000'
    silver\t'#c0c0c0'
    aqua\t'#00ffff'
    crimson\t'#dc143c'
    azure\t'#0080ff'
    mint\t'#00ff80'
    rose\t'#ff0040'
    amber\t'#ffbf00'
    sky\t'#00bfff'
"

complete -c alu81a-rgb -l hex -d 'Set color by hex (RRGGBB or #RRGGBB)' -r
complete -c alu81a-rgb -l rgb -d 'Set color by RGB (R,G,B  0-255 each)'  -r
complete -c alu81a-rgb -l hsl -d 'Set color by HSL (H,S,L  H:0-360 S/L:0-100)' -r

# Effect
complete -c alu81a-rgb -s m -l mode -d 'Effect mode' -r -a "
    disable\t'All LEDs off'
    direct\t'Direct Control'
    solid\t'Solid Color'
    alphas-mods\t'Alphas Mods'
    gradient-ud\t'Gradient Up Down'
    gradient-lr\t'Gradient Left Right'
    breathing\t'Breathing'
    band-sat\t'Band Sat'
    band-val\t'Band Val'
    band-pinwheel-sat\t'Band Pinwheel Sat'
    band-pinwheel-val\t'Band Pinwheel Val'
    band-spiral-sat\t'Band Spiral Sat'
    band-spiral-val\t'Band Spiral Val'
    cycle-all\t'Cycle All'
    cycle-lr\t'Cycle Left Right'
    cycle-ud\t'Cycle Up Down'
    rainbow-chevron\t'Rainbow Moving Chevron'
    cycle-out-in\t'Cycle Out In'
    cycle-out-in-dual\t'Cycle Out In Dual'
    cycle-pinwheel\t'Cycle Pinwheel'
    cycle-spiral\t'Cycle Spiral'
    dual-beacon\t'Dual Beacon'
    rainbow-beacon\t'Rainbow Beacon'
    rainbow-pinwheels\t'Rainbow Pinwheels'
    raindrops\t'Raindrops'
    jellybean-raindrops\t'Jellybean Raindrops'
    hue-breathing\t'Hue Breathing'
    hue-pendulum\t'Hue Pendulum'
    hue-wave\t'Hue Wave'
    typing-heatmap\t'Typing Heatmap'
    digital-rain\t'Digital Rain'
    solid-reactive-simple\t'Solid Reactive Simple'
    solid-reactive\t'Solid Reactive'
    solid-reactive-wide\t'Solid Reactive Wide'
    solid-reactive-multiwide\t'Solid Reactive Multiwide'
    solid-reactive-cross\t'Solid Reactive Cross'
    solid-reactive-multicross\t'Solid Reactive Multicross'
    solid-reactive-nexus\t'Solid Reactive Nexus'
    solid-reactive-multinexus\t'Solid Reactive Multinexus'
    splash\t'Splash'
    multisplash\t'Multisplash'
    solid-splash\t'Solid Splash'
    solid-multisplash\t'Solid Multisplash'
    pixel-rain\t'Pixel Rain'
    pixel-fractal\t'Pixel Fractal'
"

complete -c alu81a-rgb -s s -l speed      -d 'Animation speed 0-255 (255=fastest)' -r
complete -c alu81a-rgb -s b -l brightness -d 'Global brightness 0-100 percent'     -r

# Device
complete -c alu81a-rgb -l device -d 'Override hidraw device path' -r \
    -a "(ls /dev/hidraw* 2>/dev/null)"

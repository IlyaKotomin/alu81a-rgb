#compdef alu81a-rgb

# Zsh completion for alu81a-rgb

_alu81a_rgb() {
    local -a modes colors hidraw_devs

    modes=(
        'disable:All LEDs off'
        'direct:Direct Control'
        'solid:Solid Color'
        'alphas-mods:Alphas Mods'
        'gradient-ud:Gradient Up Down'
        'gradient-lr:Gradient Left Right'
        'breathing:Breathing'
        'band-sat:Band Sat'
        'band-val:Band Val'
        'band-pinwheel-sat:Band Pinwheel Sat'
        'band-pinwheel-val:Band Pinwheel Val'
        'band-spiral-sat:Band Spiral Sat'
        'band-spiral-val:Band Spiral Val'
        'cycle-all:Cycle All'
        'cycle-lr:Cycle Left Right'
        'cycle-ud:Cycle Up Down'
        'rainbow-chevron:Rainbow Moving Chevron'
        'cycle-out-in:Cycle Out In'
        'cycle-out-in-dual:Cycle Out In Dual'
        'cycle-pinwheel:Cycle Pinwheel'
        'cycle-spiral:Cycle Spiral'
        'dual-beacon:Dual Beacon'
        'rainbow-beacon:Rainbow Beacon'
        'rainbow-pinwheels:Rainbow Pinwheels'
        'raindrops:Raindrops'
        'jellybean-raindrops:Jellybean Raindrops'
        'hue-breathing:Hue Breathing'
        'hue-pendulum:Hue Pendulum'
        'hue-wave:Hue Wave'
        'typing-heatmap:Typing Heatmap'
        'digital-rain:Digital Rain'
        'solid-reactive-simple:Solid Reactive Simple'
        'solid-reactive:Solid Reactive'
        'solid-reactive-wide:Solid Reactive Wide'
        'solid-reactive-multiwide:Solid Reactive Multiwide'
        'solid-reactive-cross:Solid Reactive Cross'
        'solid-reactive-multicross:Solid Reactive Multicross'
        'solid-reactive-nexus:Solid Reactive Nexus'
        'solid-reactive-multinexus:Solid Reactive Multinexus'
        'splash:Splash'
        'multisplash:Multisplash'
        'solid-splash:Solid Splash'
        'solid-multisplash:Solid Multisplash'
        'pixel-rain:Pixel Rain'
        'pixel-fractal:Pixel Fractal'
    )

    colors=(
        'red:#ff0000'    'green:#00ff00'   'blue:#0000ff'
        'white:#ffffff'  'black:Off/#000000' 'cyan:#00ffff'
        'yellow:#ffff00' 'magenta:#ff00ff' 'orange:#ff8000'
        'purple:#8000ff' 'pink:#ff0080'    'teal:#008080'
        'lime:#80ff00'   'gold:#ffd700'    'coral:#ff6040'
        'indigo:#4b0082' 'violet:#ee82ee'  'turquoise:#40e0d0'
        'navy:#000080'   'maroon:#800000'  'olive:#808000'
        'silver:#c0c0c0' 'aqua:#00ffff'    'crimson:#dc143c'
        'azure:#0080ff'  'mint:#00ff80'    'rose:#ff0040'
        'amber:#ffbf00'  'sky:#00bfff'
    )

    hidraw_devs=( /dev/hidraw*(N) )

    _arguments -s \
        '(-h --help)'{-h,--help}'[Show help and exit]' \
        '(-v --verbose)'{-v,--verbose}'[Show device path and HID payload]' \
        '--off[Turn off all LEDs]' \
        '--list-modes[Print all effect modes and exit]' \
        '--list-colors[Print all named colors and exit]' \
        '--color[Set color by name]:color name:->colors' \
        '--hex[Set color by hex (RRGGBB or \#RRGGBB)]:hex color:' \
        '--rgb[Set color by RGB values (R\,G\,B)]:R,G,B:' \
        '--hsl[Set color by HSL (H\,S\,L)]:H,S,L:' \
        '(-m --mode)'{-m,--mode}'[Effect mode]:mode:->modes' \
        '(-s --speed)'{-s,--speed}'[Animation speed 0-255]:speed (0-255):' \
        '(-b --brightness)'{-b,--brightness}'[Global brightness 0-100]:brightness (0-100):' \
        "--device[Override hidraw device path]:device:($hidraw_devs)"

    case $state in
        modes)  _describe 'mode' modes ;;
        colors) _describe 'color' colors ;;
    esac
}

_alu81a_rgb

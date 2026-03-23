# Bash completion for alu81a-rgb

_alu81a_rgb()
{
    local cur prev
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"

    local opts="--color --hex --rgb --hsl
                -m --mode -s --speed -b --brightness
                --off --device --no-second-report --delay
                --list-modes --list-colors
                -v --verbose -h --help"

    local modes="disable direct solid alphas-mods gradient-ud gradient-lr
        breathing band-sat band-val band-pinwheel-sat band-pinwheel-val
        band-spiral-sat band-spiral-val cycle-all cycle-lr cycle-ud
        rainbow-chevron cycle-out-in cycle-out-in-dual cycle-pinwheel
        cycle-spiral dual-beacon rainbow-beacon rainbow-pinwheels raindrops
        jellybean-raindrops hue-breathing hue-pendulum hue-wave typing-heatmap
        digital-rain solid-reactive-simple solid-reactive solid-reactive-wide
        solid-reactive-multiwide solid-reactive-cross solid-reactive-multicross
        solid-reactive-nexus solid-reactive-multinexus splash multisplash
        solid-splash solid-multisplash pixel-rain pixel-fractal"

    local colors="red green blue white black cyan yellow magenta orange purple
        pink teal lime gold coral indigo violet turquoise navy maroon olive
        silver aqua crimson azure mint rose amber sky"

    case "${prev}" in
        --color)
            COMPREPLY=( $(compgen -W "${colors}" -- "${cur}") )
            return 0 ;;
        -m|--mode)
            COMPREPLY=( $(compgen -W "${modes}" -- "${cur}") )
            return 0 ;;
        --device)
            COMPREPLY=( $(compgen -G "/dev/hidraw*") )
            return 0 ;;
        --hex|--rgb|--hsl|-s|--speed|-b|--brightness|--delay)
            return 0 ;;
    esac

    COMPREPLY=( $(compgen -W "${opts}" -- "${cur}") )
}

complete -F _alu81a_rgb alu81a-rgb

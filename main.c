/*
 * alu81a-rgb - RGB controller for Dark Project Terra Nova / Nostra (ALU81A)
 * Vendor: 0x342D  Product: 0xE40B
 * Protocol: https://github.com/IlyaKotomin/terra-nova-nostra-rgb-protocol
 */

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define VENDOR_ID   0x342Du
#define PRODUCT_ID  0xE40Bu

#define REPORT_SIZE  32
#define CMD_SET      0x41u
#define REPORT_ID_A  0x07u
#define REPORT_ID_B  0x08u

#define DEFAULT_MODE        0x02u
#define DEFAULT_SPEED       0xFFu
#define DEFAULT_BRIGHTNESS  100u

typedef struct {
    uint8_t     id;
    const char *name;
    const char *slug;
} Mode;

static const Mode MODES[] = {
    {0x00, "Disable",                   "disable"},
    {0x01, "Direct Control",            "direct"},
    {0x02, "Solid Color",               "solid"},
    {0x03, "Alphas Mods",               "alphas-mods"},
    {0x04, "Gradient Up Down",          "gradient-ud"},
    {0x05, "Gradient Left Right",       "gradient-lr"},
    {0x06, "Breathing",                 "breathing"},
    {0x07, "Band Sat",                  "band-sat"},
    {0x08, "Band Val",                  "band-val"},
    {0x09, "Band Pinwheel Sat",         "band-pinwheel-sat"},
    {0x0A, "Band Pinwheel Val",         "band-pinwheel-val"},
    {0x0B, "Band Spiral Sat",           "band-spiral-sat"},
    {0x0C, "Band Spiral Val",           "band-spiral-val"},
    {0x0D, "Cycle All",                 "cycle-all"},
    {0x0E, "Cycle Left Right",          "cycle-lr"},
    {0x0F, "Cycle Up Down",             "cycle-ud"},
    {0x10, "Rainbow Moving Chevron",    "rainbow-chevron"},
    {0x11, "Cycle Out In",              "cycle-out-in"},
    {0x12, "Cycle Out In Dual",         "cycle-out-in-dual"},
    {0x13, "Cycle Pinwheel",            "cycle-pinwheel"},
    {0x14, "Cycle Spiral",              "cycle-spiral"},
    {0x15, "Dual Beacon",               "dual-beacon"},
    {0x16, "Rainbow Beacon",            "rainbow-beacon"},
    {0x17, "Rainbow Pinwheels",         "rainbow-pinwheels"},
    {0x18, "Raindrops",                 "raindrops"},
    {0x19, "Jellybean Raindrops",       "jellybean-raindrops"},
    {0x1A, "Hue Breathing",             "hue-breathing"},
    {0x1B, "Hue Pendulum",              "hue-pendulum"},
    {0x1C, "Hue Wave",                  "hue-wave"},
    {0x1D, "Typing Heatmap",            "typing-heatmap"},
    {0x1E, "Digital Rain",              "digital-rain"},
    {0x1F, "Solid Reactive Simple",     "solid-reactive-simple"},
    {0x20, "Solid Reactive",            "solid-reactive"},
    {0x21, "Solid Reactive Wide",       "solid-reactive-wide"},
    {0x22, "Solid Reactive Multiwide",  "solid-reactive-multiwide"},
    {0x23, "Solid Reactive Cross",      "solid-reactive-cross"},
    {0x24, "Solid Reactive Multicross", "solid-reactive-multicross"},
    {0x25, "Solid Reactive Nexus",      "solid-reactive-nexus"},
    {0x26, "Solid Reactive Multinexus", "solid-reactive-multinexus"},
    {0x27, "Splash",                    "splash"},
    {0x28, "Multisplash",               "multisplash"},
    {0x29, "Solid Splash",              "solid-splash"},
    {0x2A, "Solid Multisplash",         "solid-multisplash"},
    {0x2B, "Pixel Rain",                "pixel-rain"},
    {0x2C, "Pixel Fractal",             "pixel-fractal"},
};
#define N_MODES  (sizeof(MODES) / sizeof(MODES[0]))

/* RGB (0-255) to keyboard hue/sat bytes. Brightness is a separate dimmer. */
static void rgb_to_hs(uint8_t r, uint8_t g, uint8_t b,
                      uint8_t *hue_out, uint8_t *sat_out)
{
    double rf = r / 255.0, gf = g / 255.0, bf = b / 255.0;
    double cmax  = fmax(rf, fmax(gf, bf));
    double cmin  = fmin(rf, fmin(gf, bf));
    double delta = cmax - cmin;
    double h = 0.0, s = 0.0;

    if (delta > 1e-9) {
        if (cmax == rf)
            h = 60.0 * fmod((gf - bf) / delta, 6.0);
        else if (cmax == gf)
            h = 60.0 * ((bf - rf) / delta + 2.0);
        else
            h = 60.0 * ((rf - gf) / delta + 4.0);

        if (h < 0.0) h += 360.0;
        s = (cmax > 1e-9) ? delta / cmax : 0.0;
    }

    *hue_out = (uint8_t)round(h / 360.0 * 255.0);
    *sat_out = (uint8_t)round(s * 255.0);
}

/* HSL (h: 0-360, s: 0-100, l: 0-100) to RGB (0-255) */
static void hsl_to_rgb(double h, double s, double l,
                       uint8_t *r, uint8_t *g, uint8_t *b)
{
    s /= 100.0;
    l /= 100.0;

    double c  = (1.0 - fabs(2.0 * l - 1.0)) * s;
    double x  = c * (1.0 - fabs(fmod(h / 60.0, 2.0) - 1.0));
    double m  = l - c / 2.0;
    double rf, gf, bf;

    if      (h < 60.0)  { rf = c; gf = x; bf = 0; }
    else if (h < 120.0) { rf = x; gf = c; bf = 0; }
    else if (h < 180.0) { rf = 0; gf = c; bf = x; }
    else if (h < 240.0) { rf = 0; gf = x; bf = c; }
    else if (h < 300.0) { rf = x; gf = 0; bf = c; }
    else                { rf = c; gf = 0; bf = x; }

    *r = (uint8_t)round((rf + m) * 255.0);
    *g = (uint8_t)round((gf + m) * 255.0);
    *b = (uint8_t)round((bf + m) * 255.0);
}

/* Collect all /dev/hidrawN paths matching VENDOR_ID:PRODUCT_ID, sorted numerically.
 * The keyboard exposes several interfaces with the same VID:PID; only one accepts
 * RGB reports, so we probe each in order until one succeeds. */
#define MAX_CANDIDATES 16

static int find_all_devices(char paths[][256], int max)
{
    DIR *dir = opendir("/sys/class/hidraw");
    if (!dir) {
        fprintf(stderr, "Cannot open /sys/class/hidraw: %s\n", strerror(errno));
        return 0;
    }

    int indices[MAX_CANDIDATES];
    int count = 0;

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL && count < max) {
        if (strncmp(ent->d_name, "hidraw", 6) != 0)
            continue;

        char uevent[300];
        snprintf(uevent, sizeof(uevent),
                 "/sys/class/hidraw/%s/device/uevent", ent->d_name);

        FILE *f = fopen(uevent, "r");
        if (!f)
            continue;

        char line[256];
        int  match = 0;
        while (fgets(line, sizeof(line), f)) {
            if (strncmp(line, "HID_ID=", 7) != 0)
                continue;
            unsigned bus, vendor, product;
            if (sscanf(line + 7, "%04X:%08X:%08X", &bus, &vendor, &product) == 3)
                if (vendor == VENDOR_ID && product == PRODUCT_ID)
                    match = 1;
            break;
        }
        fclose(f);

        if (match)
            indices[count++] = atoi(ent->d_name + 6);
    }
    closedir(dir);

    for (int i = 0; i < count - 1; i++)
        for (int j = i + 1; j < count; j++)
            if (indices[j] < indices[i]) {
                int tmp = indices[i]; indices[i] = indices[j]; indices[j] = tmp;
            }

    for (int i = 0; i < count; i++)
        snprintf(paths[i], 256, "/dev/hidraw%d", indices[i]);

    return count;
}

/* Returns 0 on success, errno on failure. */
static int send_report(int fd, uint8_t report_id,
                       uint8_t mode, uint8_t speed,
                       uint8_t hue,  uint8_t sat, uint8_t brightness)
{
    uint8_t buf[REPORT_SIZE];
    memset(buf, 0, sizeof(buf));
    buf[0] = report_id;
    buf[1] = CMD_SET;
    buf[2] = mode;
    buf[3] = 0x00;
    buf[4] = speed;
    buf[5] = hue;
    buf[6] = sat;
    buf[7] = brightness;

    ssize_t n = write(fd, buf, REPORT_SIZE);
    if (n < 0)      return errno;
    if (n != REPORT_SIZE) return EIO;
    return 0;
}

/* The protocol requires sending the same command twice: report 0x07 then 0x08.
 * Returns 0 on success, EPIPE if this interface rejected it (wrong interface),
 * or another errno for a real error. */
static int send_command(int fd, uint8_t mode, uint8_t speed,
                        uint8_t hue, uint8_t sat, uint8_t brightness)
{
    int err;
    err = send_report(fd, REPORT_ID_A, mode, speed, hue, sat, brightness);
    if (err) return err;
    err = send_report(fd, REPORT_ID_B, mode, speed, hue, sat, brightness);
    return err;
}

/* Accepts slug, full name (case-insensitive), or a decimal/0xHEX number. */
static int resolve_mode(const char *s)
{
    char *end;
    long val = strtol(s, &end, 0);
    if (*end == '\0' && val >= 0 && val < (long)N_MODES) {
        for (size_t i = 0; i < N_MODES; i++)
            if (MODES[i].id == (uint8_t)val)
                return (int)val;
    }

    for (size_t i = 0; i < N_MODES; i++) {
        if (strcasecmp(s, MODES[i].slug) == 0 ||
            strcasecmp(s, MODES[i].name) == 0)
            return MODES[i].id;
    }

    return -1;
}

static int parse_rgb(const char *s, uint8_t *r, uint8_t *g, uint8_t *b)
{
    int ri, gi, bi;
    if (sscanf(s, "%d,%d,%d", &ri, &gi, &bi) != 3)
        return -1;
    if (ri < 0 || ri > 255 || gi < 0 || gi > 255 || bi < 0 || bi > 255)
        return -1;
    *r = (uint8_t)ri;
    *g = (uint8_t)gi;
    *b = (uint8_t)bi;
    return 0;
}

static int parse_hex(const char *s, uint8_t *r, uint8_t *g, uint8_t *b)
{
    if (*s == '#') s++;
    if (strlen(s) != 6) return -1;
    for (int i = 0; i < 6; i++)
        if (!isxdigit((unsigned char)s[i])) return -1;

    unsigned rv, gv, bv;
    sscanf(s, "%02x%02x%02x", &rv, &gv, &bv);
    *r = (uint8_t)rv;
    *g = (uint8_t)gv;
    *b = (uint8_t)bv;
    return 0;
}

static int parse_hsl(const char *s, double *h, double *sl, double *l)
{
    char hs[32], ss[32], ls[32];
    if (sscanf(s, "%31[^,],%31[^,],%31s", hs, ss, ls) != 3)
        return -1;

    *h  = strtod(hs, NULL);
    *sl = strtod(ss, NULL);
    *l  = strtod(ls, NULL);

    if (*h < 0.0 || *h > 360.0) return -1;
    if (*sl < 0.0 || *sl > 100.0) return -1;
    if (*l  < 0.0 || *l  > 100.0) return -1;
    return 0;
}

typedef struct { const char *name; uint8_t r, g, b; } NamedColor;

static const NamedColor NAMED_COLORS[] = {
    {"red",       0xFF, 0x00, 0x00},
    {"green",     0x00, 0xFF, 0x00},
    {"blue",      0x00, 0x00, 0xFF},
    {"white",     0xFF, 0xFF, 0xFF},
    {"black",     0x00, 0x00, 0x00},
    {"cyan",      0x00, 0xFF, 0xFF},
    {"yellow",    0xFF, 0xFF, 0x00},
    {"magenta",   0xFF, 0x00, 0xFF},
    {"orange",    0xFF, 0x80, 0x00},
    {"purple",    0x80, 0x00, 0xFF},
    {"pink",      0xFF, 0x00, 0x80},
    {"teal",      0x00, 0x80, 0x80},
    {"lime",      0x80, 0xFF, 0x00},
    {"gold",      0xFF, 0xD7, 0x00},
    {"coral",     0xFF, 0x60, 0x40},
    {"indigo",    0x4B, 0x00, 0x82},
    {"violet",    0xEE, 0x82, 0xEE},
    {"turquoise", 0x40, 0xE0, 0xD0},
    {"navy",      0x00, 0x00, 0x80},
    {"maroon",    0x80, 0x00, 0x00},
    {"olive",     0x80, 0x80, 0x00},
    {"silver",    0xC0, 0xC0, 0xC0},
    {"aqua",      0x00, 0xFF, 0xFF},
    {"crimson",   0xDC, 0x14, 0x3C},
    {"azure",     0x00, 0x80, 0xFF},
    {"mint",      0x00, 0xFF, 0x80},
    {"rose",      0xFF, 0x00, 0x40},
    {"amber",     0xFF, 0xBF, 0x00},
    {"sky",       0x00, 0xBF, 0xFF},
};
#define N_NAMED_COLORS  (sizeof(NAMED_COLORS) / sizeof(NAMED_COLORS[0]))

static int resolve_named_color(const char *s, uint8_t *r, uint8_t *g, uint8_t *b)
{
    for (size_t i = 0; i < N_NAMED_COLORS; i++) {
        if (strcasecmp(s, NAMED_COLORS[i].name) == 0) {
            *r = NAMED_COLORS[i].r;
            *g = NAMED_COLORS[i].g;
            *b = NAMED_COLORS[i].b;
            return 0;
        }
    }
    return -1;
}

static void print_named_colors(void)
{
    printf("Named colors (use with --color NAME):\n");
    for (size_t i = 0; i < N_NAMED_COLORS; i++)
        printf("  %-12s  #%02x%02x%02x\n",
               NAMED_COLORS[i].name,
               NAMED_COLORS[i].r,
               NAMED_COLORS[i].g,
               NAMED_COLORS[i].b);
}

static void print_modes(void)
{
    printf("Available modes:\n");
    printf("  %-4s  %-36s  %s\n", "ID", "Name", "Slug");
    printf("  %-4s  %-36s  %s\n", "----", "------------------------------------",
           "----------------------------");
    for (size_t i = 0; i < N_MODES; i++)
        printf("  %-4u  %-36s  %s\n", MODES[i].id, MODES[i].name, MODES[i].slug);
}

static void usage(const char *prog)
{
    printf(
"Usage: %s [COLOR] [EFFECT] [OPTIONS]\n"
"\n"
"RGB controller for Dark Project Terra Nova / Nostra (ALU81A keyboard)\n"
"Device: VID=0x342D  PID=0xE40B\n"
"\n"
"Color (pick one):\n"
"  --color NAME         Named color (red, blue, green, orange, ...)\n"
"  --hex RRGGBB         Hex color string (with or without '#')\n"
"  --rgb R,G,B          RGB values, 0-255 each\n"
"  --hsl H,S,L          HSL values  H: 0-360  S/L: 0-100\n"
"\n"
"Effect:\n"
"  -m, --mode NAME|ID   Effect mode slug/name/number (default: solid)\n"
"  -s, --speed N        Animation speed 0-255 (default: 255 = fastest)\n"
"  -b, --brightness N   Global brightness 0-100 %% (default: 100)\n"
"\n"
"Shortcuts:\n"
"  --off                Turn off all LEDs\n"
"\n"
"Other:\n"
"  --device PATH        Override auto-detected hidraw device path\n"
"  --list-modes         Print all effect modes and exit\n"
"  --list-colors        Print all named colors and exit\n"
"  -v, --verbose        Show device path and HID payload\n"
"  -h, --help           Show this help and exit\n"
"\n"
"Examples:\n"
"  %s --color orange\n"
"  %s --color red --mode breathing -b 80\n"
"  %s --hex ff4400 -m pixel-fractal -s 180\n"
"  %s --rgb 0,255,128 -b 60\n"
"  %s --hsl 240,100,50 --mode breathing\n"
"  %s --off\n",
    prog, prog, prog, prog, prog, prog, prog);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    const char *color_name = NULL;
    const char *color_rgb  = NULL;
    const char *color_hex  = NULL;
    const char *color_hsl  = NULL;
    const char *mode_arg   = NULL;
    const char *device_arg = NULL;
    int speed_arg      = -1;
    int brightness_arg = -1;
    int flag_off       = 0;
    int flag_verbose   = 0;
    int flag_modes     = 0;
    int flag_colors    = 0;

    for (int i = 1; i < argc; i++) {
        const char *a = argv[i];

#define NEED_ARG(flag) \
        do { if (i + 1 >= argc) { \
            fprintf(stderr, "Error: %s requires an argument\n", flag); \
            return 1; \
        } } while (0)

        if (strcmp(a, "-h") == 0 || strcmp(a, "--help") == 0) {
            usage(argv[0]);
            return 0;
        } else if (strcmp(a, "--list-modes") == 0) {
            flag_modes = 1;
        } else if (strcmp(a, "--list-colors") == 0) {
            flag_colors = 1;
        } else if (strcmp(a, "-v") == 0 || strcmp(a, "--verbose") == 0) {
            flag_verbose = 1;
        } else if (strcmp(a, "--off") == 0) {
            flag_off = 1;
        } else if (strcmp(a, "--color") == 0) {
            NEED_ARG("--color");
            color_name = argv[++i];
        } else if (strcmp(a, "--rgb") == 0) {
            NEED_ARG("--rgb");
            color_rgb = argv[++i];
        } else if (strcmp(a, "--hex") == 0) {
            NEED_ARG("--hex");
            color_hex = argv[++i];
        } else if (strcmp(a, "--hsl") == 0) {
            NEED_ARG("--hsl");
            color_hsl = argv[++i];
        } else if (strcmp(a, "-m") == 0 || strcmp(a, "--mode") == 0) {
            NEED_ARG(a);
            mode_arg = argv[++i];
        } else if (strcmp(a, "-s") == 0 || strcmp(a, "--speed") == 0) {
            NEED_ARG(a);
            speed_arg = atoi(argv[++i]);
        } else if (strcmp(a, "-b") == 0 || strcmp(a, "--brightness") == 0) {
            NEED_ARG(a);
            brightness_arg = atoi(argv[++i]);
        } else if (strcmp(a, "--device") == 0) {
            NEED_ARG("--device");
            device_arg = argv[++i];
        } else {
            fprintf(stderr, "Error: unknown option '%s'  (try --help)\n", a);
            return 1;
        }

#undef NEED_ARG
    }

    if (flag_modes)  { print_modes();        return 0; }
    if (flag_colors) { print_named_colors(); return 0; }

    int n_color = (color_name != NULL) + (color_rgb != NULL) +
                  (color_hex  != NULL) + (color_hsl != NULL);
    if (!flag_off && n_color > 1) {
        fprintf(stderr, "Error: specify only one of --color, --hex, --rgb, --hsl\n");
        return 1;
    }

    if (speed_arg != -1 && (speed_arg < 0 || speed_arg > 255)) {
        fprintf(stderr, "Error: --speed must be 0-255\n");
        return 1;
    }
    if (brightness_arg != -1 && (brightness_arg < 0 || brightness_arg > 100)) {
        fprintf(stderr, "Error: --brightness must be 0-100\n");
        return 1;
    }

    uint8_t hue = 0, sat = 0xFF;

    if (flag_off) {
        hue = 0; sat = 0;
        if (mode_arg       == NULL) mode_arg       = "disable";
        if (brightness_arg == -1)   brightness_arg = 0;
    } else if (color_name != NULL) {
        uint8_t r, g, b;
        if (resolve_named_color(color_name, &r, &g, &b) < 0) {
            fprintf(stderr, "Error: unknown color name '%s'  (try --list-colors)\n",
                    color_name);
            return 1;
        }
        rgb_to_hs(r, g, b, &hue, &sat);
    } else if (color_rgb != NULL) {
        uint8_t r, g, b;
        if (parse_rgb(color_rgb, &r, &g, &b) < 0) {
            fprintf(stderr, "Error: invalid --rgb '%s'  (expected R,G,B with values 0-255)\n",
                    color_rgb);
            return 1;
        }
        rgb_to_hs(r, g, b, &hue, &sat);
    } else if (color_hex != NULL) {
        uint8_t r, g, b;
        if (parse_hex(color_hex, &r, &g, &b) < 0) {
            fprintf(stderr, "Error: invalid --hex '%s'  (expected RRGGBB or #RRGGBB)\n",
                    color_hex);
            return 1;
        }
        rgb_to_hs(r, g, b, &hue, &sat);
    } else if (color_hsl != NULL) {
        double h, s, l;
        if (parse_hsl(color_hsl, &h, &s, &l) < 0) {
            fprintf(stderr, "Error: invalid --hsl '%s'  (expected H,S,L  H:0-360 S/L:0-100)\n",
                    color_hsl);
            return 1;
        }
        uint8_t r, g, b;
        hsl_to_rgb(h, s, l, &r, &g, &b);
        rgb_to_hs(r, g, b, &hue, &sat);
    } else {
        hue = 0x00;
        sat = 0xFF;
    }

    uint8_t mode_id = DEFAULT_MODE;
    if (mode_arg != NULL) {
        int m = resolve_mode(mode_arg);
        if (m < 0) {
            fprintf(stderr, "Error: unknown mode '%s'  (try --list-modes)\n", mode_arg);
            return 1;
        }
        mode_id = (uint8_t)m;
    }

    uint8_t speed      = (speed_arg      >= 0) ? (uint8_t)speed_arg      : DEFAULT_SPEED;
    uint8_t brightness = (brightness_arg >= 0) ? (uint8_t)brightness_arg : DEFAULT_BRIGHTNESS;

    char cand[MAX_CANDIDATES][256];
    int  n_cand = 0;

    if (device_arg != NULL) {
        strncpy(cand[0], device_arg, 255);
        cand[0][255] = '\0';
        n_cand = 1;
    } else {
        n_cand = find_all_devices(cand, MAX_CANDIDATES);
        if (n_cand == 0) {
            fprintf(stderr,
                    "Error: keyboard not found (VID=0x%04X PID=0x%04X).\n"
                    "  Is it connected? Do you have read/write access to /dev/hidraw*?\n"
                    "  Try: sudo alu81a-rgb ...  or install the udev rule.\n",
                    VENDOR_ID, PRODUCT_ID);
            return 1;
        }
    }

    for (int i = 0; i < n_cand; i++) {
        int fd = open(cand[i], O_RDWR);
        if (fd < 0) {
            if (flag_verbose)
                printf("Skip     %s  (open: %s)\n", cand[i], strerror(errno));
            continue;
        }

        int err = send_command(fd, mode_id, speed, hue, sat, brightness);
        close(fd);

        if (err == 0) {
            if (flag_verbose) {
                printf("Device   %s\n", cand[i]);
                printf("Payload  mode=0x%02X speed=0x%02X hue=0x%02X sat=0x%02X bri=%u\n",
                       mode_id, speed, hue, sat, brightness);
            }
            return 0;
        }

        if (err == EPIPE) {
            if (flag_verbose)
                printf("Skip     %s  (wrong interface)\n", cand[i]);
            continue;
        }

        fprintf(stderr, "Error: write to '%s' failed: %s\n", cand[i], strerror(err));
        return 1;
    }

    fprintf(stderr,
            "Error: none of the %d matching hidraw interface(s) accepted the command.\n"
            "  Try running with -v for details.\n", n_cand);
    return 1;
}

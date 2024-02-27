#include <stdint.h>

#include "freetype/fttypes.h"
#define _POSIX_C_SOURCE 1
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <ft2build.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

#include "../xml/xdg-shell-client-protocol.h"
#include "../xml/zwlr-protocol.h"
#include "config.h"
#include FT_FREETYPE_H

struct repeatInfo {
   uint32_t rate;
   uint32_t delay;
};

typedef struct pointerC {
   int32_t x;
   int32_t y;
}PointerC;

typedef struct Color {
   float r;
   float g;
   float b;
   float a;
} Color;

enum pointer_event_mask {
   POINTER_EVENT_ENTER = 1 << 0,
   POINTER_EVENT_LEAVE = 1 << 1,
   POINTER_EVENT_MOTION = 1 << 2,
   POINTER_EVENT_BUTTON = 1 << 3,
   POINTER_EVENT_AXIS = 1 << 4,
   POINTER_EVENT_AXIS_SOURCE = 1 << 5,
   POINTER_EVENT_AXIS_STOP = 1 << 6,
   POINTER_EVENT_AXIS_DISCRETE = 1 << 7,
};

struct pointer_event {
   uint32_t event_mask;
   wl_fixed_t surface_x, surface_y;
   uint32_t button, state;
   uint32_t time;
   uint32_t serial;
   struct {
      bool valid;
      wl_fixed_t value;
      int32_t discrete;
   } axes[2];
   uint32_t axis_source;
};

typedef enum Colors {
   BACKG,
   FOREG,
   BORDER,
   BOX,
} Colors;

static const uint64_t colors[4] = {
    [BACKG] = 0xff0e0e10,
   //  [BACKG] = 0xFFFF0000,
    [FOREG] = 0xffc6c5cf,
    [BORDER] = 0xFFc2c5dc,
    [BOX] = 0xFF1f1f23,
};
static uint64_t colorsGamma[4] = {0};
static const uint8_t log_level = 0;  
#define LOG(imp,...) {if (log_level <= imp) {fprintf(stderr, __VA_ARGS__);}}
#define WLCHECK(x,e) {if(!(x)){LOG(10, "Error running " #x " on " __FILE__ ":" FUNNYCSTRING(__LINE__) ": " e "\n"); exit(1);}}
#define WLCHECKE(x,e) {if(!(x)){LOG(10, "Error running " #x " on " __FILE__ ":" FUNNYCSTRING(__LINE__) ": " e " [%m]\n"); exit(1);}}
#define FTCHECK(x,e) {uint32_t err=x;if(err){LOG(10, "Freetype error: %s %u:[%s]!\n",e,err,FT_Error_String(err)); exit(1);}}
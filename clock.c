#include <SDL3/SDL_timer.h>
#include <stdint.h>
#include "clock.h"

static uint64_t next_cycle_ns;

void Clock_Start(void) {
    next_cycle_ns = SDL_GetTicksNS();
}

void Clock_Update(void) {
    const uint64_t cycle_ns = SDL_NS_PER_SECOND / CPU_HZ;

    next_cycle_ns += cycle_ns;
    uint64_t now = SDL_GetTicksNS();

    if (now < next_cycle_ns) {
        SDL_DelayPrecise(next_cycle_ns - now);
    } else if (now - next_cycle_ns > SDL_NS_PER_SECOND / 10) {
        next_cycle_ns = now;
    }
}
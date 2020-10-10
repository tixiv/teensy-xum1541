/*
 * Name: s2.c
 * Project: xu1541
 * Author: Till Harbaum
 * Tabsize: 4
 * Copyright: (c) 2007 by Till Harbaum <till@harbaum.org>
 * License: GPL
 *
 */

/* This file contains the "serial2" helper functions for opencbm */
/* changes in the protocol must be reflected here. */

#include "xum1541.h"

void
s3_write_byte(uint8_t c)
{
    iec_release(IO_ATN);
    while (iec_get(IO_CLK)); // sync with floppy

    if (c & 0x80)
        iec_set(IO_CLK);
    else
        iec_release(IO_CLK);
    if (c & 0x20)
        iec_set(IO_DATA);
    else
        iec_release(IO_DATA);

    DELAY_US(4);

    if (c & 0x40)
        iec_set(IO_CLK);
    else
        iec_release(IO_CLK);
    if (c & 0x10)
        iec_set(IO_DATA);
    else
        iec_release(IO_DATA);

    DELAY_US(10);

    if (c & 0x08)
        iec_set(IO_CLK);
    else
        iec_release(IO_CLK);
    if (c & 0x02)
        iec_set(IO_DATA);
    else
        iec_release(IO_DATA);

    DELAY_US(7);

    if (c & 0x04)
        iec_set(IO_CLK);
    else
        iec_release(IO_CLK);
    if (c & 0x01)
        iec_set(IO_DATA);
    else
        iec_release(IO_DATA);

    // hold data and wait until 1541 is not signaling ready over clk anymore
    DELAY_US(16);

    // back to normal bus state
    iec_set(IO_ATN);
    iec_release(IO_CLK);
    iec_release(IO_DATA);
}

uint8_t
s3_read_byte(void)
{
    uint8_t c = 0;

    cli();

    iec_release(IO_ATN);
    while (iec_get(IO_CLK)); // sync with floppy

    DELAY_US(9);

    if (iec_get(IO_CLK))
        c |= 0x08;
    if (iec_get(IO_DATA))
        c |= 0x02;

    DELAY_US(10);

    if (iec_get(IO_CLK))
        c |= 0x04;
    if (iec_get(IO_DATA))
        c |= 0x01;

    DELAY_US(10);

    if (iec_get(IO_CLK))
        c |= 0x10;
    if (iec_get(IO_DATA))
        c |= 0x20;

    DELAY_US(8);

    if (iec_get(IO_CLK))
        c |= 0x40;
    if (iec_get(IO_DATA))
        c |= 0x80;

    iec_set(IO_ATN); // back to normal bus state

    sei();

    DELAY_US(4);

    return c;
}

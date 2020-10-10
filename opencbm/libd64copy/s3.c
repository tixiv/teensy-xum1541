/*
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 *
 *  Copyright 1999 Michael Klein <michael(dot)klein(at)puffin(dot)lb(dot)shuttle(dot)de>
 *  Copyright 2008 Spiro Trikaliotis
*/

#include "opencbm.h"
#include "d64copy_int.h"

#include <stdlib.h>
#include <stdio.h>

#include "arch.h"

#include "opencbm-plugin.h"

static opencbm_plugin_s3_read_n_t * opencbm_plugin_s3_read_n = NULL;

static opencbm_plugin_s3_write_n_t * opencbm_plugin_s3_write_n = NULL;

static const unsigned char s3_drive_prog_read[] = {
#include "s3_read.inc"
};

static const unsigned char s3_drive_prog_write[] = {
#include "s3_write.inc"
};

static CBM_FILE fd_cbm;
static int two_sided;

static void s3_error_unsupported_exit()
{
  fprintf(stderr, "s3 transfer is not supported with your plugin!");
  exit(1);
}

/* read_n redirects USB reads to the external reader if required */
static void read_n(unsigned char *data, int size)
{
  if (!opencbm_plugin_s3_read_n)
    s3_error_unsupported_exit();

  opencbm_plugin_s3_read_n(fd_cbm, data, size);
}

/* write_n redirects USB writes to the external reader if required */
static void write_n(const unsigned char *data, int size)
{
  if (!opencbm_plugin_s3_write_n)
    s3_error_unsupported_exit();

  opencbm_plugin_s3_write_n(fd_cbm, data, size);
}

static int read_block(unsigned char tr, unsigned char se, unsigned char *block)
{
    unsigned char status;

    unsigned char buf[2];
    buf[0] = tr;
    buf[1] = se;

    write_n(buf, 2);

#ifndef USE_CBM_IEC_WAIT
    arch_usleep(20000);
#endif

    unsigned char rd_buf[BLOCKSIZE + 1];

    read_n(rd_buf, BLOCKSIZE + 1);
    status = rd_buf[0];

    memcpy (block, rd_buf + 1, BLOCKSIZE);

    return status;
}

static int write_block(unsigned char tr, unsigned char se, const unsigned char *blk, int size, int read_status)
{
  unsigned char buf[size + 3];
  buf[0] = tr;
  buf[1] = se;
  buf[2] = 0x55; // dummy to get length divisible by 4

  memcpy(buf + 3, blk, size);

  unsigned char status;
  write_n(buf, size+3);

  read_n(&status, 1);

  return status;
}

static int open_disk(CBM_FILE fd, d64copy_settings *settings,
                     const void *arg, int for_writing,
                     turbo_start start, d64copy_message_cb message_cb)
{
    unsigned char d = (unsigned char)(ULONG_PTR)arg;

    fd_cbm = fd;
    two_sided = settings->two_sided;

    opencbm_plugin_s3_read_n = cbm_get_plugin_function_address("opencbm_plugin_s3_read_n");
    opencbm_plugin_s3_write_n = cbm_get_plugin_function_address("opencbm_plugin_s3_write_n");

    if(for_writing)
      cbm_upload(fd_cbm, d, TRANSFER_CODE_START, s3_drive_prog_write, sizeof(s3_drive_prog_write));
    else
      cbm_upload(fd_cbm, d, TRANSFER_CODE_START, s3_drive_prog_read, sizeof(s3_drive_prog_read));

    start(fd, d);

    cbm_iec_release(fd_cbm, IEC_CLOCK);
    while(!cbm_iec_get(fd_cbm, IEC_CLOCK));

    cbm_iec_set(fd_cbm, IEC_ATN);
    arch_usleep(20000);
    
    return 0;
}

static void close_disk(void)
{
  unsigned char c = 0;
                                                                        SETSTATEDEBUG((void)0);
    write_n(&c, 1);
                                                                        SETSTATEDEBUG((void)0);
    write_n(&c, 1);
    arch_usleep(100);
                                                                        SETSTATEDEBUG(DebugBitCount=-1);
    cbm_iec_release(fd_cbm, IEC_DATA);
                                                                        SETSTATEDEBUG((void)0);
    cbm_iec_release(fd_cbm, IEC_ATN);
                                                                        SETSTATEDEBUG((void)0);
    cbm_iec_set(fd_cbm, IEC_CLOCK);
                                                                        SETSTATEDEBUG((void)0);

    opencbm_plugin_s3_read_n = NULL;

    opencbm_plugin_s3_write_n = NULL;
}

static int send_track_map(unsigned char tr, const char *trackmap, unsigned char count)
{
    int i;
    int size;
    unsigned char *data;

                                                                        SETSTATEDEBUG((void)0);
    size = d64copy_sector_count(two_sided, tr);
    data = malloc(2+size);

    data[0] = tr;
    data[1] = count;

    /* build track map */
    for(i = 0; i < size; i++)
	data[2+i] = !NEED_SECTOR(trackmap[i]);
    
    write_n(data, size+2);
    free(data);
                                                                        SETSTATEDEBUG((void)0);
    return 0;
}

static int read_gcr_block(unsigned char *se, unsigned char *gcrbuf)
{
    unsigned char buf[2];

    read_n(buf, 2);
    *se = buf[0];

    if(buf[1]) {
        return buf[1];
    }

    read_n(gcrbuf, GCRBUFSIZE);

    return 0;
}

DECLARE_TRANSFER_FUNCS_EX(s3_transfer, 1, 1);

/*
 * Copyright (c) 2020 LAAS/CNRS
 * All rights reserved.
 *
 * Redistribution  and  use  in  source  and binary  forms,  with  or  without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of  source  code must retain the  above copyright
 *      notice and this list of conditions.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice and  this list of  conditions in the  documentation and/or
 *      other materials provided with the distribution.
 *
 * THE SOFTWARE  IS PROVIDED "AS IS"  AND THE AUTHOR  DISCLAIMS ALL WARRANTIES
 * WITH  REGARD   TO  THIS  SOFTWARE  INCLUDING  ALL   IMPLIED  WARRANTIES  OF
 * MERCHANTABILITY AND  FITNESS.  IN NO EVENT  SHALL THE AUTHOR  BE LIABLE FOR
 * ANY  SPECIAL, DIRECT,  INDIRECT, OR  CONSEQUENTIAL DAMAGES  OR  ANY DAMAGES
 * WHATSOEVER  RESULTING FROM  LOSS OF  USE, DATA  OR PROFITS,  WHETHER  IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR  OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *                                             Martin Jacquet - September 2020
 */

#include "accamviz.h"

#include "camviz_c_types.h"

#include "codels.hpp"

/* --- Function disp_start ---------------------------------------------- */

/** Codel display_start of function disp_start.
 *
 * Returns genom_ok.
 */
genom_event
display_start(uint16_t ratio, const camviz_ids_img_size *size,
              const char window[64], char win[64], bool *disp,
              const genom_context self)
{
    strcpy(win, window);
    namedWindow(win, WINDOW_NORMAL);
    resizeWindow(win, round((size->w)/ratio), round((size->h)/ratio));
    *disp = true;

    warnx("start displaying");
    return genom_ok;
}


/* --- Function disp_stop ----------------------------------------------- */

/** Codel display_stop of function disp_stop.
 *
 * Returns genom_ok.
 */
genom_event
display_stop(bool *disp, const genom_context self)
{
    destroyWindow("camviz-genom3");
    *disp = false;

    warnx("stop displaying");
    return genom_ok;
}


/* --- Function rec_start ----------------------------------------------- */

/** Codel record_start of function rec_start.
 *
 * Returns genom_ok.
 */
genom_event
record_start(const char path[64], const camviz_ids_img_size *size,
             camviz_recorder **rec, const genom_context self)
{
    (*rec)->w = VideoWriter(path, CV_FOURCC('M','J','P','G'), 59, Size(size->w,size->h));
    (*rec)->on = true;

    warnx("start recording to %s", path);
    return genom_ok;
}


/* --- Function rec_stop ------------------------------------------------ */

/** Codel record_stop of function rec_stop.
 *
 * Returns genom_ok.
 */
genom_event
record_stop(camviz_recorder **rec, const genom_context self)
{
    (*rec)->w.release();
    (*rec)->on = false;

    warnx("stop recording");
    return genom_ok;
}

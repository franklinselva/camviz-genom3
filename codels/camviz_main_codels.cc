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

/* --- Task main -------------------------------------------------------- */


/** Codel viz_start of task main.
 *
 * Triggered by camviz_start.
 * Yields to camviz_sleep.
 */
genom_event
viz_start(camviz_ids *ids, const genom_context self)
{
    ids->disp = false;
    ids->rec = new camviz_recorder();
    ids->fov = false;

    return camviz_sleep;
}


/** Codel viz_sleep of task main.
 *
 * Triggered by camviz_sleep.
 * Yields to camviz_pause_sleep, camviz_main.
 */
genom_event
viz_sleep(const camviz_frame *frame, camviz_ids_img_size *size,
          const genom_context self)
{
    if (frame->read(self) == genom_ok && frame->data(self) && frame->data(self)->pixels._length > 0)
    {
        *size = {frame->data(self)->width, frame->data(self)->height};
        return camviz_main;
    }
    else
        return camviz_pause_sleep;
}


/** Codel viz_main of task main.
 *
 * Triggered by camviz_main.
 * Yields to camviz_pause_main.
 */
genom_event
viz_main(const camviz_ids_img_size *size, bool fov,
         const camviz_frame *frame, bool disp, const char win[64],
         camviz_recorder **rec, const genom_context self)
{
    if (!disp && !(*rec)->on) return camviz_pause_main; // sleep if no action is required

    frame->read(self);
    or_sensor_frame* fdata = frame->data(self);

    if (size->w != fdata->width || size->h != fdata->height) {
        warnx("Invalid size, skipping frame");
        return camviz_pause_main;
    }

    Mat cvframe = Mat(
        Size(fdata->width, fdata->height),
        (fdata->bpp == 1) ? CV_8UC1 : CV_8UC3,
        fdata->pixels._buffer,
        Mat::AUTO_STEP
    );

    if (fdata->bpp == 3)
        cvtColor(cvframe, cvframe, COLOR_RGB2BGR);  // opencv de ses morts

    if (fov)
    {
        if (fdata->bpp == 1)
            circle(cvframe, Point(fdata->width/2,fdata->height/2), fdata->height/2, Scalar(0), 2);
        else
            circle(cvframe, Point(fdata->width/2,fdata->height/2), fdata->height/2, Scalar(0,0,255), 2);
    }

    if (disp) {
        imshow(win, cvframe);
        waitKey(1);
    }

    if ((*rec)->on)
    {
        if (fdata->bpp == 1) cvtColor(cvframe, cvframe, COLOR_GRAY2BGR);
        (*rec)->w.write(cvframe);
    }
    return camviz_pause_main;
}

/** Codel viz_stop of task main.
 *
 * Triggered by camviz_stop.
 * Yields to camviz_ether.
 */
genom_event
viz_stop(camviz_recorder **rec, bool *disp, const genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return camviz_ether;
}

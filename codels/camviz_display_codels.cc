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

#include <opencv2/opencv.hpp>

using namespace cv;

/* --- Task display ----------------------------------------------------- */


/** Codel viz_start of task display.
 *
 * Triggered by camviz_start.
 * Yields to camviz_wait.
 */
genom_event
viz_start(camviz_ids *ids, const genom_context self)
{
    ids->disp = false;
    ids->fov = false;
    ids->size = {480, 270};
    return camviz_wait;
}


/** Codel viz_wait of task display.
 *
 * Triggered by camviz_wait.
 * Yields to camviz_pause_wait, camviz_disp.
 */
genom_event
viz_wait(bool disp, const camviz_frame *frame,
         const genom_context self)
{
    if (disp && frame->read(self) == genom_ok
        && frame->data(self) && frame->data(self)->pixels._length > 0)
        return camviz_disp;
    else
        return camviz_pause_wait;
}


/** Codel viz_display of task display.
 *
 * Triggered by camviz_disp.
 * Yields to camviz_pause_wait.
 */
genom_event
viz_display(const camviz_ids_img_size *size, bool fov,
            const camviz_frame *frame, const genom_context self)
{
    frame->read(self);
    or_sensor_frame* fdata = frame->data(self);

    int type;
    if (fdata->bpp == 1)
        type = CV_8UC1;
    else
        type = CV_8UC3;

    Mat cvframe = Mat(
        Size(fdata->width, fdata->height),
        type,
        (void*)fdata->pixels._buffer,
        Mat::AUTO_STEP
    );

    if (fdata->bpp == 3)
        cvtColor(cvframe, cvframe, COLOR_RGB2BGR);

    if (fov)
        circle(cvframe, Point(fdata->width/2,fdata->height/2), fdata->height/2, Scalar(0,0,255), 2);

    imshow("camviz-genom3", cvframe);
    waitKey(1);

    return camviz_pause_wait;
}

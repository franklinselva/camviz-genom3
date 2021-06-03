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
#include <unistd.h>

/* --- Task main -------------------------------------------------------- */


/** Codel viz_start of task main.
 *
 * Triggered by camviz_start.
 * Yields to camviz_sleep.
 * Throws camviz_e_sys.
 */
genom_event
viz_start(camviz_ids *ids, const genom_context self)
{
    ids->ratio = 0;
    ids->rec = new camviz_recorder();
    ids->fov = false;
    ids->size = {0,0};
    if (genom_sequence_reserve(&ids->pixel_ports, 0))
        return camviz_e_sys_error("cannot reserve sequence", self);
    ids->pixel_ports._length = 0;

    return camviz_sleep;
}


/** Codel viz_sleep of task main.
 *
 * Triggered by camviz_sleep.
 * Yields to camviz_pause_sleep, camviz_main.
 * Throws camviz_e_sys.
 */
genom_event
viz_sleep(const camviz_frame *frame, camviz_ids_img_size *size,
          const genom_context self)
{
    if (frame->read(self) == genom_ok && frame->data(self) && frame->data(self)->pixels._length > 0)
    {
        *size = {frame->data(self)->width, frame->data(self)->height};
        usleep(1000);
        return camviz_main;
    }
    else
        return camviz_pause_sleep;
}


/** Codel viz_main of task main.
 *
 * Triggered by camviz_main.
 * Yields to camviz_pause_main.
 * Throws camviz_e_sys.
 */
genom_event
viz_main(const sequence_camviz_port_info *pixel_ports, float ratio,
         bool fov, const camviz_frame *frame,
         const camviz_pixel *pixel, const char win[64],
         camviz_recorder **rec, const genom_context self)
{
    if (!ratio && !(*rec)->on) return camviz_pause_main; // sleep if no action is required

    frame->read(self);
    or_sensor_frame* fdata = frame->data(self);

    int type;
    if      (fdata->bpp == 1) type = CV_8UC1;
    else if (fdata->bpp == 2) type = CV_16UC1;
    else if (fdata->bpp == 3) type = CV_8UC3;
    else if (fdata->bpp == 4) type = CV_8UC4;
    else return camviz_e_sys_error("invalid frame type", self);

    Mat cvframe = Mat(
        Size(fdata->width, fdata->height),
        type,
        fdata->pixels._buffer,
        Mat::AUTO_STEP
    );

    if (fdata->bpp == 3)
        cvtColor(cvframe, cvframe, COLOR_RGB2BGR);  // opencv de ses morts

    if (fov)
    {
        if (fdata->bpp < 3)
            circle(cvframe, Point(fdata->width/2,fdata->height/2), fdata->height/2, Scalar(0), 2);
        else
            circle(cvframe, Point(fdata->width/2,fdata->height/2), fdata->height/2, Scalar(0,0,255), 2);
    }

    for (uint16_t i=0; i<pixel_ports->_length; i++)
        if (pixel->read(pixel_ports->_buffer[i], self) == genom_ok && pixel->data(pixel_ports->_buffer[i], self)) {
            uint16_t x = pixel->data(pixel_ports->_buffer[i], self)->x;
            uint16_t y = pixel->data(pixel_ports->_buffer[i], self)->y;
            if (fdata->bpp < 3)
                circle(cvframe, Point(x,y), 2, Scalar(0), 2);
            else
                circle(cvframe, Point(x,y), 2, Scalar(0,0,255), 2);
        }

    if (ratio) {
        if (getWindowProperty(win, WND_PROP_AUTOSIZE) == -1) {
            namedWindow(win, WINDOW_NORMAL);
            resizeWindow(win, round(fdata->width / ratio), round(fdata->height / ratio));
        }
        imshow(win, cvframe);
        waitKey(1);
    }

    if ((*rec)->on)
    {
        if (fdata->bpp == 1) cvtColor(cvframe, cvframe, COLOR_GRAY2BGR);
        try {
            (*rec)->w.write(cvframe);
        }
        catch (cv::Exception& e) {
            warnx("unable to write frame, skipping");
        }
    }
    return camviz_pause_main;
}

/** Codel viz_stop of task main.
 *
 * Triggered by camviz_stop.
 * Yields to camviz_ether.
 * Throws camviz_e_sys.
 */
genom_event
viz_stop(const char win[64], camviz_recorder **rec, float *ratio,
         const genom_context self)
{
    destroyWindow(win);
    *ratio = 0;
    (*rec)->w.release();
    (*rec)->on = false;

    return camviz_ether;
}

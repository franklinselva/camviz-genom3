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
 * Yields to camviz_ether.
 * Throws camviz_e_sys.
 */
genom_event
viz_start(camviz_ids *ids, const genom_context self)
{
    ids->ratio = 0;
    ids->pix_size = 3;
    strcpy(ids->prefix, "\0");

    if (genom_sequence_reserve(&ids->cameras, 0))
        return camviz_e_sys_error("cannot initialize sequence", self);
    ids->cameras._length = 0;

    return camviz_ether;
}


/* --- Activity add_camera ---------------------------------------------- */

/** Codel camera_start of activity add_camera.
 *
 * Triggered by camviz_start.
 * Yields to camviz_main.
 * Throws camviz_e_sys.
 */
genom_event
camera_start(const char port_name[64],
             sequence_camviz_camera_s *cameras, uint16_t *cam_id,
             const genom_context self)
{
    // Add new camera in port list
    for(*cam_id=0; *cam_id<cameras->_length; (*cam_id)++)
        if (!strcmp(cameras->_buffer[*cam_id].name, port_name))
            return camviz_e_sys_error("camera already monitored", self);

    if (*cam_id >= cameras->_maximum)
        if (genom_sequence_reserve(cameras, *cam_id + 1))
            return camviz_e_sys_error("add_camera failed", self);
    (cameras->_length)++;
    strcpy(cameras->_buffer[*cam_id].name, port_name);

    // Init camera fields
    if (genom_sequence_reserve(&cameras->_buffer[*cam_id].pixels, 0))
        return camviz_e_sys_error("cannot initialize sequence", self);
    cameras->_buffer[*cam_id].pixels._length = 0;
    cameras->_buffer[*cam_id].orientation = 0;

    warnx("monitoring camera %s", port_name);
    return camviz_main;
}

/** Codel camera_main of activity add_camera.
 *
 * Triggered by camviz_main.
 * Yields to camviz_pause_main.
 * Throws camviz_e_sys.
 */
genom_event
camera_main(uint16_t cam_id, const char prefix[64], float ratio,
            const camviz_frame *frame, const camviz_pixel *pixel,
            sequence_camviz_camera_s *cameras, uint16_t pix_size,
            const genom_context self)
{
    // Sleep if no action is required
    if (!strcmp(prefix, "\0") && !ratio)
    {
        destroyAllWindows();
        return camviz_pause_main;
    }

    camviz_camera_s* cam = &cameras->_buffer[cam_id];
    if (frame->read(cam->name, self) != genom_ok || !frame->data(cam->name, self) || frame->data(cam->name, self)->pixels._length == 0)
        return camviz_pause_main;

    // Retrieve frame and transform to opencv format
    or_sensor_frame* fdata = frame->data(cam->name, self);

    Mat cvframe;
    if (fdata->compressed)
    {
        std::vector<uint8_t> buf;
        buf.assign(fdata->pixels._buffer, fdata->pixels._buffer + fdata->pixels._length);
        imdecode(buf, IMREAD_UNCHANGED, &cvframe);
    }
    else
    {
        int type;
        if      (fdata->bpp == 1) type = CV_8UC1;
        else if (fdata->bpp == 2) type = CV_16UC1;
        else if (fdata->bpp == 3) type = CV_8UC3;
        else if (fdata->bpp == 4) type = CV_8UC4;
        else return camviz_e_sys_error("invalid frame type", self);

        cvframe = Mat(
            Size(fdata->width, fdata->height),
            type,
            fdata->pixels._buffer,
            Mat::AUTO_STEP
        );
    }

    if (fdata->bpp == 1) cvtColor(cvframe, cvframe, COLOR_GRAY2BGR);
    if (fdata->bpp == 3) cvtColor(cvframe, cvframe, COLOR_RGB2BGR);  // opencv de ses morts

    // Go through pixel ports and display, if any
    for (uint16_t i=0; i<cam->pixels._length; i++)
        if (pixel->read(cam->pixels._buffer[i].name, self) == genom_ok &&
            pixel->data(cam->pixels._buffer[i].name, self) &&
            pixel->data(cam->pixels._buffer[i].name, self)->pix._present)
        {
            uint16_t x = pixel->data(cam->pixels._buffer[i].name, self)->pix._value.x;
            uint16_t y = pixel->data(cam->pixels._buffer[i].name, self)->pix._value.y;
            Scalar color (cam->pixels._buffer[i].color._buffer[2], cam->pixels._buffer[i].color._buffer[1], cam->pixels._buffer[i].color._buffer[0]);
            circle(cvframe, Point(x,y), pix_size, color, -1);
        }

    // Rotate if required
    if (cam->orientation == 1)
        rotate(cvframe, cvframe, ROTATE_90_CLOCKWISE);
    else if (cam->orientation == 2)
        rotate(cvframe, cvframe, ROTATE_180);
    else if (cam->orientation == 3)
        rotate(cvframe, cvframe, ROTATE_90_COUNTERCLOCKWISE);

    // Display if required
    if (ratio)
    {
        if (getWindowProperty(cam->name, WND_PROP_VISIBLE) == -1)
        {
            namedWindow(cam->name, WINDOW_NORMAL);
            resizeWindow(cam->name, round(fdata->width / ratio), round(fdata->height / ratio));
        }
        imshow(cam->name, cvframe);
        waitKey(1);
    }

    // Record if required
    if (strcmp(prefix, "\0"))
    {
        if (!cam->rec)
        {
            cam->rec = new camviz_recorder();
            char path[128];
            strcpy(path, prefix);
            strcat(path, "/");
            strcat(path, cam->name);
            strcat(path, ".avi");
            cam->rec->w = VideoWriter(path, VideoWriter::fourcc('M','J','P','G'), 25, Size(fdata->width, fdata->height));
            warnx("start recording to %s", path);
        }
        try {
            cam->rec->w.write(cvframe);
        }
        catch (Exception& e) {
            warnx("unable to write frame: %s", e.what());
        }
    }
    return camviz_pause_main;
}

/** Codel camera_stop of activity add_camera.
 *
 * Triggered by camviz_stop.
 * Yields to camviz_ether.
 * Throws camviz_e_sys.
 */
genom_event
camera_stop(uint16_t cam_id, sequence_camviz_camera_s *cameras,
            const genom_context self)
{
    destroyWindow(cameras->_buffer[cam_id].name);
    if (cameras->_buffer[cam_id].rec)
        cameras->_buffer[cam_id].rec->w.release();
    cameras->_buffer[cam_id].rec = NULL;

    warnx("stopped monitoring camera %s", cameras->_buffer[cam_id].name);

    strcpy(cameras->_buffer[cam_id].name, "\0");
    cameras->_length--;

    return camviz_ether;
}


/* --- Activity add_pixel_display --------------------------------------- */

/** Codel add_pixel of activity add_pixel_display.
 *
 * Triggered by camviz_start.
 * Yields to camviz_ether.
 * Throws camviz_e_sys.
 */
genom_event
add_pixel(const char pixel_name[64], const char cam_name[64],
          const sequence3_octet *color,
          sequence_camviz_camera_s *cameras, const genom_context self)
{
    // Check that requested camera exists
    bool fail = false;
    uint16_t cam_id;
    retry:
    for (cam_id=0; cam_id<cameras->_length; cam_id++)
        if (!strcmp(cameras->_buffer[cam_id].name, cam_name))
            break;
    if (cam_id == cameras->_length)
    {
        if (!fail)
        {
            fail = true;
            usleep(1000); // sleep 1ms
            goto retry;
        }
        else
            return camviz_e_sys_error("camera not found", self);
    }
    camviz_camera_s* cam = &cameras->_buffer[cam_id];

    // Check that pixel is not already displayed for the camera
    uint16_t pix_id;
    for(pix_id=0; pix_id<cam->pixels._length; pix_id++)
        if (!strcmp(cam->pixels._buffer[pix_id].name, pixel_name))
            return camviz_e_sys_error("pixel already in display", self);
    // Allocate sequence memory if need be
    if (pix_id >= cam->pixels._maximum)
        if (genom_sequence_reserve(&cam->pixels, pix_id + 1))
            return camviz_e_sys_error("add_pixel impossible", self);
    (cam->pixels._length)++;

    // Add pixel to list
    strcpy(cam->pixels._buffer[pix_id].name, pixel_name);
    cam->pixels._buffer[pix_id].color = *color;

    warnx("display new pixel for camera %s: %s", cam_name, pixel_name);

    return camviz_ether;
}

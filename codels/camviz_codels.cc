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

/* --- Attribute show --------------------------------------------------- */

/** Validation codel display_validate of attribute show.
 *
 * Returns genom_ok.
 * Throws camviz_e_sys.
 */
genom_event
display_validate(float ratio, const genom_context self)
{
    if (ratio < 0)
        return camviz_e_sys_error("invalid ratio", self);
    return genom_ok;
}


/* --- Function stop_rec ------------------------------------------------ */

/** Codel stop_rec of function stop_rec.
 *
 * Returns genom_ok.
 */
genom_event
stop_rec(char prefix[64], sequence_camviz_camera_s *cameras,
         const genom_context self)
{
    strcpy(prefix, "\0");

    for (uint16_t cam_id=0; cam_id<cameras->_length; cam_id++)
    {
        if (cameras->_buffer[cam_id].rec)
            cameras->_buffer[cam_id].rec->w.release();
        cameras->_buffer[cam_id].rec = NULL;
    }
    return genom_ok;
}


/* --- Function remove_pixel_display ------------------------------------ */

/** Codel remove_pixel of function remove_pixel_display.
 *
 * Returns genom_ok.
 * Throws camviz_e_sys.
 */
genom_event
remove_pixel(const char pixel_name[64], const char cam_name[64],
             sequence_camviz_camera_s *cameras,
             const genom_context self)
{
    // Check that requested camera exists
    uint16_t cam_id;
    for (cam_id=0; cam_id<cameras->_length; cam_id++)
        if (!strcmp(cameras->_buffer[cam_id].name, cam_name))
            break;
    if (cam_id == cameras->_length)
        return camviz_e_sys_error("camera not found", self);

    camviz_camera_s* cam = &cameras->_buffer[cam_id];

    // Check that pixel is already displayed by this camera
    uint16_t pix_id;
    for(pix_id=0; pix_id<cam->pixels._length; pix_id++)
        if (!strcmp(cam->pixels._buffer[pix_id].name, pixel_name))
            break;
    if (cam_id == cam->pixels._length)
        return camviz_e_sys_error("pixel already in display", self);

    // Remove pixel from list
    for (uint16_t i=pix_id; i<cam->pixels._length-1; i++)
        cam->pixels._buffer[pix_id] = cam->pixels._buffer[pix_id+1];

    (cam->pixels._length)--;

    warnx("remove pixel %s from camera %s", pixel_name, cam_name);

    return genom_ok;
}


/* --- Function set_orientation ----------------------------------------- */

/** Codel set_orientation of function set_orientation.
 *
 * Returns genom_ok.
 * Throws camviz_e_sys.
 */
genom_event
set_orientation(const char cam_name[64], uint16_t orientation,
                sequence_camviz_camera_s *cameras,
                const genom_context self)
{
    // check orientation value
    if (orientation < 0 || orientation > 3) {
        warnx("wrong output frame value (allowed: 0, 1, 2, 3)");
        errno = EDOM;
        return camviz_e_sys_error("wrong value", self);
    }

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

    // Change orientation
    cameras->_buffer[cam_id].orientation = orientation;

    return genom_ok;
}


/* --- Function stop ---------------------------------------------------- */

/** Codel stop of function stop.
 *
 * Returns genom_ok.
 */
genom_event
stop(char prefix[64], float *ratio, const genom_context self)
{
    *ratio = 0;
    strcpy(prefix, "\0");

    return genom_ok;
}

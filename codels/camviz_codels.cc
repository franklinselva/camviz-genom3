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


/* --- Function add_pixel_display --------------------------------------- */

/** Codel add_pixel of function add_pixel_display.
 *
 * Returns genom_ok.
 * Throws camviz_e_sys.
 */
genom_event
add_pixel(const char pixel_name[64], const char cam_name[64],
          sequence_camviz_camera_s *cameras, const genom_context self)
{
    // Check that requested camera exists
    uint16_t cam_id;
    for(cam_id=0; cam_id<cameras->_length; cam_id++)
        if (!strcmp(cameras->_buffer[cam_id].name, cam_name))
            break;
    if (cam_id == cameras->_length)
            return camviz_e_sys_error("camera not found", self);
    camviz_camera_s* cam = &cameras->_buffer[cam_id];

    // Check that pixel is not already displayed for the camera
    uint16_t pix_id;
    for(pix_id=0; pix_id<cam->pixel_ports._length; pix_id++)
        if (!strcmp(cam->pixel_ports._buffer[pix_id], pixel_name))
            return camviz_e_sys_error("pixel already in display", self);
    // Allocate sequence memory if need be
    if (pix_id >= cam->pixel_ports._maximum)
        if (genom_sequence_reserve(&cam->pixel_ports, pix_id + 1))
            return camviz_e_sys_error("add_pixel impossible", self);
    (cam->pixel_ports._length)++;

    // Add pixel to list
    strcpy(cam->pixel_ports._buffer[pix_id], pixel_name);

    warnx("display new pixel for camera %s: %s", cam_name, pixel_name);
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

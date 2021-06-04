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


/* --- Function add_camera ---------------------------------------------- */

/** Codel camera_start of function add_camera.
 *
 * Returns genom_ok.
 */
genom_event
camera_start(const char port_name[64],
             sequence_camviz_camera_s *cameras,
             const genom_context self)
{
    // Add new camera in port list
    uint8_t i;
    for(i=0; i<cameras->_length; i++)
        if (!strcmp(cameras->_buffer[i].name, port_name))
            return camviz_e_sys_error("camera already monitored", self);

    if (i >= cameras->_maximum)
        if (genom_sequence_reserve(cameras, i + 1))
            return camviz_e_sys_error("add_camera failed", self);
    (cameras->_length)++;
    strcpy(cameras->_buffer[i].name, port_name);

    // Init camera fields
    if (genom_sequence_reserve(&cameras->_buffer[i].pixel_ports, 0))
        return camviz_e_sys_error("cannot initialize sequence", self);
    cameras->_buffer[i].pixel_ports._length = 0;

    warnx("monitoring camera %s", port_name);
    return genom_ok;
}


/* --- Function stop ---------------------------------------------------- */

/** Codel stop of function stop.
 *
 * Returns genom_ok.
 */
genom_event
stop(sequence_camviz_camera_s *cameras, char prefix[64], float *ratio,
     const genom_context self)
{
    ratio = 0;
    strcpy(prefix, "\0");

    for (uint8_t i=0; i< cameras->_length; i++)
    {
        destroyWindow(cameras->_buffer[i].name);
        cameras->_buffer[i].rec->w.release();
        cameras->_buffer[i].rec = NULL;
        strcpy(cameras->_buffer[i].name, "\0");
    }

    if (genom_sequence_reserve(cameras, 0))
        return camviz_e_sys_error("cannot reinitialize sequence", self);
    cameras->_length = 0;

    return genom_ok;
}

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


/* --- Attribute set_orientation ---------------------------------------- */

/** Validation codel set_orientation of attribute set_orientation.
 *
 * Returns genom_ok.
 * Throws camviz_e_sys.
 */
genom_event
set_orientation(uint16_t orientation, const genom_context self)
{
    if (orientation < 0 || orientation > 3) {
        warnx("wrong output frame value (allowed: 0, 1, 2, 3)");
        errno = EDOM;
        return camviz_e_sys_error("wrong value", self);
    }
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

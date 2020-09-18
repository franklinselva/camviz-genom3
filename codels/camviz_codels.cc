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


/* --- Function start --------------------------------------------------- */

/** Codel display_start of function start.
 *
 * Returns genom_ok.
 */
genom_event
display_start(const camviz_ids_img_size *size, bool *disp,
              const genom_context self)
{
    namedWindow("camviz-genom3", WINDOW_NORMAL);
    resizeWindow("camviz-genom3", size->w, size->h);
    *disp = true;

    return genom_ok;
}


/* --- Function stop ---------------------------------------------------- */

/** Codel display_stop of function stop.
 *
 * Returns genom_ok.
 */
genom_event
display_stop(bool *disp, const genom_context self)
{
    destroyWindow("camviz-genom3");
    *disp = false;

    return genom_ok;
}

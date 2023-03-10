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
#ifndef H_CAMVIZ_CODELS
#define H_CAMVIZ_CODELS

#include "accamviz.h"

#include "camviz_c_types.h"

#include <err.h>
#include <opencv2/opencv.hpp>

using namespace cv;

struct camviz_recorder {
    VideoWriter w;
};

static inline genom_event
camviz_e_sys_error(const char *s, genom_context self)
{
    camviz_e_sys_detail d;
    char buf[64], *p;
    d.code = errno;

    p = strerror_r(d.code, buf, sizeof(buf));
    snprintf(d.what, sizeof(d.what), "%s%s%s", s ? s : "", s ? ": " : "", p);
    return camviz_e_sys(&d, self);
}

#endif /* H_CAMVIZ_CODELS */

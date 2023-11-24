/*
Copyright (c) 2023 Joe Davisson.

This file is part of Rendera.

Rendera is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Rendera is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Rendera; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

#include <string.h>

#include <FL/fl_draw.H>
#include <FL/Fl_Group.H>

#include "Group.H"

Group::Group(int x, int y, int w, int h, const char *l)
: Fl_Group(x, y, w, h, l)
{
  labelsize(12);
  labelcolor(FL_FOREGROUND_COLOR);
  align(FL_ALIGN_INSIDE | FL_ALIGN_CENTER | FL_ALIGN_TOP);
  resize(x, y, w, h);
}

Group::~Group()
{
}

void Group::draw()
{
  int lw = 0;
  int lh = 0;

  fl_draw_box(FL_UP_FRAME, x(), y(), w(), h(), FL_BACKGROUND_COLOR);

  if (strlen(label()) > 0)
  {
    fl_draw_box(FL_UP_BOX, x(), y(), w(), 22, FL_BACKGROUND2_COLOR);
    measure_label(lw, lh);
    draw_label(x() + (w() - lw) / 2, y() + 4, lw, lh);
    fl_draw_box(FL_UP_FRAME, x(), y() + 22, w(), h() - 22, FL_BACKGROUND_COLOR);
  }

  draw_children();
}


/*
Copyright (c) 2022 Joe Davisson.

This file is part of EasySXB.

EasySXB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

EasySXB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with EasySXB; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

// test comment

#include <FL/fl_draw.H>
#include <FL/Fl_Group.H>

#include "Separator.H"

Separator::Separator(Fl_Group *g, int x, int y, int w, int h, const char *label)
: Fl_Widget(x, y, w, h, label)
{
  group = g;
  resize(group->x() + x, group->y() + y, w, h);
}

Separator::~Separator()
{
}

void Separator::draw()
{
  fl_draw_box(FL_THIN_DOWN_FRAME, x(), y(), w(), h(), FL_BLACK); 
}


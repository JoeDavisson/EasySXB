/*
Copyright (c) 2026 Joe Davisson.

This file is part of JoeClient.

JoeClient is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

JoeClient is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with JoeClient; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

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
  fl_rectf(x(), y() + h() / 2, w(), 2, FL_SELECTION_COLOR); 
}


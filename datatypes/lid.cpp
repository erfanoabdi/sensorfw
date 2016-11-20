/**
   @file lid.cpp
   @brief QObject based datatype for LidData

   <p>
   Copyright (C) 2016 Canonical,  Ltd.

   @author Lorn Potter <lorn.potter@canonical.com>

   This file is part of Sensord.

   Sensord is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   Sensord is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with Sensord.  If not, see <http://www.gnu.org/licenses/>.
   </p>
 */

#include "lid.h"

Lid::Lid(const LidData& lidData)
    : QObject(), data_(lidData.timestamp_, lidData.type_, lidData.value_)
{
}

Lid::Lid(const Lid& lid)
    : QObject(), data_(lid.lidData().timestamp_, lid.lidData().type_, lid.lidData().value_)
{
}

/*
    EIBD eib bus access and management daemon
    Copyright (C) 2015 Matthias Urlichs <matthias@urlichs.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "layer2.h"
#include "layer3.h"

Layer2::Layer2 (L2options *opt, Trace *tr)
{
  l3 = 0;
  t = opt ? opt->t : 0;
  if (! t)
    t = tr;
  mode = BUSMODE_DOWN;
  allow_monitor = !(opt && (opt->flags & FLAG_B_NO_MONITOR));
  if (opt) opt->flags &=~ FLAG_B_NO_MONITOR;
}

bool
Layer2::init (Layer3 *layer3)
{
  l3 = layer3;
  if (! l3)
    return false;
  l3->registerLayer2 (shared_from_this());
  return true;
}

void
Layer2::RunStop()
{
  if (l3) {
    l3->deregisterLayer2 (shared_from_this());
    l3 = 0;
  }
}

bool
Layer2::addAddress (eibaddr_t addr)
{
  unsigned i;
  for (i = 0; i < indaddr (); i++)
    if (indaddr[i] == addr)
      return false;
  indaddr.add (addr);
  return true;
}

bool
Layer2::addReverseAddress (eibaddr_t addr)
{
  if (addr == 0)
    return false;
  unsigned i;
  for (i = 0; i < revaddr (); i++)
    if (revaddr[i] == addr)
      return false;
  revaddr.add (addr);
  return true;
}

bool
Layer2::addGroupAddress (eibaddr_t addr)
{
  unsigned i;
  for (i = 0; i < groupaddr (); i++)
    if (groupaddr[i] == addr)
      return false;
  groupaddr.add (addr);
  return true;
}

bool
Layer2::removeAddress (eibaddr_t addr)
{
  unsigned i;
  for (i = 0; i < indaddr (); i++)
    if (indaddr[i] == addr)
      {
        indaddr.deletepart (i, 1);
        return true;
      }
  return false;
}

bool
Layer2::removeReverseAddress (eibaddr_t addr)
{
  unsigned i;
  for (i = 0; i < revaddr (); i++)
    if (revaddr[i] == addr)
      {
        revaddr.deletepart (i, 1);
        return true;
      }
  return false;
}

bool
Layer2::removeGroupAddress (eibaddr_t addr)
{
  unsigned i;
  for (i = 0; i < groupaddr (); i++)
    if (groupaddr[i] == addr)
      {
        groupaddr.deletepart (i, 1);
        return true;
      }
  return false;
}

bool
Layer2::hasAddress (eibaddr_t addr)
{
  for (unsigned int i = 0; i < indaddr (); i++)
    if (indaddr[i] == addr)
      return true;
  return false;
}

bool
Layer2::hasReverseAddress (eibaddr_t addr)
{
  for (unsigned int i = 0; i < revaddr (); i++)
    if (revaddr[i] == addr)
      return true;
  return false;
}

bool
Layer2::hasGroupAddress (eibaddr_t addr)
{
  for (unsigned int i = 0; i < groupaddr (); i++)
    if (groupaddr[i] == addr || groupaddr[i] == 0)
      return true;
  return false;
}

bool
Layer2::enterBusmonitor ()
{
  if (!allow_monitor)
    return false;
  if (mode != BUSMODE_DOWN)
    return false;
  mode = BUSMODE_MONITOR;
  return true;
}

bool
Layer2::leaveBusmonitor ()
{
  if (mode != BUSMODE_MONITOR)
    return false;
  mode = BUSMODE_DOWN;
  return true;
}

bool
Layer2::Open ()
{
  if (mode != BUSMODE_DOWN)
    return false;
  mode = BUSMODE_UP;
  return true;
}

bool
Layer2::Close ()
{
  if (mode != BUSMODE_UP)
    return false;
  mode = BUSMODE_DOWN;
  return true;
}

bool
Layer2::Send_Queue_Empty ()
{
  return true;
}


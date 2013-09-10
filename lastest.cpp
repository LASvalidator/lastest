/*
===============================================================================

  FILE:  lastest.cpp
  
  CONTENTS:
  
    This tool tests the LASread library

  PROGRAMMERS:
  
    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com
  
  COPYRIGHT:
  
    (c) 2013, martin isenburg, rapidlasso - fast tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING.txt file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    1 April 2013 -- on Easter Monday all-nighting in Perth airport for PER->SYD
  
===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lasreadopener.hpp"
#include "lasutility.hpp"

static void byebye(BOOL error, BOOL wait=FALSE)
{
  if (wait)
  {
    fprintf(stderr,"<press ENTER>\n");
    getc(stdin);
  }
  exit(!error);
}

static void usage(BOOL error, BOOL wait=FALSE)
{
  fprintf(stderr,"Usage:\n");
  fprintf(stderr,"lastest -i lidar.las\n");
  byebye(error, wait);
}

int main(int argc, char *argv[])
{
  int i;

  fprintf(stderr, "This tool tests the LASread library.\n");

  LASreadOpener lasreadopener;

  if (argc == 1)
  {
    fprintf(stderr,"lastest.exe is best run with arguments in the command line\n");
    char file_name[256];
    fprintf(stderr,"enter input LAS file name: "); fgets(file_name, 256, stdin);
    file_name[strlen(file_name)-1] = '\0';
    lasreadopener.set_file_name(file_name);
  }
  else
  {
    if (!lasreadopener.parse(argc, argv))
    {
      fprintf(stderr, "ERROR: parsing for LAS input\n");
      usage(TRUE);
    }
  }

  for (i = 1; i < argc; i++)
  {
    if (argv[i][0] == '\0')
    {
      continue;
    }
    else if (strcmp(argv[i],"-version") == 0)
    {
      fprintf(stderr, "\nlastest with LASread (v %d.%d) built %d by rapidlasso GmbH\n", LASREAD_VERSION_MAJOR, LASREAD_VERSION_MINOR, LASREAD_BUILD_DATE);
      byebye(FALSE);
    }
    else if (strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"-help") == 0)
    {
      usage(FALSE);
    }
    else
    {
      fprintf(stderr, "ERROR: cannot understand argument '%s'\n", argv[i]);
      usage(TRUE);
    }
  }

  // check input

  if (!lasreadopener.is_active())
  {
    fprintf(stderr,"ERROR: no input specified\n");
    byebye(TRUE, argc == 1);
  }

  // open lasreader

  LASreader* lasreader = lasreadopener.open();
  if (lasreader == 0)
  {
    fprintf(stderr, "ERROR: could not open %s\n", lasreadopener.get_file_name());
    byebye(TRUE, argc == 1);
  }    

  // report some stats

  fprintf(stderr, "name: %s\n", lasreadopener.get_file_name());
  fprintf(stderr, "version %d.%d\n", (I32)lasreader->header.version_major, (I32)lasreader->header.version_minor);
  fprintf(stderr, "point_data_format %d\n", (I32)lasreader->header.point_data_format);

  // get error pointer

  LASerror* laserror = &lasreader->header;

  // parse points if no fail

  if (!laserror->fails)
  {
    LASinventory lasinventory;

    while (lasreader->read_point())
    {
      lasinventory.add(&lasreader->point);
    }

    if (lasinventory.is_active())
    {
      fprintf(stderr, "min_gps_time %g\n", lasinventory.min_gps_time);
      fprintf(stderr, "max_gps_time %g\n", lasinventory.max_gps_time);
    }
  }
  
  // report any fails in detail

  if (laserror->fails)
  {
    for (i = 0; i < laserror->fail_num; i+=2)
    {
      fprintf(stderr, "fail '%s' reason: %s\n", laserror->fails[i], laserror->fails[i+1]);
    }
  }

  // report any warnings in detail

  if (laserror->warnings)
  {
    for (i = 0; i < laserror->warning_num; i+=2)
    {
      fprintf(stderr, "warning '%s' reason: %s\n", laserror->warnings[i], laserror->warnings[i+1]);
    }
  }

  lasreader->close();
  delete lasreader;

  byebye(argc==1);

  return 0;
}

/*
===============================================================================

  FILE:  lastest.cpp
  
  CONTENTS:
  
    This source code test compiles the LASread, LAScheck, and CRSscan libraries

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
#include "xmlwriter.hpp"
#include "lascheck.hpp"

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

  fprintf(stderr, "This is a test compile of the LASread, LAScheck, and CRSscan libraries.\n");

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
      fprintf(stderr, "\nlastest built %d with LASread (v %d.%d) and LAScheck (v %d.%d) by rapidlasso\n", LASREAD_BUILD_DATE, LASREAD_VERSION_MAJOR, LASREAD_VERSION_MINOR, LASCHECK_VERSION_MAJOR, LASCHECK_VERSION_MINOR);
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

  // output logging

  XMLwriter xmlwriter;

  // maybe we are doing one summary report

  if (!xmlwriter.open("lastest.xml", "LAStest"))
  {
    fprintf(stderr,"ERROR: cannot open 'lastest.xml'\n");
    byebye(TRUE, argc == 1);
  }

  // create LAScheck 

  LAScheck lascheck;

  // open lasreader

  LASreader* lasreader = lasreadopener.open();
  if (lasreader == 0)
  {
    fprintf(stderr, "ERROR: could not open %s\n", lasreadopener.get_file_name());
    byebye(TRUE, argc == 1);
  }    

  // parse points

  LASinventory lasinventory;

  while (lasreader->read_point())
  {
    lasinventory.add(&lasreader->point);
  }

  // check correctness (without output)

  U32 pass = lascheck.check(&lasreader->header, &lasinventory);

  // start a new report
  
  xmlwriter.begin("report");

  // report description of file

  xmlwriter.beginsub("file");
  xmlwriter.write("name", lasreadopener.get_file_name());
  xmlwriter.write("path", lasreadopener.get_path());
  xmlwriter.write("version_major", lasreader->header.version_major);
  xmlwriter.write("version_minor", lasreader->header.version_minor);
  xmlwriter.write("point_data_format", lasreader->header.point_data_format);
  xmlwriter.write("CRS", "not implemented (yet)");
  xmlwriter.endsub("file");

  // report the verdict

  xmlwriter.beginsub("summary");
  xmlwriter.write((pass == LAS_PASS ? "pass" : ((pass & LAS_FAIL) ? "fail" : "warning")));
  xmlwriter.endsub("summary");

  // report details (if necessary)

  if (pass != LAS_PASS)
  {
    xmlwriter.beginsub("details");
    pass = lascheck.check(&lasreader->header, &lasinventory, &xmlwriter);
    xmlwriter.endsub("details");
  }

  // end the report

  xmlwriter.end("report");

  // close the LAStest XML output file

  xmlwriter.close("LAStest");

  lasreader->close();
  delete lasreader;

  byebye(argc==1);

  return 0;
}

////////////////////////////////////////////////////////
// Name     : libppm.cpp
// Purpose  : Read/Write Portable Pixel Map images
// Author   : Chris M. Christoudias
// Modified by
// Created  : 03/20/2002
// Copyright: (c) Chris M. Christoudias
// Version  : v0.1
////////////////////////////////////////////////////////

#include "libppm.h"
#include <stdio.h>
#include <string.h>
#include <iostream>

int writePPMImage(char *filename, unsigned char *image, int height, int width, int depth, char *comments)
{

  if(!filename || !image) return PPM_NULL_PTR;
  FILE *fp = fopen(filename, "wb");
  if(!fp) return PPM_FILE_ERROR;

  //********************************************************
  //Write header information and comments.
  //********************************************************

  std::string p6 = "P6\n";
  fprintf(fp, p6.c_str(), width, height);
  if(comments && strlen(comments) <= 70) fprintf(fp, "%s\n", comments);
  fprintf(fp, "%d %d\n%d\n", width, height, depth);
  
  //********************************************************
  //Output raw image data.
  //********************************************************

  int writeCount = fwrite(image, sizeof(unsigned char), height*width*3, fp);
  fclose(fp);
  if(writeCount !=height*width*3) return PPM_FILE_ERROR;
  return PPM_NO_ERRORS;
}

int writePGMImage(char *filename, unsigned char *image, int height, int width, int depth, char *comments)
{

  if(!filename || !image) return PPM_NULL_PTR;
  FILE *fp = fopen(filename, "wb");
  if(!fp) return PPM_FILE_ERROR;

  //********************************************************
  //Write header information and comments.
  //********************************************************

  std::string p5 = "P5\n";
  fprintf(fp, p5.c_str(), width, height);
  if(comments && strlen(comments) <= 70) fprintf(fp, "%s\n", comments);
  fprintf(fp, "%d %d\n%d\n", width, height, depth);
  
  //********************************************************
  //Output raw image data.
  //********************************************************

  int writeCount = fwrite(image, sizeof(unsigned char), height*width, fp);
  fclose(fp);
  if(writeCount !=height*width) return PPM_FILE_ERROR;
  return PPM_NO_ERRORS;
}

//write a PNM image
int writePNMImage(char *filename , unsigned char *image, int height, int width, int depth, bool color, 
		  char *comments)
{
  int error;
  if(color) {
    error = writePPMImage(filename, image, height, width, depth, comments);
  } else {
    error = writePGMImage(filename, image, height, width, depth, comments);
  }
  return error;
}

int readPPMImage(char *filename, unsigned char **image, int& height, int& width, int& depth)
{

  if(!filename) return PPM_NULL_PTR;
  FILE *fp = fopen(filename, "rb");
  
  //********************************************************
  //Read header information.
  //********************************************************
  
/*  char buf[70];
  if(!fgets(buf, 70, fp)) return PPM_FILE_ERROR;
  if(strncmp(buf, "P6", 2)) return PPM_UNKNOWN_FORMAT;  
  do {
    if(!fgets(buf, 70, fp)) return PPM_FILE_ERROR;
  } while(buf[0] == '#');
  sscanf(buf, "%d %d", &width, &height);  
  if(!fgets(buf, 70, fp)) return PPM_FILE_ERROR;
  sscanf(buf , "%d", &depth);
  */
  
  //!-Start modification by Sushil Mittal for handling different kinds of formats of .ppm file, 05/05/2009
  char buf[70];
  if(!fscanf(fp, "%s", buf)) return PPM_FILE_ERROR;
  if(strncmp(buf, "P6", 2)) return PPM_UNKNOWN_FORMAT; 
  std::string s = "s";
  if(!fscanf(fp,s.c_str(),buf)) return 7;
  printf("%s\n",buf);
  if(buf[0] == '#')
  {
	  
	  while(buf[0] == '#')
	  {
		  if(!fscanf(fp, "%s", buf)) return PPM_FILE_ERROR;
	  }
  }
  else
  {
	  if(!fscanf(fp, "%s", buf)) return PPM_FILE_ERROR;
  }
  sscanf(buf,"%d",&width);
  if(!fscanf(fp, "%s", buf)) return PPM_FILE_ERROR;  
  sscanf(buf,"%d",&height);
  if(!fscanf(fp, "%s", buf)) return PPM_FILE_ERROR;
  sscanf(buf,"%d",&depth);
  if(!fgets(buf,70,fp)) return 7;
  
//!-End modification by Sushil Mittal for handling different kinds of formats of .ppm file, 05/05/2009
  //********************************************************
  //Read raw data information.
  //********************************************************

  *image = new unsigned char [height * width * 3];
  int readCount = fread(*image, sizeof(unsigned char), height*width*3, fp);
  fclose(fp);
  if(readCount != height*width*3) return PPM_FILE_ERROR;
  return PPM_NO_ERRORS;
}

int readPGMImage(char *filename, unsigned char **image, int& height, int& width, int& depth)
{

  if(!filename) return PPM_NULL_PTR;
  FILE *fp = fopen(filename, "rb");
  
  //********************************************************
  //Read header information.
  //********************************************************
  
  char buf[70];
  if(!fgets(buf, 70, fp)) return PPM_FILE_ERROR;
  if(strncmp(buf, "P5", 2)) return PPM_UNKNOWN_FORMAT;
  if(buf[2] == ' ') { // MatLab
      std::cout << "matlab" << std::endl;
      char buf2[68];
      for(int i = 0; i < 68; i++)
          buf2[i] = buf[i + 2];
      sscanf(buf2, "%d %d %d", &width, &height, &depth);
  }
  else {
    do {
        if(!fgets(buf, 70, fp)) return PPM_FILE_ERROR;
    } while(buf[0] == '#');
    sscanf(buf, "%d %d", &width, &height);  
    if(!fgets(buf, 70, fp)) return PPM_FILE_ERROR;
    sscanf(buf , "%d", &depth);
  }
  std::cout << width << " " << height << " " << depth << " " << std::endl;
  //********************************************************
  //Read raw data information.
  //********************************************************

  *image = new unsigned char [height * width];
  int readCount = fread(*image, sizeof(unsigned char), height*width, fp);
  fclose(fp);
  if(readCount != height*width) return PPM_FILE_ERROR;
  return PPM_NO_ERRORS;
}

//read a PNM image
int readPNMImage(char *filename, unsigned char **image, int& height, int& width, int& depth, bool& color)
{
  color = true; //assume PPM format
  int error = readPPMImage(filename, image, height, width, depth);
  if(error == PPM_UNKNOWN_FORMAT) {
    color = false;
    error = readPGMImage(filename, image, height, width, depth);
  }
  return error;
}



/*
 * Copyright (c) 2019, phm97
 * bmp.h - Created on 23-11-2024
 */

#ifndef BMP_H_INCLUDED
#define BMP_H_INCLUDED

int loadBMP(const char *filename, unsigned char **pixels, int *width, int *height);
int writeBMP(const char *filename, unsigned char *data, int width, int height);

#endif
// -*- Mode: C; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*-
// vim:tabstop=4:shiftwidth=4:expandtab:

/**
 * @file    dpopen.h
 *
 * Header file of a duplex pipe stream.
 *
 * @version 1.3, 2004/07/26
 * @author  Wu Yongwei
 *
 */

#ifndef _DPOPEN_H
#define _DPOPEN_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

FILE* dpopen(const char* command);
int dpclose(FILE* stream);
int dphalfclose(FILE* stream);

#ifdef __cplusplus
}
#endif

#endif /* _DPOPEN_H */

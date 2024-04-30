/*
 * filename:	embmath.c
 * date:	04.29.24
 * author:	Lucas Merritt/merrittlj
 * description:	Implementation for embmath.h.
 */

#include "embmath.h"


int ipow(int base, int exp)
{
    int result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}

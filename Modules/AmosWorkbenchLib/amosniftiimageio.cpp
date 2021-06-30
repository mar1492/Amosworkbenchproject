#include "amosniftiimageio.h"


bool AmosNiftiImageIO::SupportsDimension(unsigned long dim)
{
    return dim > 1 && dim < 5;
}
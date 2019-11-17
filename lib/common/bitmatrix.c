#include <string.h>
#include "bitmatrix.h"

static unsigned short bitmatrix_row_size(unsigned short dimension)
{
    unsigned short row_size = dimension >> 3;

    if ((dimension & 0x07) != 0)
        row_size++;

    return row_size;
}

struct bitmatrix *bitmatrix_create_static(void *buff, const unsigned short row,
        const unsigned short column, const unsigned char reset_bit)
{
    const unsigned char reset_value = reset_bit ? 0xFF : 0;
    const unsigned short row_size = bitmatrix_row_size(column);
    const unsigned short all_size = row_size * row;
    struct bitmatrix *matrix = (struct bitmatrix *)buff;

    matrix->row = row;
    matrix->column = column;
    matrix->row_size = row_size;
    matrix->size = all_size;
    memset(matrix->matrix, reset_value, all_size);

    return matrix;
}

unsigned char bitmatrix_set(struct bitmatrix *matrix, const unsigned short row,
        const unsigned short column, const unsigned char bit)
{
    unsigned char ret;
    unsigned char mask;
    unsigned short offset;

    if (matrix == NULL || row >= matrix->row || column >= matrix->column)
        return 0;

    offset = matrix->row_size * row + (column >> 3);
    mask = 1 << (column & 0x07);
    ret = matrix->matrix[offset] & mask;
    matrix->matrix[offset] &= ~mask;
    if (bit)
        matrix->matrix[offset] |= mask;

    return ret != 0;
}

unsigned char bitmatrix_flip(struct bitmatrix *matrix, const unsigned short row,
        const unsigned short column)
{
    unsigned char ret;
    unsigned char mask;
    unsigned short offset;

    if (matrix == NULL || row >= matrix->row || column >= matrix->column)
        return 0;

    offset = matrix->row_size * row + (column >> 3);
    mask = 1 << (column & 0x07);
    ret = matrix->matrix[offset] & mask;
    matrix->matrix[offset] &= ~mask;
    if (!ret)
        matrix->matrix[offset] |= mask;

    return ret != 0;
}

unsigned char bitmatrix_xor(struct bitmatrix *matrix, const unsigned short row,
        const unsigned short column, const unsigned char bit)
{
    unsigned data;
    unsigned char ret;
    unsigned char mask;
    unsigned short offset;

    if (matrix == NULL || row >= matrix->row || column >= matrix->column)
        return 0;

    offset = matrix->row_size * row + (column >> 3);
    mask = 1 << (column & 0x07);
    ret = matrix->matrix[offset] & mask;
    data = (bit ? mask : 0) ^ ret;
    matrix->matrix[offset] &= ~mask;
    matrix->matrix[offset] |= data;

    return ret != 0;
}

unsigned char bitmatrix_get(const struct bitmatrix *matrix, const unsigned short row,
        const unsigned short column)
{
    unsigned char mask;
    unsigned short offset;

    if (matrix == NULL || row >= matrix->row || column >= matrix->column)
        return 0;

    offset = matrix->row_size * row + (column >> 3);
    mask = 1 << (column & 0x07);
    
    return (matrix->matrix[offset] & mask) != 0;
}

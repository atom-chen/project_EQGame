/*
 *  Spam Module
 */
#include "EQCommPCH.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <Windows.h>
#include <stdlib.h>
// #include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "mem_collector.h"
#include "fkw_seg.h"

#include "spam.h"

fkw_dict_array_t *g_dict_array;

void g_load_dict(const char *dict_file) {
    int hash_size = 1000;
    g_dict_array = fkw_dict_array_init(hash_size);
    g_dict_array = fkw_load_dict(g_dict_array, dict_file);
}

void g_free_dict() {
    fkw_free_dict(&g_dict_array);
}


char *spam_string(const char *input, int input_len, char *output, int output_len)
{
    memcpy(output, input, input_len);
    output[output_len] = '\0';

    fkw_str_t text;
    
    text.len  = output_len;
    text.data = (unsigned char *)output;

    fkw_full_seg(&text, g_dict_array);

    return output;
}
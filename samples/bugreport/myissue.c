/* Minimal test with all headers generated into a single file. */
#include "build/myissue_generated.h"
#include "flatcc/support/hexdump.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    int ret;
    void *buf;
    size_t size;
    flatcc_builder_t builder, *B;

    (void)argc;
    (void)argv;

    B = &builder;
    flatcc_builder_init(B);

    #define BUFFER_COUNT 5
    const int sizes[BUFFER_COUNT] = {
      1,
      16,
      22,
      55,
      123,
    };
    Eclectic_Buffer_ref_t buffer_refs[BUFFER_COUNT];
    for (int i = 0; i < BUFFER_COUNT; ++i) {
      fprintf(stdout, "pre start min_align: %d\n", B->min_align);
      flatcc_builder_start_vector(B, 1, /*align=*/16, FLATBUFFERS_COUNT_MAX(1));
      fprintf(stdout, "post start min_align: %d\n", B->min_align);
      uint8_t* p = flatcc_builder_extend_vector(B, sizes[i]);
      memset(p, 'x', sizes[i]);
      fprintf(stdout, "pre end min_align: %d\n", B->min_align);
      flatcc_builder_ref_t data = flatcc_builder_end_vector(B);
      fprintf(stdout, "post end min_align: %d\n", B->min_align);
      Eclectic_Buffer_start(B);
      Eclectic_Buffer_data_add(B, data);
      buffer_refs[i] = Eclectic_Buffer_end(B);
    }
    flatcc_builder_ref_t buffers_vec = flatcc_builder_create_offset_vector(B, buffer_refs, BUFFER_COUNT);

    Eclectic_FooBar_start_as_root(B);
    Eclectic_FooBar_say_create_str(B, "hello");
    Eclectic_FooBar_meal_add(B, Eclectic_Fruit_Orange);
    Eclectic_FooBar_height_add(B, -8000);
    Eclectic_FooBar_buffers_add(B, buffers_vec);

    fprintf(stdout, "pre root end min_align: %d\n", B->min_align);
    Eclectic_FooBar_end_as_root(B);
    fprintf(stdout, "post root end min_align: %d\n", B->min_align);
    buf = flatcc_builder_get_direct_buffer(B, &size);

    FILE* f = fopen("output.bin", "wb");
    fwrite(buf, 1, size, f);
    fclose(f);

    ret = Eclectic_FooBar_verify_as_root(buf, size);
    if (ret) {
        hexdump("Eclectic.FooBar buffer for myissue", buf, size, stdout);
        printf("could not verify Electic.FooBar table, got %s\n", flatcc_verify_error_string(ret));
    }
    flatcc_builder_clear(B);
    return ret;
}

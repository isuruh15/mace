#include <common.h>

__kernel void space_to_depth(OUT_OF_RANGE_PARAMS
                             GLOBAL_WORK_GROUP_SIZE_DIM3
                             __read_only image2d_t input,
                             __private const int block_size,
                             __private const int input_width,
                             __private const int input_depth_blocks,
                             __private const int output_hb,
                             __private const int output_width,
                             __private const int output_depth_blocks,
                             __write_only image2d_t output) {
  const int d = get_global_id(0);
  const int w = get_global_id(1);
  const int hb = get_global_id(2);

#ifndef NON_UNIFORM_WORK_GROUP
  if (d >= global_size_dim0 || w >= global_size_dim1
      || hb >= global_size_dim2) {
    return;
  }
#endif

  const int in_pos = mad24(d, input_width, w);

  const int out_hb = hb / block_size;
  const int offset_h = hb % block_size;
  const int out_w = w / block_size;
  const int offset_w = w % block_size;
  const int offset_d = (offset_h * block_size + offset_w) * input_depth_blocks;
  const int out_d = d + offset_d;

  if (out_d >= output_depth_blocks || out_hb >= output_hb || out_w >= output_width) {
    return;
  }

  const int out_pos = mad24(out_d, output_width, out_w);
  DATA_TYPE4 in_data = READ_IMAGET(input, SAMPLER, (int2)(in_pos, hb));

  WRITE_IMAGET(output, (int2)(out_pos, out_hb), in_data);
}

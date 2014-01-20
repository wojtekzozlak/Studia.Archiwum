#define MAX_K 30

/*
 * Base, brute kernel.
 */
__kernel void bruteKernel(
    __global float* input_points,
    __global float* input_query,
    __global unsigned int* output_query,
    const unsigned int n,
    const unsigned int d,
    const unsigned int l,
    const unsigned int k)
{
  uint tid = get_global_id(0);
  uint query_offset = tid * d;
  uint i, j;

  __private float distance[MAX_K];
  __private uint label[MAX_K];
  for (i = 0; i < MAX_K; i++) {
    distance[i] = INFINITY;
    label[i] = 99999;
  }


  float dist;
  float f_buf;
  uint lab;
  uint i_buf;
  for (i = 0; i < n; i++) {
    // Compute distance
    uint point_offset = i * (d + 1);
    dist = 0;
    lab = ((__global uint*)input_points)[point_offset + d];

    for (j = 0; j < d; j++) {
      float diff = input_points[point_offset + j] - input_query[query_offset + j];
      dist += diff * diff;
    }
    dist = sqrt(dist);

    // Insert into table
    for (j = 0; j < k; j++) {
      if (dist < distance[j]) {
        f_buf = distance[j];
        i_buf = label[j];
        distance[j] = dist;
        label[j] = lab;
        dist = f_buf;
        lab = i_buf;
      }
    }
  }

  // Find most popular label
  uint max_count = 0;
  uint max_lab = 0;
  for (i = 0; i < k; i++) {
    uint count = 0;
    for (j = 0; j < k; j++) {
      count += label[i] == label[j];
    }
    if ((count > max_count) || (count == max_count && max_lab >= label[i])) {
      max_count = count;
      max_lab = label[i];   
    }
  }

  output_query[tid] = max_lab;
}


/*
 * First upgrade, compile-time constants.
 */
__kernel void withConstants(
    __global float* input_points,
    __global float* input_query,
    __global unsigned int* output_query)
{
  uint tid = get_global_id(0);
  uint query_offset = tid * ARG_D;
  uint i, j;

  __private float distance[ARG_K];
  __private uint label[ARG_K];
  for (i = 0; i < ARG_K; i++) {
    distance[i] = INFINITY;
    label[i] = 99999;
  }


  float dist;
  float f_buf;
  uint lab;
  uint i_buf;
  for (i = 0; i < ARG_N; i++) {
    // Compute distance
    uint point_offset = i * (ARG_D + 1);
    dist = 0;
    lab = ((__global uint*)input_points)[point_offset + ARG_D];

    for (j = 0; j < ARG_D; j++) {
      float diff = input_points[point_offset + j] - input_query[query_offset + j];
      dist += diff * diff;
    }
    dist = sqrt(dist);

    // Insert into table
    for (j = 0; j < ARG_K; j++) {
      if (dist < distance[j]) {
        f_buf = distance[j];
        i_buf = label[j];
        distance[j] = dist;
        label[j] = lab;
        dist = f_buf;
        lab = i_buf;
      }
    }
  }

  // Find most popular label
  uint max_count = 0;
  uint max_lab = 0;
  for (i = 0; i < ARG_K; i++) {
    uint count = 0;
    for (j = 0; j < ARG_K; j++) {
      count += label[i] == label[j];
    }
    if ((count > max_count) || (count == max_count && max_lab >= label[i])) {
      max_count = count;
      max_lab = label[i];   
    }
  }

  output_query[tid] = max_lab;
}



/*
 * Second upgrade, query point in registers.
 */
__kernel void queryInRegisters(
    __global float* input_points,
    __global float* input_query,
    __global unsigned int* output_query)
{
  uint tid = get_global_id(0);
  uint query_offset = tid * ARG_D;
  uint i, j;

  __private float distance[ARG_K];
  __private uint label[ARG_K];
  for (i = 0; i < ARG_K; i++) {
    distance[i] = INFINITY;
    label[i] = 99999;
  }

  __private float query_pt[ARG_D];
  for (i = 0; i < ARG_D; i++) {
    query_pt[i] = input_query[query_offset + i];
  }


  float dist;
  float f_buf;
  uint lab;
  uint i_buf;
  for (i = 0; i < ARG_N; i++) {
    // Compute distance
    uint point_offset = i * (ARG_D + 1);
    dist = 0;
    lab = ((__global uint*)input_points)[point_offset + ARG_D];

    for (j = 0; j < ARG_D; j++) {
      float diff = input_points[point_offset + j] - query_pt[j];
      dist += diff * diff;
    }
    dist = sqrt(dist);

    // Insert into table
    for (j = 0; j < ARG_K; j++) {
      if (dist < distance[j]) {
        f_buf = distance[j];
        i_buf = label[j];
        distance[j] = dist;
        label[j] = lab;
        dist = f_buf;
        lab = i_buf;
      }
    }
  }

  // Find most popular label
  uint max_count = 0;
  uint max_lab = 0;
  for (i = 0; i < ARG_K; i++) {
    uint count = 0;
    for (j = 0; j < ARG_K; j++) {
      count += label[i] == label[j];
    }
    if ((count > max_count) || (count == max_count && max_lab >= label[i])) {
      max_count = count;
      max_lab = label[i];   
    }
  }

  output_query[tid] = max_lab;
}



#define SWAP(a, b, buff) { buff = a; a = b; b = buff; }

/*
 * Using heap for updates.
 */
__kernel void heap(
    __global float* input_points,
    __global float* input_query,
    __global unsigned int* output_query)
{
  uint tid = get_global_id(0);
  uint query_offset = tid * ARG_D;
  uint i, j;

  __private float distance[ARG_K];
  __private uint label[ARG_K];
  for (i = 0; i < ARG_K; i++) {
    distance[i] = INFINITY;
    label[i] = 99999;
  }

  __private float query_pt[ARG_D];
  for (i = 0; i < ARG_D; i++) {
    query_pt[i] = input_query[query_offset + i];
  }


  float dist;
  float f_buf;
  uint lab;
  uint i_buf;
  for (i = 0; i < ARG_N; i++) {
    // Compute distance
    uint point_offset = i * (ARG_D + 1);
    dist = 0;
    lab = ((__global uint*)input_points)[point_offset + ARG_D];

    for (j = 0; j < ARG_D; j++) {
      float diff = input_points[point_offset + j] - query_pt[j];
      dist += diff * diff;
    }
    dist = sqrt(dist);


    // Maybe insert to heap?
    if (dist < distance[0]) {
      distance[0] = dist;
      label[0] = lab;

      for (j = 0;;) {
        float left = (2 * j < ARG_K ? distance[2 * j] : -INFINITY);
        float right = (2 * j + 1 < ARG_K ? distance[2 * j + 1] : -INFINITY);
        uint branch = left >= right ? 2 * j : 2 * j + 1;

        if (branch < ARG_K && distance[branch] > distance[j]) {
          SWAP(distance[branch], distance[j], f_buf);
          SWAP(label[branch], label[j], i_buf);
          j = branch;
        } else break;
      }
    }
  }

  // Find most popular label
  uint max_count = 0;
  uint max_lab = 0;
  for (i = 0; i < ARG_K; i++) {
    uint count = 0;
    for (j = 0; j < ARG_K; j++) {
      count += label[i] == label[j];
    }
    if ((count > max_count) || (count == max_count && max_lab >= label[i])) {
      max_count = count;
      max_lab = label[i];   
    }
  }

  output_query[tid] = max_lab;
}

#include <omp.h>
#include <x86intrin.h>

#include "compute.h"

// Computes the dot product of vec1 and vec2, both of size n
int dot(uint32_t n, int32_t *vec1, int32_t *vec2) {
  
  int32_t result = 0;

  if (n >= 8) {
    __m256i const perm_mask = _mm256_setr_epi32(7,6,5,4,3,2,1,0);

    __m256i sum = _mm256_setzero_si256();

    for (unsigned int i = 0; i < n/8*8; i+=8) {
      __m256i a = _mm256_loadu_si256((__m256i *) (vec1 + i));

      // load ith last 8 elements of mat_B.
      __m256i b = _mm256_loadu_si256((__m256i *) (vec2 + (n - (i + 8))));

      //reverse the order of b
      __m256i reverse_b = _mm256_permutevar8x32_epi32(b, perm_mask);

      __m256i tmp = _mm256_mullo_epi32(a, reverse_b);

      sum = _mm256_add_epi32(sum, tmp);
    }

    int32_t tmp_arr[8];
    _mm256_storeu_si256((__m256i *) tmp_arr, sum);

    result += tmp_arr[0] + tmp_arr[1] + tmp_arr[2] + tmp_arr[3] + tmp_arr[4] + tmp_arr[5] + tmp_arr[6] + tmp_arr[7];
  }

  //tail case.
  for (int i = n/8 * 8; i < n; i++) {
    result += vec1[i] * vec2[n-1 -i];
  }

  return result;
}

// Computes the convolution of two matrices
int convolve(matrix_t *a_matrix, matrix_t *b_matrix, matrix_t **output_matrix) {

  
  // TODO: convolve matrix a and matrix b, and store the resulting matrix in
  // output_matrix

  // Assume 1D array with mat_a_num_cols > mat_b_num_cols
  int mat_a_num_cols = a_matrix->cols;
  int mat_b_num_cols = b_matrix->cols;

  int mat_a_num_rows = a_matrix->rows;
  int mat_b_num_rows = b_matrix->rows;

  int output_row = mat_a_num_rows - mat_b_num_rows + 1;
  int output_col = mat_a_num_cols - mat_b_num_cols + 1;

  *output_matrix = (matrix_t *) calloc(1, sizeof(matrix_t));

  (*output_matrix)->rows = output_row;
  (*output_matrix)->cols = output_col;
  (*output_matrix)->data = calloc(output_col * output_row, sizeof(int32_t));

  #pragma omp parallel for collapse(2)
  for (unsigned int row_ct = 0; row_ct < output_row; row_ct++) {
    for (unsigned int col_ct = 0; col_ct < output_col; col_ct++) {
      
      int result = 0;

      unsigned int a_output_offset = (mat_a_num_cols * row_ct) + col_ct;

      #pragma for reduction(+ : result)
      for (unsigned int r = 0; r < mat_b_num_rows; r++) {
        
        // unsigned int index_a = a_output_offset + (r * mat_a_num_cols);
        // unsigned int index_b = (mat_b_num_rows -1 -r) * mat_b_num_cols;
        result += dot(mat_b_num_cols, a_matrix->data + (a_output_offset + (r * mat_a_num_cols)), b_matrix->data + ((mat_b_num_rows -1 -r) * mat_b_num_cols));
      }

       //unsigned int output_index = (row_ct * output_col) + col_ct;
      (*output_matrix)->data[(row_ct * output_col) + col_ct] = result;
    } 
  }

  return 0;
}

// Executes a task
int execute_task(task_t *task) {
  matrix_t *a_matrix, *b_matrix, *output_matrix;

  if (read_matrix(get_a_matrix_path(task), &a_matrix))
    return -1;
  if (read_matrix(get_b_matrix_path(task), &b_matrix))
    return -1;

  if (convolve(a_matrix, b_matrix, &output_matrix))
    return -1;

  if (write_matrix(get_output_matrix_path(task), output_matrix))
    return -1;

  free(a_matrix->data);
  free(b_matrix->data);
  free(output_matrix->data);
  free(a_matrix);
  free(b_matrix);
  free(output_matrix);
  return 0;
}

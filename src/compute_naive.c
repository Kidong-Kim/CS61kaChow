#include "compute.h"

// Computes the dot product of vec1 and vec2, both of size n
int dot(uint32_t n, int32_t *vec1, int32_t *vec2) {
  // TODO: implement dot product of vec1 and vec2, both of size n

  int result = 0;
  for(int i = 0; i < n; i++) {
    result +=  vec1[i] * vec2[i];
  }

  return result;
}

// Computes the convolution of two matrices
int convolve(matrix_t *a_matrix, matrix_t *b_matrix, matrix_t **output_matrix) {
  // TODO: convolve matrix a and matrix b, and store the resulting matrix in
  // output_matrix

  // int i = sleep(1);

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

  for (int row_ct = 0; row_ct < output_row; row_ct++) {
    for (int col_ct = 0; col_ct < output_col; col_ct++) {
      
      int result = 0;

      int a_output_offset = (mat_a_num_cols * row_ct) + col_ct;

      for (int r = 0; r < mat_b_num_rows; r++) {
        for (int c = 0; c < mat_b_num_cols; c++) {
          int index_b = (mat_b_num_rows -r) * mat_b_num_cols -c -1;
          int index_a = a_output_offset + (r * mat_a_num_cols) + c;
          result += a_matrix->data[index_a] * b_matrix->data[index_b];
        }
      }

      int output_index = (row_ct * output_col) + col_ct;

      (*output_matrix)->data[output_index] = result;
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

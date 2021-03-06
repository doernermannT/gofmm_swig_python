/*
 * test_gkmx.c
 *
 * Chenhan D. Yu
 *
 * Department of Computer Science, University of Texas at Austin
 *
 * Purpose: 
 * this is the main function to exam the correctness between dgemm_tn()
 * and dgemm() from the BLAS library.
 *
 * Todo:
 *
 * Modification:
 *
 * */

#include <tuple>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <hmlp.h>
#include <hmlp_blas_lapack.h>

#ifdef HMLP_MIC_AVX512
#include <hbwmalloc.h>
#endif

#define GFLOPS 1073741824 
#define TOLERANCE 1E-13


void nbody
(
	int m, int n, int k,
	float  *A, int lda,
  float  *B, int ldb,
  float  *C, int ldc
);


void nbody
(
	int m, int n, int k,
	double *A, int lda,
  double *B, int ldb,
  double *C, int ldc
);






template<typename T>
void compute_error
(
  int m, int n,
  T *test, int ldtest,
  T *goal, int ldgoal
)
{
  std::tuple<int, int, T> max_err( -1, -1, (T)0.0 );
  T abs_err = 0.0, rel_err = 0.0, nrm2 = 0.0;

  for ( auto j = 0; j < n; j ++ ) 
  {
    for ( auto i = 0; i < m; i ++ ) 
    {
      auto tmp_goal = goal[ j * ldgoal + i ];
      auto tmp_test = test[ j * ldtest + i ];
      auto err = fabs( tmp_test - tmp_goal );
      if ( err > std::get<2>( max_err ) ) 
      {
        max_err = std::make_tuple( i, j, err );
      }
      rel_err += err * err;
      nrm2    += tmp_goal * tmp_goal;
    }
  }

  abs_err = sqrt( rel_err );
  rel_err /= nrm2;
  rel_err = sqrt( rel_err );

  if ( rel_err > TOLERANCE ) 
  {
	  printf( "rel error %E, abs error %E, max error %E at (%d %d)\n", 
		  rel_err, abs_err, std::get<2>( max_err ), 
      std::get<0>( max_err ), std::get<1>( max_err ) );
  }
}


/* 
 * --------------------------------------------------------------------------
 * @brief  This is the test routine to exam the correctness of GSKS. XA and
 *         XB are d leading coordinate tables, and u, w have to be rhs
 *         leading. In this case, dgsks() doesn't need to know the size of
 *         nxa and nxb as long as those index map--amap, bmap, umap and wmap
 *         --are within the legal range.
 *
 * @param  *kernel gsks data structure
 * @param  m       Number of target points
 * @param  n       Number of source points
 * @param  k       Data point dimension
 * --------------------------------------------------------------------------
 */
template<typename TLOWPRECISION, typename T>
void test_nbody( int m, int n, int k ) 
{
  T *A, *B, *C, *C_ref;
  TLOWPRECISION *A_f, *B_f, *C_f, *C_fref;
  double ref_beg, ref_time, gkmx_beg, gkmx_time;
  double flops = ( (double)( m * n ) / GFLOPS ) * ( 2.0 * k + 0.0 );

  int n_iter = 3;


  // ------------------------------------------------------------------------
  // Memory allocation for all common buffers
  // ------------------------------------------------------------------------
#ifdef HMLP_MIC_AVX512
  A_f   = (TLOWPRECISION*)hbw_malloc( sizeof(TLOWPRECISION) * m * k );
  B_f   = (TLOWPRECISION*)hbw_malloc( sizeof(TLOWPRECISION) * k * n );
  C_f   = (TLOWPRECISION*)hbw_malloc( sizeof(TLOWPRECISION) * m * n );
  A     = (T*)hbw_malloc( sizeof(T) * m * k );
  B     = (T*)hbw_malloc( sizeof(T) * k * n );
  C     = (T*)hbw_malloc( sizeof(T) * m * n );
  C_ref = (T*)hbw_malloc( sizeof(T) * m * n );
#else
  A_f   = (TLOWPRECISION*)malloc( sizeof(TLOWPRECISION) * m * k );
  B_f   = (TLOWPRECISION*)malloc( sizeof(TLOWPRECISION) * k * n );
  C_f   = (TLOWPRECISION*)malloc( sizeof(TLOWPRECISION) * m * n );
  A     = (T*)malloc( sizeof(T) * m * k );
  B     = (T*)malloc( sizeof(T) * k * n );
  //C     = (T*)malloc( sizeof(T) * m * n );
  //C_ref = (T*)malloc( sizeof(T) * m * n );
  posix_memalign( (void**)&C,      32, sizeof(T) * m * n );
  posix_memalign( (void**)&C_ref,  32, sizeof(T) * m * n );
  posix_memalign( (void**)&C_fref, 32, sizeof(TLOWPRECISION) * m * n );
#endif
  // ------------------------------------------------------------------------

  // ------------------------------------------------------------------------
  // Initialization
  // ------------------------------------------------------------------------
  #pragma omp parallel for
  for ( auto i = 0; i < m; i ++ ) 
    for ( auto p = 0; p < k; p ++ ) 
      A_f[ i * k + p ] = (T)( rand() % 100 ) / 1000.0;
      //A[ i * k + p ] = (T)( rand() % 100 ) / 1000.0;
  for ( auto j = 0; j < n; j ++ ) 
    for ( auto p = 0; p < k; p ++ ) 
      B_f[ j * k + p ] = (T)( rand() % 100 ) / 1000.0;	
      //B[ j * k + p ] = (T)( rand() % 100 ) / 1000.0;	
  
  

  double beg = omp_get_wtime();
  #pragma omp parallel for
  for ( auto i = 0; i < m; i ++ ) 
  {
    for ( auto p = 0; p < k; p ++ ) 
    {
      A[ i * k + p ] = A_f[ i * k + p ];
      //A_f[ i * k + p ] = A[ i * k + p ];
    }
  }
  #pragma omp parallel for
  for ( auto j = 0; j < n; j ++ ) 
  {
    for ( auto p = 0; p < k; p ++ ) 
    {
      B[ j * k + p ] = B_f[ j * k + p ];
      //B_f[ j * k + p ] = B[ j * k + p ];
    }
  }
  double conversion_t = omp_get_wtime() - beg;
  // ------------------------------------------------------------------------


  // ------------------------------------------------------------------------
  // Call my implementation (NN)
  // ------------------------------------------------------------------------
  for ( auto iter = -1; iter < n_iter; iter ++ ) 
  {
    if ( iter == 0 ) gkmx_beg = omp_get_wtime();
    nbody
    (
      m, n, k,
      A_f, m,
      B_f, k,
      C_f, m 
    );
    //gnbx
    //(
    //  m, n, k,
    //  A, m,
    //  B, k,
    //  C, m 
    //);
  }
  gkmx_time = omp_get_wtime() - gkmx_beg;
  // ------------------------------------------------------------------------


  beg = omp_get_wtime();
  #pragma omp parallel for
  for ( size_t i = 0; i < m * n; i ++ ) C[ i ] = C_f[ i ];
  //for ( size_t i = 0; i < m * n; i ++ ) C_f[ i ] = C[ i ];
  conversion_t += omp_get_wtime() - beg;


  // ------------------------------------------------------------------------
  // Call the reference function (NN)
  // ------------------------------------------------------------------------
  for ( auto iter = -1; iter < n_iter; iter ++ ) 
  {
    if ( iter == 0 ) ref_beg = omp_get_wtime();
    hmlp::xgemm
    ( 
      "N", "N", 
      m, n, k, 
      1.0, A,     m, 
           B,     k, 
      0.0, C_ref, m 
    );
    //hmlp::xgemm
    //( 
    //  "N", "N", 
    //  m, n, k, 
    //  1.0, A_f,    m, 
    //       B_f,    k, 
    //  0.0, C_fref, m 
    //);
  }
  ref_time = omp_get_wtime() - ref_beg;
  // ------------------------------------------------------------------------


  beg = omp_get_wtime();
  #pragma omp parallel for
  //for ( size_t i = 0; i < m * n; i ++ ) C_ref[ i ] = C_fref[ i ];
  for ( size_t i = 0; i < m * n; i ++ ) C_f[ i ] = C[ i ];
  conversion_t += omp_get_wtime() - beg;



  ref_time  /= n_iter; 
  //ref_time += conversion_t;
  gkmx_time /= n_iter;

  compute_error( m, n, C, m, C_ref, m );
  //compute_error( m, n, C_f, m, C_fref, m );

  printf( "NN %5d, %5d, %5d, %5.2lf, %5.2lf;\n", 
      m, n, k, flops / gkmx_time, flops / ref_time );


#ifdef HMLP_MIC_AVX512
  hbw_free( A );
  hbw_free( B );
  hbw_free( C );
  hbw_free( C_ref );
#else
  free( A );
  free( B );
  free( C );
  free( C_ref );
#endif

};


int main( int argc, char *argv[] )
{
  int m, n, k;

  sscanf( argv[ 1 ], "%d", &m );
  sscanf( argv[ 2 ], "%d", &n );
  sscanf( argv[ 3 ], "%d", &k );


  //test_nbody<float, double>( m, n, k );
  test_nbody<double, double>( m, n, k );

  return 0;
};

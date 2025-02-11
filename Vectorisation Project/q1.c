#include <stdio.h> //this library is needed for printf function
#include <stdlib.h> //this library is needed for rand() function
#include <windows.h> //this library is needed for pause() function
#include <time.h> //this library is needed for clock() function
#include <math.h> //this library is needed for abs()
#include <immintrin.h> 
#include <omp.h>

void init();
void q1();
void q1_vec_j();
void q1_vec_k();
int check_correctness();

//#define N 32
//#define N 64
//#define N 128
#define N 256 //input size
//#define N 512
//#define N 1024

// defining arrays
float A[N][N], B[N][N], C[N][N], vec_c[N][N], new_B[N][N];


int main() {
	// initialise question d variables
	int i = 0;
	float total_time = 0.0f, average_time = 0.0f, flops = 0.0f;

	//define the timers measuring execution time
	double start_1, end_1; //ignore this for  now

	init();
	q1();
	//q1_vec_k();
	q1_vec_j();
	// outputs incorrect or correct depending on comparison of arrays from check_correctness
	if (check_correctness() == 0) {
		printf("correct");
	}
	else {
		printf("incorrect");
	}

	// loops through question 1 and displays appropiate outputs
	while (total_time < 2.5) {
		init();//initialize the arrays

		start_1 = omp_get_wtime(); //start the timer 
		q1(); //main routine
		end_1 = omp_get_wtime(); //end the timer
		i++;

		total_time += (end_1 - start_1);
	}
	
	average_time = total_time / i;
	printf("\naverage time : %f", average_time);
	flops = (2.0f * pow(N, 3)) / average_time;
	printf("\nflops: %f\n", flops);
	printf("\ntotal time: %f", total_time);
	//printf(" clock() method: %ldms\n", (end_1 - start_1) );//print the ex.time


	system("pause"); //this command does not let the output window to close

	return 0; //normally, by returning zero, we mean that the program ended successfully. 
}

void init() {

	float e = 0.1234f, p = 0.7264f;//if you do not specify the 'f' after 0.0, then double precision data type is assumed (not float which single precision). 

	//MVM
	for (unsigned int i = 0; i < N; i++)
		for (unsigned int j = 0; j < N; j++) {
			A[i][j] = ((i - j) % 9) + p;
			B[i][j] = ((i + j) % 11) + e;
			new_B[j][i] = ((i + j) % 11) + e;
			C[i][j] = 0.0f;
			vec_c[i][j] = 0.0f;
		}

}



void q1() {

	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			for (int k = 0; k < N; k++)
				C[i][j] += A[i][k] * B[k][j];

}

void q1_vec_j() {
	// initialised looping variables to access them outside of loop
	int i, j, k;
	for (i = 0; i < N; i++) {
		for (j = 0; j < (N / 8 * 8); j += 8) {
			__m256 vectorised_c = _mm256_loadu_ps(&vec_c[i][j]);// loads 8 consecutive elements from the array vec_c --> vec[i][j], vec[i][j+1], vec[i][j+2], vec[i][j+3], vec[i][j+4], vec[i][j+5], vec[i][j+6], vec[i][j+7]
			for (k = 0; k<N; k ++) {
				__m256 vectorised_a = _mm256_set1_ps(A[i][k]);// broadcasts single value to all elements
				__m256 vectorised_b = _mm256_loadu_ps(&B[k][j]);// load 8 consecutive elements from the array B
				vectorised_c = _mm256_fmadd_ps(vectorised_a, vectorised_b, vectorised_c);// performs A[i][k] * B[k][j] + vec_c[i][j]
				_mm256_storeu_ps(&vec_c[i][j], vectorised_c);// stores the 8 updated elementsback into vec_c array

			}
		}
		// this handles the remaining elements where the total elements remaining isnt a multiple of 8 so avx 256 vectorisation is impossible
		// this solves the remaining elements in a scalar manner
		for (; j < N; j++) {
			for (k = 0; k < N; k++) {
				vec_c[i][j] += A[i][k] * B[k][j];
			}	
		}
	}
}

void q1_vec_k() {
	int i, j, k;
	__m256 vectorised_c, vectorised_b, vectorised_a, sum_1, sum_2;

	// if N is less than 8, then the vectorisation is not possible
	if (N < 8) {
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				for (int k = 0; k < N; k++)
					vec_c[i][j] += A[i][k] * B[k][j];
	}
	// if N is greater than 8, then vectorisation is possible
	else {
		for (i = 0; i < N; i++) {
			for (j = 0; j < N - 8; j++) {
				vectorised_c = _mm256_setzero_ps();// clears array by setting elements to 0
				for (k = 0; k < (N / 8 * 8); k += 8) {
					//C[i][j] += A[i][k] * B[k][j];
					vectorised_b = _mm256_loadu_ps(&new_B[k][j]);// loads 8 consecutive elements from the array new_B
					vectorised_a = _mm256_loadu_ps(&A[i][k]);// loads 8 consecutive elements from the array A
					vectorised_c = _mm256_fmadd_ps(vectorised_a, vectorised_b, vectorised_c);// vectorised_c = (vectorised_a * vectorised_b) + vectorised_c
				}
				sum_1 = _mm256_hadd_ps(vectorised_c, vectorised_c);// adds horizontal pairs of elements 
				sum_2 = _mm256_hadd_ps(sum_1, sum_1);// adds horizontal pairs of elements 
				float vectorised_result[8];
				_mm256_storeu_ps(vectorised_result, sum_2);// stores the 8 updated elements back into vec_c array
				float final_sum = vectorised_result[0] + vectorised_result[4];
				vec_c[i][j] = final_sum;

				// processess left over elements in the k-dimension
				for (k = (N / 8 * 8); k < N; k++) {
					vec_c[i][j] += A[i][k] * B[k][j];
				}
			}
			// processess left over elements in j-dimension
			__m256 vectorised_c = _mm256_loadu_ps(&vec_c[i][j]); // loads 8 consecutive elements from the array vec_c
			for (k = 0; k < N; k++) {
				__m256 vectorised_a = _mm256_set1_ps(A[i][k]);// broadcasts single value to all elements
				__m256 vectorised_b = _mm256_loadu_ps(&B[k][j]); // loads 8 consecutive elements from the array B
				vectorised_c = _mm256_fmadd_ps(vectorised_a, vectorised_b, vectorised_c);// performs A[i][k] * B[k][j] + vec_c[i][j]
			}
			_mm256_storeu_ps(&vec_c[i][j], vectorised_c); // stores the 8 updated elements back into vec_c array
		}
	}
	
}

int check_correctness() {
	int i, j;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			if (C[i][j] > vec_c[i][j]) {
				if (fabs(C[i][j] - vec_c[i][j]) / vec_c[i][j] > 0.0001f) {
					// not equal
					return 1;

				}
			}
			else {
				if (fabs(vec_c[i][j] - C[i][j]) / C[i][j] > 0.0001f) {
					// not equal
					return 1;
				}

			}
			
		}
		
	}
	// equal
	return 0;
}


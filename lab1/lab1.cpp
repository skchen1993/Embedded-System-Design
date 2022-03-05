#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#define is_even_macro(n) ( n%2==0 ? 1:0)

pthread_mutex_t mutex;
long long int num_in_circle;

void test_int(){
    // integer sum
    struct  timeval start,end;
    unsigned  long diff;
    gettimeofday(&start,NULL);
    int sum = 0;
    for(int i = 0;i < 2147483647;i++){
        sum += 1;
    }
    printf("%d\n",sum);
    gettimeofday(&end,NULL);
    diff = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
    printf("Total spent: %ld us, %lf secs\n",diff, diff/1000000.0);
}
void test_int2(){
    // integer sum
    struct  timeval start,end;
    unsigned  long diff;
    gettimeofday(&start,NULL);
    int sum = 0;
    for(int i = 0;i < 2147483;i++){
        for(int j = 0;j < 1000;j++){
            sum += 1;
        }
    }
    printf("%d\n",sum);
    gettimeofday(&end,NULL);
    diff = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
    printf("Total spent: %ld us, %lf secs\n",diff, diff/1000000.0);
}
void test_float(){
    // floating point sum
    struct  timeval start,end;
    unsigned  long diff;
    gettimeofday(&start,NULL);
    double sum = 0.0;
    for(int i = 0;i < 2147483647;i++){
        sum += 1.0;
    }
    printf("%lf\n",sum);
    gettimeofday(&end,NULL);
    diff = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
    printf("Total spent: %ld us, %lf secs\n",diff, diff/1000000.0);
}
void test_float2(){
    // floating point sum
    struct  timeval start,end;
    unsigned  long diff;
    gettimeofday(&start,NULL);
    double sum = 0.0;
    for(int i = 0;i < 2147483;i++){
        for(int j = 0;j < 1000;j++){
            sum += 1.0;
        }
    }
    printf("%lf\n",sum);
    gettimeofday(&end,NULL);
    diff = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
    printf("Total spent: %ld us, %lf secs\n",diff, diff/1000000.0);
}
void *sub_job(void *num){

    long long int num_of_tosses = *(long long int*) num;
    long long int sum = 0;
    unsigned int seed = clock();
    for(int toss = 0; toss < num_of_tosses; toss++ ){
        double x = ((double)rand_r(&seed) / (double)RAND_MAX )*2.0 - 1.0;
        double y = ((double)rand_r(&seed) / (double)RAND_MAX )*2.0 - 1.0;
        if(x * x + y * y <= 1)
            sum++;
    }
    //cout << sum << endl;
    pthread_mutex_lock(&mutex); 
    num_in_circle += sum;
    pthread_mutex_unlock(&mutex); 

    pthread_exit(NULL);
}
void pi_sim(long long int num_of_tosses, int CPU_cores){
    struct  timeval start,end;
    unsigned  long diff;
    gettimeofday(&start,NULL);

    // initialize
    num_in_circle = 0;

    // create threads
    pthread_t *thrds_ary;
    thrds_ary = (pthread_t *)malloc(CPU_cores*sizeof(pthread_t));

    // create mutex
    pthread_mutex_init(&mutex, NULL);

    // determine job number to each thread
    long long int num_chunk = num_of_tosses / CPU_cores;

    // threads do their jobs
    for(int i = 0;i < CPU_cores;i++){
        if(pthread_create(&thrds_ary[i], NULL, sub_job, (void*)&num_chunk) != 0){	
            printf("pthred create error\n");
        }
    }
    // join threads
    for(int i = 0;i < CPU_cores;i++){
        pthread_join(thrds_ary[i],NULL);
    }
    double pi_estimate = 4.0 * (double)(num_in_circle)/((double)num_of_tosses);
    printf("pi = %lf\n",pi_estimate);

    pthread_mutex_destroy(&mutex);
    free(thrds_ary);

    //
    printf("num_of_tosses = %lld, CPU_cores = %d\n",num_of_tosses,CPU_cores);
    gettimeofday(&end,NULL);
    diff = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
    printf("Total spent: %ld us, %lf secs\n",diff, diff/1000000.0);
}
int is_even_func(int n){
    if(n%2 == 0)
        return 1;
    else
        return 0;
}
void test_macro(){
    printf("macro_version:\n");
    struct  timeval start,end;
    unsigned  long diff;
    gettimeofday(&start,NULL);
    long long int sum = 0;
    for(long long int i = 0;i < 2147483647L;i++){
        sum += is_even_macro(i);
    }
    printf("even number = %lld\n",sum);
    gettimeofday(&end,NULL);
    diff = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
    printf("Total spent: %ld us, %lf secs\n",diff, diff/1000000.0);

    printf("\nfunction_version:\n");
    gettimeofday(&start,NULL);
    sum = 0;
    for(long long int i = 0;i < 2147483647L;i++){
        sum += is_even_func(i);
    }
    gettimeofday(&end,NULL);
    diff = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
    printf("Total spent: %ld us, %lf secs\n",diff, diff/1000000.0);
}
void IO_test(int n){
    struct  timeval start,end;
    unsigned  long diff;
    gettimeofday(&start,NULL);
    for(int i = 0;i < n;i++){
        printf("%d\n",i);
    }
    gettimeofday(&end,NULL);
    diff = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
    printf("Total spent: %ld us, %lf secs\n",diff, diff/1000000.0);
}
int main()
{
    //single for-loop
    printf("===================================\n");
    printf("Integer for-loop: \n");
    test_int();
    printf("\nFloating point for-loop: \n");
    test_float();

    //nested for-loop
    printf("===================================\n");
    printf("Integer for-loop(nested): \n");
    test_int2();
    printf("\nFloating point for-loop(nested): \n");
    test_float2();

    // monte-carlo simulation of pi (CPU_cores = 1)
    printf("===================================\n");
    pi_sim(100000000,1);

    // monte-carlo simulation of pi (CPU_cores = 2)
    printf("===================================\n");
    pi_sim(100000000,2);

    // monte-carlo simulation of pi (CPU_cores = 4)
    printf("===================================\n");
    pi_sim(100000000,4);

    // macro
    printf("===================================\n");
    test_macro();

    // I/0
    printf("===================================\n");
    IO_test(100000);
    return 0;
}
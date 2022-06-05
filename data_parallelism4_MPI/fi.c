#include <time.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#define real_res  1.202056903159594
int main(int argc, char** argv)
{
	 const int root=0;
	   int rank;
	   int coreSize;
	   int n;
	   long double result;
	   long double val;
	   long double sum;
	   long double totalTimeSerial=0.0,calTimeSerial=0.0;
	   long double totalTimeParallel=0.0;
	   long double startTimeParallel=0.0,endTimeParallel=0.0;
	     clock_t startTimeSerial,endTimeSerial;
	   MPI_Status status;	/* return status for 	*/
        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	 MPI_Comm_size(MPI_COMM_WORLD, &coreSize);
	  if(rank==root)
	 {
		 //read  from user
		 printf("Enter the start: ");
		 scanf("%d",&n);
		 startTimeParallel=MPI_Wtime();
		 
	 }
		  MPI_Bcast(&n, 1, MPI_INT, root, MPI_COMM_WORLD);
         if(rank!=root)
		 {
			  startTimeSerial=clock();
			 long long st=(rank-1)*(1e6/(coreSize-1))+n;
			 long long end=st+(1e6/(coreSize-1));
			 val=0.0;
			 sum=0.0;
			 calTimeSerial=0.0;
			 
			 for(st;st<end;st++)
			 {
			    val= 1.0/(st*st*st);
				sum+=val;
				
			 }
			 endTimeSerial=clock();
			 calTimeSerial+=(long double)(endTimeSerial-startTimeSerial)/CLOCKS_PER_SEC;
		 }	
        MPI_Reduce(&sum, &result, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, 
					MPI_COMM_WORLD); 	
		 MPI_Reduce(&calTimeSerial, &totalTimeSerial, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, 
					MPI_COMM_WORLD); 	
								
	    if(rank==root)
		{
			long double diff= real_res-result;
			printf("The total sum of the equation = %0.15Lf\n",result);
			printf("The Error between real value and program value is %0.15LF\n",diff);
			endTimeParallel=MPI_Wtime();
			 totalTimeParallel=endTimeParallel-startTimeParallel;
			 long double speedUp=totalTimeSerial/totalTimeParallel;
			 long double efficiency=speedUp/coreSize;
			 printf("The Totel time of parallel is = %0.15Lf\n",totalTimeParallel);
			 printf("The Total time of serial is = %0.15LF\n ",totalTimeSerial);
			 printf("The speed of parallel program    = %0.15Lf\n",speedUp);
			 printf("Efficiency of a parallel program = %0.15LF\n",efficiency);
		}
	 	  MPI_Finalize();

}
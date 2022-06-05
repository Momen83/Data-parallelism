#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	
         int rank;
         int buf=0;
         const int root=0;
         int idS [100];
         int grades [100];
		 int coreSize;
		int arr_sz = 0;
		int element=0;
		int element2=0;
		int element3=0;
	    int end_signal=-1;
		int tag = 0;		/* tag for messages	*/
		MPI_Status status;	/* return status for 	*/
        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	 MPI_Comm_size(MPI_COMM_WORLD, &coreSize);
	    int numberOfSuc=0;
		int Total=0;
		//master
        if(rank == root) {
			
            FILE *fpo;
           fpo=fopen("/shared/fileGrades.txt","r");
		   
          char line[100];
		  int pr=0;
          while (fgets(line, 100, fpo)) {
                sscanf(line, "%d %d", &idS[buf],&grades[buf]);
				int coreNum=pr%(coreSize-1);
				coreNum++;
			MPI_Send( &idS[buf], 1, MPI_INT, coreNum, tag, MPI_COMM_WORLD);
			MPI_Send( &grades[buf], 1, MPI_INT,coreNum, tag, MPI_COMM_WORLD);
				//printf(idS[buf]);
				//printf(grades[buf]);
				//puts(line);
				//printf("%d %d\n",idS[buf],grades[buf]);
                buf++;
				pr++;
				//printf(arr_sz);
          }
		  
         
		   
		   int i=1;
		   
		   for (;i<coreSize;i++)
		   {
			   MPI_Send( &end_signal, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
			   element3=0;
			  // MPI_Recv(&element3, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status );
			  // Total+=element3;
		   }

		 
		    fclose(fpo);
        }

       // printf("[%d]: Before Bcast, buf is %d\n", rank, buf);

        /* everyone calls bcast, data is taken from root and ends up in everyone's buf */
        MPI_Bcast(&buf, 1, MPI_INT, root, MPI_COMM_WORLD);
		//MPI_Bcast(&end_signal, 1, MPI_INT, root, MPI_COMM_WORLD);
		
		if(rank!=root)
		{
		int flag=1;
		//int numberOfSuc=0;
		while(flag==1)
		{
			MPI_Recv(&element, 1, MPI_INT, root, tag, MPI_COMM_WORLD, &status );
			if(element!=-1)
			{
				 MPI_Recv(&element2, 1, MPI_INT, root, tag, MPI_COMM_WORLD, &status );
				 printf("in process num %d \n",rank);
				 if(element2>=60)
				 {
					 printf("Student with id %d,Successed\n",element);
					 numberOfSuc++;
				 }
				 else
				 {
					 printf("Student with id %d ,Failed\n",element);
					 
				 }
			}
		   
			else
			{
				flag=0;
				printf("number of suc %d \n",numberOfSuc);
				 MPI_Send( &numberOfSuc, 1, MPI_INT, root, tag, MPI_COMM_WORLD);
			}
  
		}
			
		}
		if(rank==0)
		{
			int i=1;
		   for (;i<coreSize;i++)
		   {
			   
			  // MPI_Send( &end_signal, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
			   element3=0;
			  MPI_Recv(&element3, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status );
			   Total+=element3;
		   }
		    printf("Total number of students passed the exam is %d out of %d\n",Total,buf);
		}
			
       MPI_Finalize();
	  
       return 0;
}
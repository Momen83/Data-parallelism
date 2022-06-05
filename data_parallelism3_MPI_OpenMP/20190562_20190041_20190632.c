#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#define THREADS 5
int main(int argc, char** argv)
{
	   int* points,*ps,*ps2;
	   int *freqMaster,*freqProcess,*freqTemp;
	   int mx=-1,mxOther=-1;
	   int sizeLines=0,sz=0,sz2;
	   const int root=0;
	   int rank;
	   int coreSize;
	   int numOfElements=0;
       int bars;
	   int split=0;
	   int tag = 0;		/* tag for messages	*/
		MPI_Status status;	/* return status for 	*/
        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	 MPI_Comm_size(MPI_COMM_WORLD, &coreSize);
	 //master
	 if(rank==root)
	 {
		 int i;
		 //read bars from user
		 printf("Enter the number of bars : ");
		 scanf("%d",&bars);
		 printf("The numbers of bars are %d\n",bars);
		   FILE * fpo,*fpo2;
			fpo = fopen( "/shared/dataset.txt" , "r" );
			fpo2 = fopen( "/shared/dataset.txt" , "r" );
			
	     //count number of lines in file
			char ch;
		while((ch=fgetc(fpo))!=EOF) {
		if(ch=='\n')
			sizeLines++;
		}
		sizeLines++;
		fclose(fpo);
		//read file
		char lines[sizeLines];
		points = (int*)malloc(sizeLines * sizeof(int));
		int index=0;
		while ( fgets( lines , sizeLines , fpo2 )) {
        sscanf( lines , "%d" , & points[index] );
		if(mx<points[index])
		{
			mx=points[index];
		}
        index++;
		}
		  freqMaster=(int*)malloc((mx+1) * sizeof(int));
		   #pragma omp parallel shared(freqMaster)
		   {
			    #pragma omp for schedule(dynamic)
			     for(i=0;i<=mx;i++)
				{
					freqMaster[i]=0;
				}
		   }			   
		 
			
			numOfElements=sizeLines/(coreSize-1);
			int pos=0;
			 
			if(coreSize>1)
			{ 
				for(i=0;i<coreSize-2;i++)
				{ 
					pos=i*numOfElements;
					
					 MPI_Send(&numOfElements, 1, MPI_INT, i+1, tag, MPI_COMM_WORLD);
		             MPI_Send(points+pos, numOfElements, MPI_INT, i+1, tag, MPI_COMM_WORLD);
					 MPI_Send(&mx, 1, MPI_INT, i+1, tag, MPI_COMM_WORLD);	
				}
			}
			int rem = numOfElements+sizeLines%(coreSize-1);
		     pos = i * numOfElements;
			  printf("The remmining element are %d\n",rem);
		       MPI_Send(&rem, 1, MPI_INT, i+1, tag, MPI_COMM_WORLD);
		      MPI_Send(points+pos, rem, MPI_INT, i+1, tag, MPI_COMM_WORLD);
			   MPI_Send(&mx, 1, MPI_INT, i+1, tag, MPI_COMM_WORLD);	
			for(i=1;i<coreSize;i++)
			{
				 MPI_Recv(&mx, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status );
				  freqTemp=(int*)malloc((mx) * sizeof(int));
		         MPI_Recv(freqTemp,mx, MPI_INT, i, tag, MPI_COMM_WORLD, &status );
		         int j;
				 #pragma omp parallel shared(freqMaster,freqTemp) 
				 {
					  #pragma omp for schedule(dynamic)
				    for(j=0;j<mx;j++)
				   {
					 freqMaster[j]+=freqTemp[j];
				   }
				 }
				 
				  
			}
			
			//split data
			split=(mx-1)/bars;
			int start=0;
			int end=split;
			for(i=1;i<=bars;i++)
			{
				
				
				int countTotal=0;
				printf("The range start with %d, end with %d with count ",start,end);
				int j;
				for(j=start;j<=end;j++)
				{
					countTotal+=freqMaster[j];
				}
				printf("%d\n",countTotal);
				start=end+1;
				end=(start-1)+split;
			}
	 }
	 else
	 {
		  int i;
		 MPI_Recv(&sz, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status );
		 ps=(int*)malloc(sz * sizeof(int));
		 MPI_Recv(ps, sz, MPI_INT, 0, tag, MPI_COMM_WORLD, &status );
		 MPI_Recv(&mxOther, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status );
		 mxOther++;
		 freqProcess=(int*)malloc((mxOther) * sizeof(int));
		  #pragma omp parallel shared(freqProcess)
		  {
			     #pragma omp for schedule(dynamic)
			   for(i=0;i<mxOther;i++)
				{
			       freqProcess[i]=0;
				}
		  }
		
		 printf("Max value of element is %d\n",mxOther);
		 printf("The points from process number %d is:\n and total points are %d\n",
		 rank,sz);
		 for(i=0;i<sz;i++)
		 {
			 printf("%d,",ps[i]);
		 }
		 printf("\n");
		  #pragma omp parallel shared(freqProcess,ps)
		  {
			   #pragma omp for schedule(dynamic)
			   for(i=0;i<sz;i++)
			{
				freqProcess[ps[i]]++;
			}
		  }
		
		 MPI_Send(&(mxOther), 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
		 MPI_Send(freqProcess, mxOther, MPI_INT, 0, tag, MPI_COMM_WORLD);
	   
	 }
	  MPI_Finalize();

}
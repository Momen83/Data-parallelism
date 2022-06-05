#include <omp.h>
#include <stdio.h>
#include <math.h>

#define THREADS 5
int checkEquality(int x1[],int y1[],int x2[],int y2[],int sz)
{
	int i=0;
	for(;i<sz;i++)
	{
		if(x1[i]!=x2[i] ||y1[i]!=y2[i])
		{
			return 0;
		}
			
	}
	return 1;
}
int main( ) {
	//to generate a new patch of pseudo-random numbers
    srand(time(0));
    int id , numOfThreads , np;
    int x[1000];
    int y[1000];
    char line[100];
    int xThread[1000];
    int yThread[1000];
    int oldMeanX[1000];
	int oldMeanY[1000];
	
    int cntLines = 0;
    numOfThreads = THREADS;
    printf( "numbers of thread is %d\n" , numOfThreads );
    //Read file From Main Thread
    FILE * fpo;
    fpo = fopen( "/shared/data.txt" , "r" );
    while ( fgets( line , 100 , fpo )) {
        sscanf( line , "%d %d" , & x[ cntLines ] , & y[ cntLines ] );
        cntLines++;
    }
    double distance[THREADS][cntLines];
    int pointToThread[1000][1000];
    //Initiate 2 random numbers for each thread/cluster
    int i = 0;
    for ( ; i < numOfThreads ; i++ ) {
        xThread[ i ] = x[ rand( ) % cntLines ];
        yThread[ i ] = y[ rand( ) % cntLines ];
		oldMeanX[i]=oldMeanY[i]=0;
    }
    
    int t;
   while(checkEquality(xThread,yThread,oldMeanX,oldMeanY,numOfThreads)==0) {
        //calculate distance 
#pragma omp parallel num_threads(THREADS) shared(distance , xThread , yThread , x , y , cntLines , pointToThread)
        {
            //#pragma omp for schedule(static) 
            for ( i = 0 ; i < cntLines ; i++ ) {
                id = omp_get_thread_num( );
                int xCenter = xThread[ id ];
                int yCenter = yThread[ id ];
                int dis = ( xCenter - x[ i ] ) * ( xCenter - x[ i ] )
                          + ( yCenter - y[ i ] ) * ( yCenter - y[ i ] );
                distance[ id ][ i ] = sqrt( dis );
            }

        }
        for ( i = 0 ; i < cntLines ; i++ ) {
            int j = 0;
            for ( ; j < numOfThreads ; j++ ) {
                pointToThread[ j ][ i ] = 0;
            }

        }
        //Filter each point to thread
        for ( i = 0 ; i < cntLines ; i++ ) {
            int j = 0;
            double minDis = 100000.0;
            int minThread = -1;
            for ( ; j < numOfThreads ; j++ ) {
                if ( minDis > distance[ j ][ i ] ) {
                    minDis = distance[ j ][ i ];
                    minThread = j;
                }
            }
            pointToThread[ minThread ][ i ] = 1;
        }

        int cnt = 0;
        int sumX = 0;
        int sumY = 0;
//calculate new mean
#pragma omp parallel num_threads(THREADS) shared(xThread , yThread , x , y , cntLines , distance , pointToThread)
        {

            for ( i = 0 ; i < cntLines ; i++ ) {
                if ( pointToThread[ omp_get_thread_num( ) ][ i ] > 0 ) {
                    cnt++;
                    sumX += x[ i ];
                    sumY += y[ i ];
                }

            }
			oldMeanX[omp_get_thread_num( ) ] = xThread[ omp_get_thread_num( ) ];
			oldMeanY[omp_get_thread_num( ) ] = yThread[ omp_get_thread_num( ) ];
            if ( cnt > 0 ) {
                xThread[ omp_get_thread_num( ) ] = sumX / cnt;
                yThread[ omp_get_thread_num( ) ] = sumY / cnt;
            }
            cnt = 0;
            sumX = 0;
            sumY = 0;
        }
		
    }
	 for ( i = 0 ; i < numOfThreads ; i++ ) {
            int j = 0;
			
            printf("cluster number %d with Mean value (%d,%d):\n",i,xThread[i],yThread[i]);
            for ( ; j < cntLines ; j++ ) {
				if(pointToThread[i][j]==1)
				{
					  printf("(%d,%d),",x[j],y[j]);
				}
              
            }
			printf("\n");

        }

}

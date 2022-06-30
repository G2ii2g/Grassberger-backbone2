#include <stdio.h>
#include<stdlib.h>
#include<limits.h>
#include <vector>
using namespace std;
char resultFileName[] = "result.txt";
char inputGraphFileName[]="InputGraph.txt";
unsigned L=8;
int int_max=INT_MAX;
void north(int x, int y);
void south(int x, int y);
void west(int x, int y);
void east(int x, int y);
vector<vector<int>>s;
int m;
int q=2;
float p;
bool perc;

void outMatrix(char* resultFileName)
{
    FILE *fo=fopen(resultFileName, "w");
    for (int y =L-1; y >=0; y--) {
        for (int x = 0; x <L; x++)
            fprintf(fo, "%4d ",s[x][y]);
        fprintf(fo, "\n");
    }
    fclose(fo);
}

void outInputGraph()
{
    FILE *fo=fopen(inputGraphFileName, "w");
    for (int y =L-1; y >=0; y--) {
        for (int x = 0; x <L; x++)
            fprintf(fo, "%d ",s[x][y] && s[x][y]<int_max);
        fprintf(fo, "\n");
    }
    fclose(fo);
}

int main(int argc, char **argv)
{
	unsigned seed=1;
	if (argc > 1)
		sscanf (argv[1], "%u", &L);
    s=vector<vector<int>>(L,vector<int>(L));
    int_max=L*L*8;
    if (argc > 2)
		sscanf(argv[2], "%u", &seed);
	srand(seed);
	printf("seed for srand= %u\n", seed);
	p = .592746 * RAND_MAX;
	for (int x=0;x<L;x++)
        north(x,0);
	printf("backbone mass= %d\n", m);
	if (perc)
        printf("cluster percolates !\n");
    else
		printf("cluster does not percolate !\n");
	//outMatrix(resultFileName);
	outInputGraph();
    return 0;
}

void north(int x,int y)
{
	if (y<L)
	{
		if (s[x][y]==0)
		{
			if (rand()<p)
			{
			    m++;
				s[x][y]=q;
                if ((q&1)==0)
                {
                    west(x-1 ,y);
                    north(x, y+1) ;
                    east(x+1,y);
                }
                else
                {
                    east(x+1,y);
                    north(x, y+1);
                    west(x-1 ,y);
                }
				if (q==s[x][y])
				m--;
			}
			else s[x][y]=int_max;
		}
		else if (q>s[x][y]) q+=2;
	}
	else
	{
		q++,q+=(q&1)==0;
		perc=true;
	}
}

void east (int x,int y)
{
	if (x<L)
	{
		if (s[x][y]==0)
		{
			if (rand()<p)
			{
			    m++;
			    s[x][y]=q;
                if ((q&1)==0)
                {
                    north(x , y+1) ;
                    east(x+1,y);
                    south(x ,y-1) ;
                }
                else
                {
                    south(x ,y-1) ;
                    east(x+1,y);
                    north(x , y+1) ;
                }
				if (q==s[x][y])
				m--;
			}
			else s[x][y]=int_max;
		}
		else if (q>s[x][y]) q+=2;
	}
}

void south(int x,int y)
{
    if (y>=0)//В статье Грассбергера ошибочно y>0
    {
        if (s[x][y]==0)
		{
			if (rand()<p)
			{
                m++;
			    s[x][y]=q;
                if ((q&1)==0)
                {
                    east(x+1,y);
                    south(x ,y-1) ;
                    west(x-1,y);
                }
                else
                {
                    west(x-1,y);
                    south(x ,y-1);
                    east(x+1,y);
                }
				if (q==s[x][y])
				m--;
			}
			else s[x][y]=int_max;
		}
		else if (q>s[x][y]) q+=2;
    }
	else q++,q+=q&1;
}

void west (int x,int y)
{
    if (x>=0)//В статье Грассбергера ошибочно y>0
    {
        if (s[x][y]==0)
		{
			if (rand()<p)
			{
                m++;
			    s[x][y]=q;
			    if ((q&1)==0)
                {
                    south(x ,y-1) ;
                    west(x-1,y);
                    north(x , y+1);
                }
                else
                {
                    north(x,y+1);
                    west(x-1,y);
                    south(x,y-1);
                }
				if (q==s[x][y])
                    m--;
			}
			else s[x][y]=int_max;
		}
		else if (q>s[x][y]) q+=2;
    }
}

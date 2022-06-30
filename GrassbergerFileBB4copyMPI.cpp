#include<stdlib.h>
#include<limits.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include<algorithm>
#include <profileapi.h>
#include "mpi.h"
using namespace std;
char* inputFileName = (char*)"graph.txt";
char resultFileName[] = "result.txt";
char* bbFileName= (char*)"backbone.txt";
vector<vector<int>>s;// ��������� ������ ��� ������ ������� �� �����
int L; //������ (������� �������)
int *s1;
char *bb1, *clearbb1;
int int_max=INT_MAX;
void north(int x, int y);
void south(int x, int y);
void west(int x, int y);
void east(int x, int y);
int m;
int q=2; //�����, ������������ �������� �� ������� ��������������
float p;

static inline int64_t GetTicks()
{
    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);

    return ticks.QuadPart;
}

static inline int64_t GetTicksPerSec()
{
    LARGE_INTEGER Frequency;

    QueryPerformanceFrequency(&Frequency);
    return Frequency.QuadPart;
}

int inMatrix ()//������ �������� �����
{
    int black=0;
    ifstream fi(inputFileName);//���������� ��� ������ �����
    string str;// ���������� ��� ������ ��������� ������ �����
    for (int y=0;getline(fi,str);y++)//���� �������� ���� ���� ������ � �����
    {
        stringstream ss;//��������� ���������� ��� ��������� �����
        ss << str;//�������� ����������� ������ � ����
        int tmp;// ���������� ��� ���������� ��������
        for (int x=0; ss >> tmp; x++)// ���� ������������ ��� �������� � ������
        {
            if(y==0)// ���� ������ ������
                s.push_back(vector<int>(0));// ������� ����� ������ ������ ��� ������� �������� ������ ������
            s[x].push_back(tmp);// �������� ��������� ������� � ��������� ������, ��� ������������, ��� x �������� ��������, ��� y
            black+=tmp;
        }
    }
    L = s.size();//���������� ������ �������
    for (int x=0;x<L;x++)// ���������� ��� x
        reverse (s[x].begin(),s[x].end());// �������������� ������, ����� �������� ����������� ���������� ����� ������ y=0
    int_max=L*L*8;
    return black;
}

void outBB1Matrix(char *bb1, const char *bbFileName)
{
    FILE* fo =fopen(bbFileName, "w");
    for (int y =L-1; y >=0; y--) {
        for (int x = 0; x <L; x++)
            fprintf(fo, "%2d ",int (bb1[x*L+y]));
        fprintf(fo, "\n");
    }
    fclose(fo);
}

void outS1Matrix(FILE *fo)
{
    for (int y =L-1; y >=0; y--) {
        for (int x = 0; x <L; x++)
            fprintf(fo, "%4d ",s1[x*L+y]);
        fprintf(fo, "\n");
    }
}

int countbb1(char* bb1)
{
    m=0;
    for(int i=0;i<L*L;i++)
        m+=bb1[i]==1;
    return m;
}

int main(int argc,char**argv)
{
    int64_t t1, t2;
    int rank,size;
    MPI_Init(NULL,NULL);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if(rank==0)
    {
        int flagout=1;
        if(argc>1){
            inputFileName=argv[1];
            if(argc>2)
                bbFileName=argv[2];
            if(argc>3)
                sscanf(argv[3],"%d",&flagout);
        }
        int black=inMatrix();
        t1=GetTicks();
        MPI_Bcast(&L,1,MPI_INT,0,MPI_COMM_WORLD);
        s1 = new int[L*L];
        bb1 = new char[L*L];
        fill(bb1,bb1+L*L, char(0));
        for (int x=0;x<L;x++)
            copy(s[x].begin(),s[x].end(),s1+x*L);
        s.resize(0);
        MPI_Bcast(s1,L*L,MPI_INT,0,MPI_COMM_WORLD);
        for (int x=0;x<L;x++)
            north(x,0);
        delete[] s1;
        MPI_Reduce(MPI_IN_PLACE,bb1,L*L,MPI_CHAR,MPI_PROD,0,MPI_COMM_WORLD);
        t2=GetTicks();
        printf("backbone mass= %d\n", countbb1(bb1));
        if(m==0)
            printf ("cluster does not percolate !\n");
        else
            printf ("cluster percolates !\n");
        double sec = double (t2 - t1)/GetTicksPerSec();
        printf("runtime = %.7f\n",sec);
        FILE *f3;
        char* repFile = (char*)"ReportGrassFileBB4copyMPI.txt";
        f3=fopen(repFile,"a+");
        fseek(f3,0,SEEK_END);
        if(ftell(f3)==0)
            fprintf(f3,"FileName L Black_c Backbone_c Time \n");
        int q_backbone=m, q_black= black;
        fprintf(f3,"%s %d %6d %6d %.7f\n",inputFileName,L,q_black,q_backbone,sec);
        fclose(f3);
        if(flagout)
            outBB1Matrix(bb1,bbFileName);
    }
    else
    {
        MPI_Bcast(&L,1,MPI_INT,0,MPI_COMM_WORLD);
        s1 = new int[L*L];
        bb1 = new char[L*L];
        fill(bb1,bb1+L*L, char(0));
        MPI_Bcast(s1,L*L,MPI_INT,0,MPI_COMM_WORLD);
        if(rank==1)
            reverse(s1,s1+L*L);// ������� ������� �� 180 ��������
        if(rank==2)
            for(int i=0;i<L;i++)// ������������ ��������� �������
                reverse(s1+L*i,s1+L*i+L);
        if (rank==3)
            for(int i=0;i<L/2;i++)// �������������� ��������� �������
                swap_ranges(s1+L*i,s1+L*i+L,s1+L*(L-i-1));
        for (int x=0;x<L;x++)
            north(x,0);
        delete[] s1;
        if(rank==1)
            reverse(bb1,bb1+L*L);// �������� ������� ������ �� 180 ��������
        if(rank==2)
            for(int i=0;i<L;i++)
                reverse(bb1+L*i,bb1+L*i+L);// �������� ������������ ��������� ������
        if(rank==3)
            for(int i=0;i<L/2;i++)// �������� �������������� ��������� ������
                swap_ranges(bb1+L*i,bb1+L*i+L,bb1+L*(L-i-1));
        MPI_Reduce(bb1,MPI_IN_PLACE,L*L,MPI_CHAR,MPI_PROD,0,MPI_COMM_WORLD);
    }
    MPI_Finalize();
    delete[]bb1;
    return 0;
}

void north(int x,int y)
{
	if (y<L)
	{
		if (s1[x*L+y]<2)
		{
			if (s1[x*L+y]==1)
            {
			    m++;
				s1[x*L+y]=q;
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
				if (q==s1[x*L+y])
                    bb1[x*L+y]=2, m--;
                else bb1[x*L+y]=1;
            }
            else s1[x*L+y]=int_max;
        }
		else if (q>s1[x*L+y]) q+=2;
	}
	else
	{
	    q++,q+=(q&1)==0;
		//printf ("cluster percolates !\n");
	}
}
void east (int x,int y)
{
	if (x<L)
	{
		if (s1[x*L+y]<2)
		{
		    if (s1[x*L+y]==1)
            {
			    m++;
			    s1[x*L+y]=q;
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
				if (q==s1[x*L+y])
                    bb1[x*L+y]=2, m--;
                else bb1[x*L+y]=1;
            }
            else s1[x*L+y]=int_max;
		}
		else if (q>s1[x*L+y]) q+=2;
	}
}

void south(int x,int y)
{
    if (y>=0)//� ������ ������������ �������� y>0
    {
        if (s1[x*L+y]<2)
		{
		    if (s1[x*L+y]==1)
            {
                m++;
			    s1[x*L+y]=q;
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
				if (q==s1[x*L+y])
                    bb1[x*L+y]=2, m--;
                else bb1[x*L+y]=1;
            }
            else s1[x*L+y]=int_max;
		}
		else if (q>s1[x*L+y]) q+=2;
    }
    else q++,q+=q&1;
}

void west (int x,int y)
{
    if (x>=0)//� ������ ������������ �������� y>0
    {
        if (s1[x*L+y]<2)
		{
		    if (s1[x*L+y]==1)
            {
                m++;
			    s1[x*L+y]=q;
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
				if (q==s1[x*L+y])
                    bb1[x*L+y]=2, m--;
                else bb1[x*L+y]=1;
            }
            else s1[x*L+y]=int_max;
		}
		else if (q>s1[x*L+y]) q+=2;

    }
}

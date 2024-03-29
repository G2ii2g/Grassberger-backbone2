#include<stdlib.h>
#include<limits.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include<algorithm>
#include <profileapi.h>
using namespace std;
char* inputFileName = (char*)"graph.txt";
char* resultFileName = (char*)"result.txt";
char* bbFileName = (char*)"backbone.txt";
vector<vector<int>>s;// ��������� ������ ��� ������ ������� �� �����
vector<vector<char>>bb;
int L; //������ (������� �������)
int int_max=INT_MAX;
void north(int x, int y);
void south(int x, int y);
void west(int x, int y);
void east(int x, int y);
int m;
int q=2; //�����, ������������ �������� �� ������� ��������������
float p;
bool perc;

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

void outBBMatrix()
{
    FILE* fo =fopen(bbFileName, "w");
    for (int y =L-1; y >=0; y--) {
        for (int x = 0; x <L; x++)
            fprintf(fo, "%d ",int (bb[x][y]));
        fprintf(fo, "\n");
    }
    fclose(fo);
}

int main(int argc,char**argv)
{
    int64_t t1, t2;
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
    bb=vector<vector<char>>(L,vector<char>(L));
    for (int x=0;x<L;x++)
            north(x,0);
    t2=GetTicks();
    printf("backbone mass= %d\n", m);
    if (perc)
        printf("cluster percolates !\n");
    else
		printf("cluster does not percolate !\n");
    double sec = double (t2 - t1)/GetTicksPerSec();
    printf("runtime = %.7f\n",sec);
    FILE *f3;
	char* repFile = (char*)"ReportGrassFileBB.txt";
	f3=fopen(repFile,"a+");
	fseek(f3,0,SEEK_END);
	if(ftell(f3)==0)
		fprintf(f3,"FileName L Black_c Backbone_c Perc Time\n");
	int q_backbone=m, q_black= black;
	fprintf(f3,"%s %d %6d %6d %d %.7f\n",inputFileName,L,q_black,q_backbone,int(perc),sec);
	fclose(f3);
	if(flagout)
        outBBMatrix();
    return 0;
}

void north(int x,int y)
{
	if (y<L)
	{
		if (s[x][y]<2)
		{
		    if (s[x][y]==1)
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
                    bb[x][y]=2, m--;
                else bb[x][y]=1;
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
		if (s[x][y]<2)
		{
		    if (s[x][y]==1)
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
                    bb[x][y]=2, m--;
                else bb[x][y]=1;
			}
			else s[x][y]=int_max;
		}
		else if (q>s[x][y]) q+=2;
	}
}

void south(int x,int y)
{
    if (y>=0)//� ������ ������������ �������� y>0
    {
        if (s[x][y]<2)
		{
		    if (s[x][y]==1)
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
                    bb[x][y]=2, m--;
                else bb[x][y]=1;
			}
			else s[x][y]=int_max;
		}
		else if (q>s[x][y]) q+=2;
    }
    else q++,q+=q&1;
}

void west (int x,int y)
{
    if (x>=0)//� ������ ������������ �������� y>0
    {
        if (s[x][y]<2)
		{
		    if (s[x][y]==1)
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
                    bb[x][y]=2, m--;
                else bb[x][y]=1;
			}
			else s[x][y]=int_max;
		}
		else if (q>s[x][y]) q+=2;
    }
}

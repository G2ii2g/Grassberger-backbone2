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
char resultFileName[] = "result.txt";
char* bbFileName= (char*)"backbone.txt";
vector<vector<int>>s;// Двумерный вектор для чтения решетки из файла
int *s1;//одномерный массив для решетки
char *bb1;// одномерный массив для остова
int L; //размер (сторона решетки)
int int_max=INT_MAX;
void north(int x, int y);
void south(int x, int y);
void west(int x, int y);
void east(int x, int y);
int m;
int q=2; //метка, показывающая является ли кластер перколяционным
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

int inMatrix ()//Чтение входного файла
{
    int black=0;
    ifstream fi(inputFileName);//переменная для чтения файла
    string str;// переменная для чтения очередной строки файла
    for (int y=0;getline(fi,str);y++)//цикл работает пока есть строки в файле
    {
        stringstream ss;//потоковая переменная для обработки строк
        ss << str;//помещаем прочитанную строку в файл
        int tmp;// переменная для очередного элемента
        for (int x=0; ss >> tmp; x++)// цикл обрабатывает все элементы в строке
        {
            if(y==0)// если первая строка
                s.push_back(vector<int>(0));// создаем новый пустой вектор для каждого элемента первой строки
            s[x].push_back(tmp);// помещаем очередной элемент в очередной вектор, это обеспечивает, что x меняется медленее, чем y
            black+=tmp;
        }
    }
    L = s.size();//Определяем размер матрицы
    for (int x=0;x<L;x++)// перебираем все x
        reverse (s[x].begin(),s[x].end());// Переворачиваем вектор, чтобы элементы прочитанные последними имели индекс y=0
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

void initalg()
{
    for (int x=0;x<L;x++)
        copy(s[x].begin(),s[x].end(),s1+x*L);
    fill(bb1,bb1+L*L, char (0));
    m=0;
    q=2;
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
    s1=new int [L*L];
    bb1=new char [L*L];
    initalg();
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
	char* repFile = (char*)"ReportGrassFileBB1arr.txt";
	f3=fopen(repFile,"a+");
	fseek(f3,0,SEEK_END);
	if(ftell(f3)==0)
		fprintf(f3,"FileName L Black_c Backbone_c Perc Time\n");
	int q_backbone=m, q_black= black;
	fprintf(f3,"%s %d %6d %6d %d %.7f\n",inputFileName,L,q_black,q_backbone,int(perc),sec);
	fclose(f3);
	if(flagout)
        outBB1Matrix(bb1, bbFileName);
    delete []s1;
    delete []bb1;
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
		perc=true;
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
    if (y>=0)//В статье Грассбергера ошибочно y>0
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
    if (x>=0)//В статье Грассбергера ошибочно y>0
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

#include<iostream>
#include<fstream>
#include<string>
#include<cstdlib>
#include<string.h>
#include<pthread.h>
#include<sys/sysinfo.h> //for get_nprocs()
using namespace std;
int ** matrix1,**matrix2,**matrix3; 
int row1=0,row2=0,col1=0,col2=0;
struct Data
{
	int count;
	int startrow;
	int place;
	Data()
	{
		count=0;
		startrow=0;
		place=0;
	}
};
void MatrixInitializer(int**& matrix,int row,int col)
{
	matrix=new int* [row];
	for(int k=0;k<row;k++)
		matrix[k]=new int[col];
	for(int m=0;m<row;m++)
		for(int n=0;n<col;n++)
			matrix[m][n]=0;
}
void InputFileReader(int **&mat,int &row,int& col,char* str)
{
	int **matrix;
	char line[1000];
	ifstream fin (str);
	if (fin.is_open())
	{
		fin.getline (line,1000,'\n');
		char * pch;
		pch = strtok (line,",");
		row=atoi(pch);
		pch = strtok (NULL, ",");
		col=atoi(pch);
		MatrixInitializer(matrix,row,col);
		int i=0;
		while ( i<row)
		{
			int j=0;
			fin.getline (line,1000);
			pch = strtok (line,",");
			while (pch != NULL && j<col)
			{
				matrix[i][j]=atoi(pch);
				pch = strtok (NULL, ",");
				j++;
			}
			i++;
		}
		mat=matrix;
		fin.close();
	}
	else cout << "File Not Found "<<endl;
}
void Output(int **matrix,int row,int col)
{
	cout<<"Matrix Data:: "<<endl;
	cout<<"Row of matrix ::"<<row<<endl;
	cout<<"Column of matrix ::"<<col<<endl;
	for(int m=0;m<row;m++){
		for(int n=0;n<col;n++)
			cout<<matrix[m][n]<<"  ";
		cout<<endl;}
}
void* MatrixMultiplication(void* Parameter)
{
	Data* temp=(Data*)Parameter;
	int j=temp->place;
	int check=0;
	for(int i=temp->startrow; i<row1&& check<temp->count; i++){
		for(; j<col2 && check<temp->count; j++,check++)
		{
			for (int k=0; k<col1; k++) 
				matrix3[i][j] += matrix1[i][k]*matrix2[k][j];
		}
		j=0;
	}
	pthread_exit( (void*) NULL);
}
int main(int argc,char* argv[])
{
	int NumOfProcessor=get_nprocs();
	char file1[11]="input1.txt";
	char file2[11]="input2.txt";
	InputFileReader(matrix1,row1,col1,file1);
	InputFileReader(matrix2,row2,col2,file2);
	if(col1!=0 && row2!=0 && col1==row2)
	{
		Data Obj;
		MatrixInitializer(matrix3,row1,col2);
		Obj.startrow=0;
		Obj.place=0;
		Obj.count=(row1*col2)/NumOfProcessor;
		int element=Obj.count;
		if(element!=0)
		{
			int totalelement=row1*col2;
			int check=0,temp,flag=0;			
			while(check<totalelement)
			{
				pthread_t id;
				if(pthread_create(&id,NULL,&MatrixMultiplication,&Obj)==-1)
				{
					cout<<"Thread Creation Failed" <<endl;
					return 1;
				}
				pthread_join(id,(void**) NULL);
				temp=check;
				check+=element;
				Obj.startrow=check/col2;
				Obj.place=check%col2;
				if(check>totalelement && flag==0)
				{
					Obj.count=totalelement-temp;
					check=totalelement;
					flag=1;
				}
			}
			Output(matrix1,row1,col1);
			Output(matrix2,row2,col2);
			Output(matrix3,row1,col2);
			ofstream fout("output.txt");
			if(fout.is_open())
			{
				fout<<row1;
				fout<<',';
				fout<<col2;
				fout<<endl;
				for(int m=0;m<row1;m++){
					for(int n=0;n<col2;n++)
						fout<<matrix3[m][n]<<',';
					fout<<endl;}
				fout.close();
			}
			else 
				cout<<"Output File Not Found"<<endl;
		}
		else
			cout<<"Kindly Decrease processor size because subtasks are becoming 0 (infinite loop)"<<endl;
	}
	else
		cout<<"Matrices are not eligible for multiplication "<<endl;
	return 0;
}

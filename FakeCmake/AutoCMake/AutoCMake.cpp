#include<iostream>
#include<cstdlib>
#include<cstring>
#include<ctime>
//该工具为实现在VsCode自动传参的情况下实现编译传入文件夹内的所有源代码
//注意,AutoCMake无法编译自己
//2021.2.19首次构建

int main(int argc,char*argv[]){
    //argv[0]是默认的程序名(含路径,默认传入)
    //argv[1]应为调用的编译器路径
    //argv[2]应为工作区目录
    //argv[3]应为输出文件名称
    //argb[4]应为编译类型,如C/C++
    using namespace std;
    if(argc!=5){
        cout<<"输入格式不正确!!\n";
        cout<<"argv[1]应为调用的编译器路径\nargv[2]应为工作区目录\nargv[3]应为输出文件名称\nargv[4]应为编译类型\n";
        cout<<"程序将在5秒后退出\n";
        for(clock_t Now=clock();clock()-Now<5*CLOCKS_PER_SEC;);
        exit(EXIT_FAILURE);
    }

    //使用区块,用后即弃
    {
        char Cmd1[]="DIR ";
        if(!strcmp(argv[4],"C++")){
            char Cmd2[]="\\*.cpp /B >";
            char Cmd3[]="\\TempList.txt";
            char GetListCmd[strlen(argv[2])*2+strlen(Cmd1)+strlen(Cmd2)+strlen(Cmd3)+1];
            //合成打印文件目录的命令行
            sprintf(GetListCmd,"%s%s%s%s%s%c",Cmd1,argv[2],Cmd2,argv[2],Cmd3,'\0');
            system(GetListCmd);
        }
        if(!strcmp(argv[4],"C")){
            char Cmd2[]="\\*.c /B >";
            char Cmd3[]="\\TempList.txt";
            char GetListCmd[strlen(argv[2])*2+strlen(Cmd1)+strlen(Cmd2)+strlen(Cmd3)+1];
            //合成打印文件目录的命令行
            sprintf(GetListCmd,"%s%s%s%s%s%c",Cmd1,argv[2],Cmd2,argv[2],Cmd3,'\0');
            system(GetListCmd);
        }
        if(strcmp(argv[4],"C++")&&strcmp(argv[4],"C")){
        cout<<"编译类型格式不正确!!\n";
        cout<<"argv[4]应为\"C++\"或\"C\"\n";
        cout<<"程序将在5秒后退出\n";
        for(clock_t Now=clock();clock()-Now<5*CLOCKS_PER_SEC;);
        exit(EXIT_FAILURE);
        }
        //完成了文件名称的打印
        cout<<"完成文件录入,详见TempList.txt\n";
    }
    FILE* ListFile;
    char FileName[]="\\TempList.txt";
    char FileDirName[strlen(argv[2])+strlen(FileName)+1];
    sprintf(FileDirName,"%s%s%c",argv[2],FileName,'\0');
    ListFile=fopen(FileDirName,"r");

    //获取最长的文件名字长和行数
    int LongestNameLen=0;
    int LineCount=0;
    for(char Container=fgetc(ListFile);Container!=EOF;){
        //初始化时先行读取,然后进行条件测试,在循环末尾时进行下一次读取
        static int Count=0;
        if(Container!='\n')
            Count++;
        if(Container=='\n'){
            if(Count>LongestNameLen)
                    LongestNameLen=Count;
                Count=0;
                LineCount++;
        }
        Container=fgetc(ListFile);
    }

    fseek(ListFile,SEEK_SET,0);//回到文件起始位置
    //申请容纳文件名的空间并从文件读入,同样使用区块,用后即弃
    {
        char FileName[LineCount][LongestNameLen+1];//+1是为了给空字符预留空间
        int CmdLenth=0;
        for(int count=0;count<LineCount;count++){
            fscanf(ListFile,"%s",FileName[count]);
            CmdLenth+=strlen(FileName[count]);
        }
        //LineCount 为文件名间间隔和末尾空字符预留空间
        //(strlen(argv[2])+1)*LineCount为每个文件路径名和'\'符号预留空间
        //末尾+1为空字符
        char Files[CmdLenth+LineCount+(strlen(argv[2])+1)*LineCount+1];
        cout<<"正在处理以下文件:\n";
        for(int count=0;count<LineCount;count++){
            static int InPutCount=0;
            sprintf(Files+InPutCount,"%s\\%s%c",argv[2],FileName[count],' ');
            InPutCount+=(strlen(FileName[count])+strlen(argv[2])+2);//空字符和'\'计入
            cout<<FileName[count]<<'\n';
        }

        //将末尾置为空字符
        //Char Files[]索引从零开始,比元素数少1
        Files[CmdLenth+LineCount+(strlen(argv[2])+1)*LineCount]='\0';


        //关闭列表文件
        fclose(ListFile);


        //LongestNameLen已经不再使用了,可以起别名用于计数
        int & StrLength=LongestNameLen;
        StrLength=0;
        char Cmds[6][22]={"-g",//调试模式编译
                                            "-static-libgcc",//静态库链接
                                            "-fexec-charset=GBK",//输出文件字符编码
                                            "-finput-charset=UTF-8",//输入文件字符编码
                                            "-pthread",//链接Pthread库
                                            "-o"//输出可执行文件
                                          };
        for(int count=0;count<6;count++)
            StrLength+=(strlen(Cmds[count])+1);//为空格和空字符留出空间
        StrLength+=(strlen(argv[1])+strlen(Files)+strlen(argv[3])+3);//同理,计入各目录的长度
        char OutPutCmd[StrLength];
        for(int count=0;count<6;count++){
            static int InPutCount=0;
            if(count==0){
                //导入argv[1](编译器目录)
                sprintf(OutPutCmd,"%s%c",argv[1],' ');
                InPutCount+=(strlen(argv[1])+1);
            }
            if(count==5){
                sprintf(OutPutCmd+InPutCount,"%s%c%s%c%s\\%s%c",Files,' ',Cmds[count],' ',argv[2],argv[3],'\0');
                continue;
            }
            sprintf(OutPutCmd+InPutCount,"%s%c",Cmds[count],' ');
            InPutCount+=(strlen(Cmds[count])+1);            
        }
        cout<<"命令行处理完成\n";
        cout<<OutPutCmd<<'\n';
        system("cd C:\\VsCode\\MultiFiles");
        system(OutPutCmd);
    }
    cout<<"编译完成\n";
    return 0;
}
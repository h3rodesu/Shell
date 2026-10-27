#include<string>
# include<vector>
#include<sstream>
#include<iostream>
#include<unistd.h>//для chdir
#include<fcntl.h>
#include<sys/wait.h>
void exec_func(const std::vector<std::string>&strs){
 std::vector<char*>args;//вектор си строк
    for(size_t i=0;i<strs.size();++i){
args.push_back(const_cast<char*>(strs[i].c_str()));
    }
args.push_back(nullptr);//необходимо для execvp
execvp(args[0],args.data());
    perror("execvp");//если execvp не сработает
exit(1);
}
int main(){

    while(true){
        std::cout<<"Shell>";    
        std::string pt;
        std::getline(std::cin,pt);
    std::vector<std::vector<std::string>>vec;
    std::vector<std::string>strs;
if(pt.empty()){
std::cout<<"Error"<<std::endl;
continue;
}
 std::stringstream ss(pt);
  std::string words;
while(ss>>words){
    strs.push_back(words);//бьется по пробелам
}   
//vec.push_back(nullptr);//обязательно для вызова execvp upd не в этом месте! стринг равный нуллптр крашит программу
if(strs[0]=="exit"){
    return 0;
}
if(strs[0]=="cd"){
    if(strs.size()>1){
if(chdir(strs[1].c_str())!=0){
 perror("cd");
}
    }
std::cout<<"\n";
    continue;
}
std::vector<std::string>cur_cmd;
for(int i=0;i<strs.size();++i){
    if(strs[i]=="|"){
vec.push_back(cur_cmd);
cur_cmd.clear();
}else{
    cur_cmd.push_back(strs[i]);
}
}
vec.push_back(cur_cmd);

int num_col=vec.size();//сколько всего команд
int prev_fd=-1;//хранит выход (конец пред канала дял чтения)
for(int i=0;i<vec.size();++i){//редиректы
std::string out;
std::string in;
std::vector<std::string>fwork;
for(int a=0;a<vec[i].size();++a){
    std::string cur=vec[i][a];
if(cur==">"||cur=="<"){
    if(a+1<vec[i].size()){
    if(cur=="<"){
        in=vec[i][a+1];//имя файла
    }
if(cur==">"){
    out=vec[i][a+1];
}
}
++a;
}
else{
fwork.push_back(cur);
}
}

    int pipefd[2];//массив для двух дескрипторов [0]-читать [1]-писать
if(i<num_col-1){//тк последней команде некуда писать
if(pipe(pipefd)<0){//создание канала на чтеие и запись
        perror("pipe");
    exit(1);
    }
}
    pid_t pid=fork();
if(pid==0){//если в ребенке
//if(i==0){
    //тут ничего не менять,первая команда,только ввод
//}
if(i>0){
    dup2(prev_fd,0);//дескриптор 0 теперь  принимает данныые не из клавиатуры а из конца предыдущего канала
close(prev_fd);
}
if(i<num_col-1){//т.е.остался последний элемент,тут нужен вывод
dup2(pipefd[1],1);//данные выведутся не на экран а в канал
close(pipefd[0]);//дочерний процесс только пишет сюда данные,читать их ему не нужно
close(pipefd[1]);//тк дескрипторы ведут в одно и то же место
}
if(!out.empty()){//есть редирект out
int out_fd=open(out.c_str(),O_WRONLY|O_CREAT|O_TRUNC,0644);
dup2(out_fd,1);//запись в файл
close(out_fd);
}
if(!in.empty()){
    int in_fd=open(in.c_str(),O_RDONLY);//читать можно только файл который уже есть
    if(in_fd<0){
        perror("in");
        exit(1);
    }
dup2(in_fd,0);//чтение
close(in_fd);
}

exec_func(fwork);//запуск программы !!!НОВЫЙ ВЕКТОР С ЧИСТЫИ КОМАНДАМИ
}
else if(pid>0){//т.е. в родителе
    if(i>0){
        close(prev_fd);
    }
    if(i<num_col-1){
close(pipefd[1]);//закрыли конец для записи
prev_fd=pipefd[0];//сохранили конец для чтения
}
}
    
}
for(int i=0;i<num_col;++i){
    wait(nullptr);
}
}

return  0;
}







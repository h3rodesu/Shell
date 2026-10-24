#include<string>
# include<vector>
#include<sstream>
#include<iostream>
#include<unistd.h>//для chdir
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
    strs.push_back(words);
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

for(int i=0;i<num_col;++i){
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
exec_func(vec[i]);//запуск программы
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


// cur_cmd.clear();
// std::vector<const char*>args;
// for(int i=0;i<vec.size();++i){
//     args.push_back(vec[i].c_str());//c_str вернет конст чар
// }
// args.push_back(nullptr);
// pid_t pid=fork();//создание ребенка, pid==0-ребенок,нужно чтобы сохрнить память шелла,скопировав процесс перед тем как
// //заменить его код другим процессом
// if(pid==0){
// execvp(args[0],const_cast<char**>(args.data()));//снять конст на время вызова 
// perror("execvp");//если execvp вернула управление-это ошибка
// exit(1);
// }
// else if(pid>0){//больше 0-означает что fork вернул pid созданного ребенка
// wait(nullptr);//ждем статус завергения программы,родитель спит,предотвращает появление зомби-процессов
// }
// }
        


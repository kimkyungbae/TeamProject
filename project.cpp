#include "project.h"

Total *total; //동적할당 하는 인스턴스인데 전역적으로 사용
int total_num = 0; //디렉토리와 파일 전체 갯수 저장할 변수
int count = 1;

int fileType(const struct stat *fileInfo); //파일인지 디렉토리인지 알려주는 함수
// void print_set(set<string> s);

void signalHandler(int signum);

string commands[5];

void execute();
void ls(string str); //디폴트 ls는 현재 디렉토리 출력
void cd(string str);
void mv(){};

int main() {
    execute();
    return 0;
}

void execute() {
    while (1) {
        string temp;
        cout << "command>> ";
        getline(cin, temp);
        int k = 0;
        int index = 0;

        int n = temp.find(" ");
        if (n == string::npos) {
            commands[0] = temp;
        } else {
            while (n != string::npos) {
                commands[index] = temp.substr(k, n - k);
                k = n + 1;
                n = temp.find(" ", k);
                ++index;
                if (n == string::npos) {
                    commands[index] = temp.substr(k, temp.size() - k);
                    break;
                }
            }
        }
        if (commands[0] == "ls" && commands[1] == "") {
            ls(".");
        } else if (commands[0] == "ls" && commands[1] != "") {
            ls(commands[1]);
        } else if (commands[0] == "cd" && commands[1] == "") {
            cout << "Usage: cd [number]." << endl;
        } else if (commands[0] == "cd" && commands[1] != "") {
            cd(commands[1]);

        } else if (commands[0] == "mv") {
            mv();
        } else if (commands[0] == "q") {
            break;
        }
        commands[1] = "";
    }
}

void ls(string str) {
    char current_dir[MAX_PATH_LEN + 1];
    if (getcwd(current_dir, MAX_PATH_LEN) == NULL) { //현재 디렉토리 임시 저장
        perror("getcwd() error!");
        exit(-1);
    }

    struct stat fileInfo;
    struct passwd *userInfo;
    char cwd[MAX_PATH_LEN + 1];
    char path[MAX_PATH_LEN + 1];

    DIR *dirp;
    struct dirent *dirInfo;
    set<string> temp_dir;  //임시로 디렉토리 저장할 set
    set<string> temp_file; //임시로 파일 저장할 set

    if (isdigit(str[0])) { //만약 ls 뒤에 나오는게 숫자라면 total의 숫자와 맞는 것을 찾아 chdir로 디렉토리 변경
        for (int i = 0; i < total_num; i++) {
            int num = atoi(str.c_str());
            if (total[i].getOrder() == num) {
                string name = total[i].getName();
                strcpy(path, name.c_str()); // string to char
                break;
            }
        }
        dirp = opendir(path);
        if (chdir(path) == -1) {
            perror("chdir() error!(1)");
            exit(-1);
        }
    } else { //ls 뒤에 나온게 숫자가 아니라면 그냥 str을 c타입으로 바꿔서 디렉토리 
        dirp = opendir(str.c_str());
        if (chdir(str.c_str()) == -1) {
            perror("chdir() error!(2)");
            exit(-1);
        }
    }
    if (getcwd(cwd, MAX_PATH_LEN) == NULL) {
        perror("getcwd() error!");
        exit(-1);
    }
    while ((dirInfo = readdir(dirp)) !=
           NULL) { // while문 돌며 디렉토리 정보 읽는다.
        char *name = dirInfo->d_name;

        struct stat fileInfo2;
        stat(name, &fileInfo2);

        int k = fileType(&fileInfo2);
        if (k == DIRECTORY) {
            temp_dir.insert(name);
        } else if (k == FILE) {
            temp_file.insert(name);
        }
        ++total_num; //디렉토리, 파일 전체 몇 개인지 세는 변수
    }

    int dir_num = temp_dir.size();
    set<string>::iterator iter;
    iter = temp_dir.begin();
    total = new Total[total_num];

    for (int i = 0; i < dir_num; ++i) {
        if (iter != temp_dir.end()) {
            total[i].insert(i, *iter, DIRECTORY);
            ++iter;
        }
    }
    iter = temp_file.begin();
    for (int i = dir_num; i < total_num; ++i) {
        if (iter != temp_file.end()) {
            total[i].insert(i, *iter, FILE);
            ++iter;
        }
    }
    for (int i = 0; i < total_num; ++i)
        total[i].print();

    if (chdir(current_dir) == -1) {
        perror("chdir() to current directory!");
        exit(-1);
    }
}

//파일인지 디렉토리인지 구분하는 함수
int fileType(const struct stat *fileInfo) {
    if (S_ISDIR(fileInfo->st_mode)) { //디렉토리이면 DIRECTORY 리턴
        return DIRECTORY;
    } else { //아닐 경우 FILE 리턴
        return FILE;
    }
}

void signalHandler(int signum) {
    if (signum == SIGINT) {
        delete[] total;
        exit(0);
    }
}

void cd(string str) {
    char cwd[MAX_PATH_LEN];
    char dir[MAX_PATH_LEN];
    if (getcwd(cwd, MAX_PATH_LEN) == NULL) {
        perror("getcwd() error!");
        exit(-1);
    }
    // cout << "Current directory" << cwd << endl; // before cd
    int num = atoi(str.c_str());
    for (int i = 0; i < total_num; i++) {
        if (total[i].getOrder() == num) {
            string name = total[i].getName();
            strcpy(dir, name.c_str()); // string to char

            struct stat fileInfo2;
            stat(dir, &fileInfo2);
            int k = fileType(&fileInfo2);

            if (k == DIRECTORY) {
                cout << "go to " << dir << endl;
                if (chdir(dir) == -1) {
                    perror("chdir() error!");
                    exit(-1);
                }
                if (getcwd(cwd, MAX_PATH_LEN) == NULL) {
                    perror("getcwd() error!");
                    exit(-1);
                }
                cout << "Current directory : " << cwd << endl; // after cd
            }

            else {
                cout << "error! It is not Directory!" << endl;
            }
        }
    }
}

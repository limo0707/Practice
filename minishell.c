#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CMD_LEN 100 // 명령어 최대 길이
#define MAX_ARGS 10     // 옵션 최대 개수

// 함수 선언
void print_prompt();
void read_command(char* buffer);
int parse_command(char* buffer, char** args);
void execute_command(char** args, int arg_count);

int main() {
    char command[MAX_CMD_LEN];
    char* args[MAX_ARGS];
    int arg_count;

    printf("--- [My Mini Shell] 시작 ---\n");
    printf("사용 가능한 명령어 : help, ls, touch [파일명], clear, exit\n\n");

    while (1) {
        print_prompt();
        read_command(command);

        arg_count = parse_command(command, args); //문자열 쪼개서 명령어 분석

        if (arg_count == 0) continue; // 입력이 없으면 다시 루프

        if (strcmp(args[0], "exit") == 0) {
            printf("쉘을 종료합니다. 안녕히 가세요!\n");
            break;
        }
        execute_command(args, arg_count);
    }

    return 0;
}

void print_prompt() {
    char cwd[1024]; // getcwd: 현재 작업 경로를 가져오는 시스템 함수
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("MyShell@User:%s$ ", cwd);
    } else {
        printf("MyShell$ ");
    }
}

// 입력 함수 - fgets를 사용하여 공백 포함 입력 받기
void read_command(char* buffer) {
    if (fgets(buffer, MAX_CMD_LEN, stdin) == NULL) {
        exit(0);
    }
    buffer[strcspn(buffer, "\n")] = '\0'; // fgets는 \n까지 저장하므로 이를 \0로 바꿔야 함
}

// 파싱 함수 - 입력된 문장을 공백 기준으로 쪼개서 배열에 저장
int parse_command(char* buffer, char** args) {
    int count = 0;
    char* ptr = strtok(buffer, " "); // 공백을 기준으로 첫 번째 단어 자름

    while (ptr != NULL) {
        if (count >= MAX_ARGS - 1) break;
        args[count++] = ptr;
        ptr = strtok(NULL, " ");
    }
    args[count] = NULL; // 마지막엔 NULL을 넣어 문장의 끝을 알림
    return count;
}

// 실행 함수 - 분석된 명령어를 바탕으로 실제 동작 수행
void execute_command(char** args, int arg_count) {

    if (strcmp(args[0], "help") == 0) {
        printf("--- 도움말 ---\n");
        printf("ls       : 현재 디렉토리 파일 목록 출력\n");
        printf("touch [이름]: 빈 파일 생성\n");
        printf("clear    : 화면 지우기\n");
        printf("exit     : 쉘 종료\n");
    }
    else if (strcmp(args[0], "ls") == 0) {
        system("ls"); 
    }
    else if (strcmp(args[0], "touch") == 0) {
        if (arg_count < 2) {
            printf("사용법: touch [파일명]\n");
        } else {
            FILE* fp = fopen(args[1], "w");
            if (fp != NULL) {
                printf("파일 '%s'가 생성되었습니다.\n", args[1]);
                fclose(fp);
            } else {
                printf("파일 생성 실패!\n");
            }
        }
    }
    else if (strcmp(args[0], "clear") == 0) {
        system("clear");
    }
    else {
        printf("'%s'은(는) 알 수 없는 명령어입니다. 'help'를 입력해보세요.\n", args[0]);
    }
}
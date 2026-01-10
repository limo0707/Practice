#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 학생 데이터를 담을 구조체 정의
typedef struct Student {
    int id;             // 학번
    char name[50];      // 이름
    float score;        // 학점
    struct Student* next; // 다음 학생을 가리키는 포인터
} Student;

Student* head = NULL;

void addStudent();
void deleteStudent();
void printAll();
void saveToFile();
void loadFromFile();
void freeAll();

int main() {
    int choice;
    
    loadFromFile(); // 프로그램 시작 시 파일에서 데이터 불러오기

    while (1) {
        printf("\n--- [학생 성적 관리 시스템] ---\n");
        printf("1) 학생 추가\n");
        printf("2) 학생 삭제\n");
        printf("3) 전체 출력\n");
        printf("4) 저장 및 종료\n");
        printf("선택: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1: addStudent(); break;
        case 2: deleteStudent(); break;
        case 3: printAll(); break;
        case 4:
            saveToFile();
            freeAll(); // 메모리 해제
            printf("프로그램을 종료합니다.\n");
            return 0;
        default: printf("잘못된 입력입니다.\n");
        }
    }
}

// 학생 추가 - 동적 할당 malloc 사용
void addStudent() {
    // 새로운 노드(학생)를 위한 메모리 동적 할당
    Student* newStudent = (Student*)malloc(sizeof(Student));
    if (newStudent == NULL) {
        printf("메모리 할당에 실패하였습니다.\n");
        return;
    }

    printf("학번 입력: ");
    scanf("%d", &newStudent->id);
    printf("이름 입력: ");
    scanf("%s", newStudent->name);
    printf("학점 입력: ");
    scanf("%f", &newStudent->score);
    
    newStudent->next = NULL;

    // 리스트 맨 앞에 추가
    if (head == NULL) {
        head = newStudent;
    } else {
        // 이미 데이터가 있다면 현재 헤드가 가리키는 곳을 새 노드가 가리키게 하고 헤드는 새 노드를 가리킴
        newStudent->next = head;
        head = newStudent;
    }
    printf(">> 학생이 추가되었습니다.\n");
}

// 학생 삭제(포인터 조작)
void deleteStudent() {
    int targetId;
    printf("삭제할 학번 입력: ");
    scanf("%d", &targetId);

    Student* curr = head;
    Student* prev = NULL;

    while (curr != NULL) {
        if (curr->id == targetId) {
            // 삭제 대상 찾음
            if (prev == NULL) {
                // 삭제할 노드가 첫 번째 노드인 경우
                head = curr->next;
            } else {
                // 중간이나 끝 노드인 경우 - 이전 노드의 next를 현재 노드의 다음으로 연결
                prev->next = curr->next;
            }
            free(curr); 메모리 해제
            printf(">> 학번 %d 학생 정보를 삭제했습니다.\n", targetId);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    printf(">> 해당 학번의 학생을 찾을 수 없습니다.\n");
}

// 전체 출력(리스트 순회)
void printAll() {
    Student* curr = head;
    if (curr == NULL) {
        printf(">> 등록된 학생이 없습니다.\n");
        return;
    }
    
    printf("\n[학생 목록]\n");
    while (curr != NULL) {
        printf("학번: %d | 이름: %s | 학점: %.2f\n", curr->id, curr->name, curr->score);
        curr = curr->next;
    }
}

// 파일 저장
void saveToFile() {
    FILE* fp = fopen("students.txt", "w");
    if (fp == NULL) {
        printf("파일 저장 실패\n");
        return;
    }

    Student* curr = head;
    while (curr != NULL) {
        // 텍스트 형태로 저장(학번 이름 학점 순)
        fprintf(fp, "%d %s %f\n", curr->id, curr->name, curr->score);
        curr = curr->next;
    }
    fclose(fp);
    printf(">> 데이터가 students.txt에 저장되었습니다.\n");
}

// 파일 불러오기
void loadFromFile() {
    FILE* fp = fopen("students.txt", "r");
    if (fp == NULL) {
        return;
    }

    while (1) {
        // 임시 공간 할당
        Student* newStudent = (Student*)malloc(sizeof(Student));
        // 파일에서 읽기 시도
        if (fscanf(fp, "%d %s %f", &newStudent->id, newStudent->name, &newStudent->score) == EOF) {
            free(newStudent); // 읽은 게 없으면 할당 해제 후 종료
            break;
        }
        
        // 읽어온 데이터를 리스트에 연결
        newStudent->next = head;
        head = newStudent;
    }
    fclose(fp);
    printf(">> 기존 데이터를 불러왔습니다.\n");
}

// 전체 메모리 해제
void freeAll() {
    Student* curr = head;
    Student* nextNode;

    while (curr != NULL) {
        nextNode = curr->next; // 다음 위치 기억
        free(curr);            // 현재 노드 삭제
        curr = nextNode;       // 이동
    }
    head = NULL;
}
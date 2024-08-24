#ifndef STUDENT_RECORD
#define STUDENT_RECORD

struct Student
{
    int id; 
    char name[25];
    char gender; 
    int age;
    // Login Credentials
    char login[30]; 
    char password[30];
    int status;
};

struct Faculty
{
    int id; 
    char name[25];
    char gender; 
    int age;
    // Login Credentials
    char login[30]; 
    char password[30];
    char department[50];
    
};


struct Course{
    char name[50];
    char department[50];
    int credits;
    int seats;
    int isActive;
    int id;
   
    char courseid[10];
};

struct Enrollment {
    int studentID;
    int courseID;
    int studentname;
    
};
#endif
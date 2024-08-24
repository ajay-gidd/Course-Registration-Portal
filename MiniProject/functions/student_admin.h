#ifndef STUDENTS_FUNCTIONS
#define STUDENTS_FUNCTIONS
#include <sys/ipc.h>
#include <sys/sem.h>


#include"common.h"
#include"useless.h"

struct Student loggedInstudent;
int semIdentifier;
bool student_operation_handler(int connFD);
bool Enroll_Course(int connFD);


bool student_operation_handler(int connFD)
{
    if (login_handler1(false, connFD, &loggedInstudent))
    {
        ssize_t writeBytes, readBytes;           
        char readBuffer[1000], writeBuffer[1000]; 
     
        key_t semKey = ftok("Record_file/Student_file", loggedInstudent.id); 

        union semun
        {
            int val; 
        } semSet;

        int semctlStatus;
        semIdentifier = semget(semKey, 1, 0); 
        if (semIdentifier == -1)
        {
            semIdentifier = semget(semKey, 1, IPC_CREAT | 0700); 
            if (semIdentifier == -1)
            {
                perror("Error while creating semaphore!");
                _exit(1);
            }

            semSet.val = 1; 
            semctlStatus = semctl(semIdentifier, 0, SETVAL, semSet);
            if (semctlStatus == -1)
            {
                perror("Error while initializing a binary sempahore!");
                _exit(1);
            }
        }

        bzero(writeBuffer, sizeof(writeBuffer));
       
        while (1)
        {
            strcat(writeBuffer, "\n");
            strcat(writeBuffer,STUDENT_MENU);
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing CUSTOMER_MENU to client!");
                return false;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            bzero(readBuffer, sizeof(readBuffer));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer));
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for CUSTOMER_MENU");
                return false;
            }
            
            
            int choice = atoi(readBuffer);
            switch (choice)
            {
            case 1:
               view_Course(connFD);
                break;
            case 2:
               Enroll_Course(connFD,-1);
                break;
            case 3:
               //drop(connFD);
                break;
            case 4:
             //view_enroll_course(connFD);
                break;
            case 5:
                //change password(connFD, loggedInCustomer.account);
                break;
            case 6:
                //Logout and Exxit(connFD);
                //break;
            default:
                writeBytes = write(connFD, CUSTOMER_LOGOUT, strlen(CUSTOMER_LOGOUT));
                return false;
            }
        }
    }
    else
    {
       
        return false;
    }
    return true;
}

bool Enroll_Course(int connFD,int id)
{


    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    struct Course course;
    char ch[20]="Enter the id";

    writeBytes = write(connFD, ch, sizeof(ch));

    if (writeBytes == -1)
    {
        perror("Error writing Enter Course id message to client!");
        return false;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading student name response from client!");
        
        return false;
    }
    id=atoi(readBuffer);

     int facultyFileDescriptor = open("Record_file/Course_file", O_RDONLY);
    if (facultyFileDescriptor == -1)
    {
        
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, FACULTY_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing FACULTY_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); 
        return false;

        off_t offset = lseek(facultyFileDescriptor, id * sizeof(struct Course), SEEK_SET);
    if (errno == EINVAL)
    {
      
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, FACULTY_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing FACULTY_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); 
        return false;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required faculty record!");
        return false;
    }

     struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Course), getpid()};

   
      int lockingStatus = fcntl(facultyFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Couldn't obtain lock on faculty record!");
        return false;
    }

    readBytes = read(facultyFileDescriptor, &course, sizeof(struct Course));
    if (readBytes == -1)
    {
        perror("Error while reading faculty record from the file!");
        return false;
    }
     
     lock.l_type = F_UNLCK;
    fcntl(facultyFileDescriptor, F_SETLK, &lock);

    close(facultyFileDescriptor);
  


    facultyFileDescriptor = open("Record_file/Course_file", O_WRONLY);
    if (facultyFileDescriptor == -1)
    {
        perror("Error while opening customer file");
        return false;
    }
    offset = lseek(facultyFileDescriptor, id * sizeof(struct Course), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required faculty record!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(facultyFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on customer record!");
        return false;
    }
    course.seats=course.seats-1;
    writeBytes = write(facultyFileDescriptor, &course, sizeof(struct Course));
    if (writeBytes == -1)
    {
        perror("Error while writing update customer info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(facultyFileDescriptor, F_SETLKW, &lock);

    close(facultyFileDescriptor);
    char sh[10]="Enroll";
    writeBytes = write(connFD, sh, sizeof(sh));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_FACULTY_SUCCESS message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); 

    return true;

    }

}



#endif
#ifndef CUSTOMER_FUNCTIONS
#define CUSTOMER_FUNCTIONS


#include <sys/ipc.h>
#include <sys/sem.h>


#include"common.h"

struct Faculty loggedInCustomer;
int semIdentifier;
bool faculty_operation_handler(int connFD);
int add_Course(int connFD);

bool view_Course(int);

bool faculty_operation_handler(int connFD)
{
    if (login_handler(false, connFD, &loggedInCustomer))
    {
        ssize_t writeBytes, readBytes;            
        char readBuffer[1000], writeBuffer[1000]; 
     
        key_t semKey = ftok("Record_file/Faculty_file", loggedInCustomer.id); 

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
        strcpy(writeBuffer, CUSTOMER_LOGIN_SUCCESS);
        while (1)
        {
            strcat(writeBuffer, "\n");
            strcat(writeBuffer, CUSTOMER_MENU);
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
                add_Course(connFD);
                break;
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


int add_Course(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Course newStudent, previousStudent;
     bzero(readBuffer, sizeof(readBuffer));

    int studentFileDescriptor = open("Record_file/Course_file", O_RDONLY);
    if (studentFileDescriptor == -1 && errno == ENOENT)
    {
        // Customer file was never created
        newStudent.id = 0;
    }
    else if (studentFileDescriptor == -1)
    {
        perror("Error while opening customer file");
        return -1;
    }
    else
    {
        int offset = lseek(studentFileDescriptor, -sizeof(struct Course), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last Student record!");
            return false;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Course), getpid()};
        int lockingStatus = fcntl(studentFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on Student record!");
            return false;
        }

        readBytes = read(studentFileDescriptor, &previousStudent, sizeof(struct Course));
        if (readBytes == -1)
        {
            perror("Error while reading customer file !");
            return false;
        }

        lock.l_type = F_UNLCK;
        fcntl(studentFileDescriptor, F_SETLK, &lock);

        close(studentFileDescriptor);

        newStudent.id = previousStudent.id + 1;
    }

  
        sprintf(writeBuffer, "%s", "Enter the Course Name");

    writeBytes = write(connFD, writeBuffer, sizeof(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing ADMIN_ADD_COURSE_NAME message to client!");
        return false;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading customer name response from client!");
        ;
        return false;
    }

    strcpy(newStudent.name, readBuffer);
   

   char ch[20]="Enter the Course id";
   write(connFD,ch,sizeof(ch));
    bzero(readBuffer, sizeof(readBuffer));
   read(connFD,readBuffer,sizeof(readBuffer));
   strcpy(newStudent.courseid,readBuffer);

    newStudent.seats=150;

    studentFileDescriptor = open("Record_file/Course_file", O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (studentFileDescriptor == -1)
    {
        perror("Error while creating / opening customer file!");
        return false;
    }
    writeBytes = write(studentFileDescriptor, &newStudent, sizeof(newStudent));
    if (writeBytes == -1)
    {
        perror("Error while writing customer file to file!");
        return false;
    }
    
    close(studentFileDescriptor);
    char  su[10]="Success^";
    writeBytes = write(connFD, su, strlen(su));
    if (writeBytes == -1)
    {
        perror("Error sending student loginID and password to the client!");
        return false;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    return newStudent.id;
}





bool view_Course(int connFD)
{

     ssize_t readBytes, writeBytes;             
    char readBuffer[1000], writeBuffer[10000]; 
 

    struct Course course;
    

    FILE *file = fopen("Record_file/Course_file", "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

  

    while (fread(&course, sizeof(struct Course), 1, file) == 1) {
        
    bzero(writeBuffer, sizeof(writeBuffer));
    sprintf(writeBuffer, "Course Details - \n\tID : %d\n\tCourse ID: %s\n\tName : %s\n\tSeats: %d\n", course.id, course.courseid,course.name,course.seats);
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));

   

    }

    fclose(file);

    return 0;


}

#endif
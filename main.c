#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
// Login Username and Password Size Macros.
#define USERNAME_SIZE 20
#define PASSWORD_SIZE  20
// Files Name Macros which acts like a database.
#define MAIN_FILE  "BusSys.bin"
#define TICKETS_FILE_NAME  "Tickets.bin"
// Macros for defining the maximum characters in bus fields.
#define BUS_DESTINATION_SIZE   20
#define BUS_TIME_SIZE   6
// Macros for defining the maximum characters in s_ticket fields.
#define PASSENGER_NAME_SIZE 50
// Macro for defining the size of login data.
#define LOGIN_SIZE  sizeof(s_loginData)
// Login Data Structure. It contains a username and password array of characters,
// with size of last defined macros.
typedef struct
{
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
} s_loginData;
/*
 * A structure that contains the properties of a bus: bus number, number of seats in the bus, free seats, cost of each seat
 * destination, and the leave time.
 */
typedef struct
{
    int bus_No;
    int no_seats;
    int free_seats;
    double cost;
    char bus_Dest[BUS_DESTINATION_SIZE];
    char bus_time[BUS_TIME_SIZE];
} s_bus;
// This structure encapsulate the data of the ticker, it contains the passenger name, seat number,
// and bus number.
typedef struct{
    char passenger_name[PASSENGER_NAME_SIZE];
    int seat_no;
    int bus_no;
} s_ticket;
// This function displays the message in each function
// Every time it clears the screen and print the title of the project with specified message in the argument.
void main_text_message(const char *message)
{
    system("cls"); // Clears the screen.
    printf("\n\t\t\t*********     Bus Reservation System Project in C   *********");
    printf("\n\t\t\t=======================================================================\n");
    printf("\n\t\t\t\t\t\t %s", message); // Prints the message that is passed to the function.
    printf("\n\t\t\t=======================================================================\n");
}
// Welcome function that prints the welcome message to the user when the program firstly run.
void welcome()
{
    printf("\n\t\t\t *************Welcome To Bus Reservation System*************");
    printf("\n\n\n\t\t\t Enter any key to continue login.....");
    getch(); // Make the program pauses until the user hit any key.
}
// This function acts like the core of many functions related to the bus.
// Since most of the functions that acts on bus struct depend mostly on accessing the bus file
// by the bus number, then this function takes tokens to check from where it is called.
// using this token to perform the needed operation on the bus file without having to repeat many boilerplate code.
int search_bus(int busNo, int viewStatus,int checkBusStatus, int destnationStatus, int modifyStatus, int cancel, char* destnation){
    int found = 0;
    s_bus bus = {0}; // Defines a bus variable with type struct of buses.
    FILE *fp = NULL; // Defines a pointer to file and initialize it with null.
    fp = fopen(MAIN_FILE, "rb+"); // Opens the Main file of buses with read and update mode.
    // This if statement checks if there is an error in opening the file.
    if(fp == NULL)
    {
        printf("\t\t\t Error in Opening the File database!!!\n");
        exit(1);
    }
    // This if statement reposition the cursor that read from the file to skip the login header.
    if (fseek(fp, LOGIN_SIZE, SEEK_SET) != 0)
    {
        fclose(fp);
        printf("\n\t\t\tError in Reading the File database\n");
        exit(1);
    }
    // This loop reads from the binary file block by block and process it according to the passed tokens to the function.
    while (fread (&bus, sizeof(bus), 1, fp))
    {
        // This if statement checks if the passed token of view buses is 1, if so it processes the current bus by displaying its data on the scree.
        if (viewStatus == 1){
            printf("\n\t\t\t Bus Number: %d\n\t\t\t", bus.bus_No);
            printf("\t\t\t\tDestination: %s\n\t\t\t", bus.bus_Dest);
            printf("\t\t\t\tLeave Time: %s\n\t\t\t", bus.bus_time);
            printf("\t\t\t\tTicket Cost: %lf\n\t\t\t", bus.cost);
            printf("\t\t\t\tFree Seats: %d\n\t\t\t", bus.free_seats);
            printf("\t\t\t\tTotal Seats: %d\n\t\t\t", bus.no_seats);
            found = 1;
        } else if (checkBusStatus == 1){ // It checks if there are free seats in the bus or not based on the passed token.
            if (bus.bus_No == busNo){
                if (bus.free_seats == 0){
                    printf("\n\t\t\t The Bus is Full!!");
                } else
                    printf("\n\t\t\t Free Seats: %d", bus.free_seats);
                break;
            }
        } else if (destnationStatus == 1){
            // This extracts the the destination string from the struct and sends it by pointers a following function.
            strcpy(destnation, bus.bus_Dest);
        } else if (modifyStatus == 1){
            // Takes the bus number as argument as well as the token of the function, then modifies the bus with passed number.
            if(bus.bus_No == busNo)
            {
                fflush(stdin);
                printf("\n\t\t\tYour Bus data has been found\n");
                found = 1;
                printf("\n\t\t\t Enter Bus Destination: ");
                fflush(stdin);
                fgets(bus.bus_Dest, BUS_DESTINATION_SIZE, stdin);
                printf("\n\t\t\t Enter Bus Leave Time: ");
                fflush(stdin);
                fgets(bus.bus_time, BUS_TIME_SIZE, stdin);
                printf("\n\t\t\t Enter The Ticket Cost in the Bus: ");
                fflush(stdin);
                scanf("%lf", &bus.cost);

                fseek(fp,ftell(fp)-sizeof(bus),0);
                fwrite(&bus, sizeof(bus), 1, fp);
                printf("\n\t\t\t------- Bus Data has been updated successfully -------\n\n");
                break;
            }
        }else if (cancel == 1){
            if (bus.bus_No == busNo){ // By Deleting the ticker the free seats in the bus needs to be increased by one, this statement does so.
                bus.free_seats++;
                fseek(fp,ftell(fp)-sizeof(bus),0);
                fwrite(&bus, sizeof(bus), 1, fp);
                break;
            }
        } else if (bus.bus_No == busNo){ // It makes the main search operation as the default operation.
            found = 1;
            break;
        }
    }
    fclose(fp); // Closes the file after finishing the usage.
    return found;
}
// This function add buses to the file (database).
void add_buses(){
    int status = 0;
    s_bus bus = {0};
    FILE* file_pointer = NULL;
    file_pointer = fopen(MAIN_FILE, "ab+");
    if(file_pointer == NULL)
    {
        printf("\t\t\t Error in Opening the File database!!!\n");
        exit(1);
    }
    main_text_message("Add Buses Information");
    printf("\n\n\t\t\tEnter Bus Details:");
    printf("\n\t\t\t------------------------------------------------------------\n");
    printf("\n\t\t\t Bus Number: ");
    fflush(stdin);
    scanf("%d", &bus.bus_No);
    if (search_bus(bus.bus_No,0,0,0,0,0,"")){
        printf("\n\t\t\t The Bus is already found!");
    } else {
        printf("\n\t\t\t Enter Bus Destination: ");
        fflush(stdin);
        fgets(bus.bus_Dest, BUS_DESTINATION_SIZE, stdin);
        printf("\n\t\t\t Enter Bus Leave Time: ");
        fflush(stdin);
        fgets(bus.bus_time, BUS_TIME_SIZE, stdin);
        printf("\n\t\t\t Enter Total Number of Seats in the Bus: ");
        fflush(stdin);
        scanf("%d", &bus.no_seats);
        printf("\n\t\t\t Enter The Ticket Cost in the Bus: ");
        fflush(stdin);
        scanf("%lf", &bus.cost);
        bus.free_seats = bus.no_seats;
        fwrite(&bus, sizeof(bus), 1, file_pointer);
        fclose(file_pointer);
        printf("\n\t\t\t------- Bus has been added successfully -------");
    }
    printf("\n\t\t\tEnter any key to continue....");
    getch();
}
// This function uses the search function in appropriate way be passing a certain token, that views the bus list in the file.
void view_bus_list(){
    main_text_message("Bus List");
    int found = search_bus(0,1,0,0,0,0,"");
    if(!found)
    {
        printf("\n\t\t\tNo Record has been found");
    }
    printf("\n\t\t\tEnter any key to continue....");
    getch();
}
// This Function is used to modify the information of the bus, using the search function as the last function does.
void modify_bus_info(){
    main_text_message("Bus Information Modification");
    int busNo = 0 ;
    printf("\n\t\t\tEnter Bus Number: ");
    fflush(stdin);
    scanf("%d", &busNo);
    int found = search_bus(busNo,0,0,0,1,0,"");
    if(!found)
    {
        printf("\n\t\t\tNo Record has been found");
    }
    printf("\n\t\t\tEnter any key to continue....");
    getch();
}
/*
 * This function is used to book a ticket, by creating a new file only for tickets, takes the input
 * from the user then books the ticket.
 * the important feature is that it accesses the file of buses and modify the passed bus number
 * to decrease the number of free seats.
 */
void book_ticket(){
    s_bus bus = {0};
    main_text_message("Book Tickets");
    s_loginData fileHeaderInfo = {0};
    FILE* file_pointer = NULL;
    FILE *fp = NULL;
    fp = fopen(MAIN_FILE, "rb+");
    s_ticket tc = {0};
    file_pointer = fopen(TICKETS_FILE_NAME, "ab+");
    fread (&fileHeaderInfo, LOGIN_SIZE, 1, fp);
    if(file_pointer == NULL || fp == NULL)
    {
        printf("\t\t\t Error in Opening the File database!!!\n");
        exit(1);
    }
    int busNo = 0, ticketsNo = 0 ;
    main_text_message("Book Tickets");
    printf("\n\t\t\t Enter Bus Number: ");
    fflush(stdin);
    scanf("%d", &busNo);
    printf("\n\t\t\t Enter Number of Tickets: ");
    fflush(stdin);
    scanf("%d", &ticketsNo);
    // Get Bus Data
    while (fread (&bus, sizeof(bus), 1, fp))
    {
        if(bus.bus_No == busNo)
        {break;}
    }
    if ( bus.free_seats < ticketsNo){
        printf("\n\n\t\t\t There are no enough seats!!!\n");
    } else {
        while (ticketsNo){
            printf("\n\t\t\t Enter Passenger's Name: ");
            fflush(stdin);
            fgets(tc.passenger_name, PASSENGER_NAME_SIZE, stdin);
            tc.seat_no = bus.free_seats;
            tc.bus_no = busNo;
            // Modify Bus free seats
            bus.free_seats--;
            fseek(fp,ftell(fp)-sizeof(bus),0);
            fwrite(&bus, sizeof(bus), 1, fp);
            fwrite(&tc, sizeof(tc), 1, file_pointer);
            printf("\n\t\t\t------- Ticket has been booked successfully -------");
            ticketsNo--;
        }
    }
    fclose(file_pointer);
    fclose(fp);
    printf("\n\t\t\tEnter any key to continue....");
    getch();
}
// This function uses the search function and checks if the bus has free seats or not.
void check_bus_status(){
    main_text_message("Bus Status");
    int busNo = 0 ;
    printf("\n\t\t\t Enter Bus Number: ");
    fflush(stdin);
    scanf("%d", &busNo);
    search_bus(busNo,0,1,0,0,0,"");
    printf("\n\t\t\tEnter any key to continue....");
    getch();
}
// This function mainly deletes a ticket and increases the free seats of the bus by one.
void cancel_booking(){
    main_text_message("Cancel Booking");
    s_ticket tck = {0};
    int found = 0;
    int seat_no = 0 ;
    FILE *fp = NULL;
    FILE *tmpFp = NULL;
    fp = fopen(TICKETS_FILE_NAME, "rb");
    if(fp == NULL)
    {
        printf("\t\t\t Error in Opening the File database!!!\n");
        exit(1);
    }
    tmpFp = fopen("tmp.bin","wb");
    if(tmpFp == NULL)
    {
        fclose(fp);
        printf("\t\t\t Error in Opening the File database!!!\n");
        exit(1);
    }
    printf("\n\t\t\tEnter Seat Number to delete s_ticket: ");
    fflush(stdin);
    scanf("%d", &seat_no);
    while (fread (&tck, sizeof(tck), 1, fp))
    {
        if(tck.seat_no != seat_no)
        {
            fwrite(&tck, sizeof(tck), 1, tmpFp);
        }
        else
        {
            found = 1;
        }
    }
    if (found){
        printf("\n\t\t\tTicket deleted successfully");
        search_bus(tck.bus_no, 0,0,0,0,1,"");
    } else {
        printf("\n\t\t\tNo Record has been found");
    }
    fclose(fp);
    fclose(tmpFp);
    remove(TICKETS_FILE_NAME);
    rename("tmp.bin", TICKETS_FILE_NAME);
    printf("\n\t\t\tEnter any key to continue....");
    getch();
}
// This function displays a list of tickets.
void view_tickets_list(){
    int found = 0;
    main_text_message("Tickets List");
    s_loginData fileHeaderInfo = {0};
    FILE* file_pointer = NULL;
    s_ticket tc = {0};
    file_pointer = fopen(TICKETS_FILE_NAME, "ab+");
    if(file_pointer == NULL)
    {
        printf("\t\t\t Error in Opening the File database!!!\n");
        exit(1);
    }
    while (fread (&tc, sizeof(tc), 1, file_pointer))
    {
        printf("\n\t\t\t Passenger Name: %s\n\t\t\t", tc.passenger_name);
        printf("\t\t\t\tSeat Number: %d\n\t\t\t", tc.seat_no);
        printf("\t\t\t\tBus Number: %d\n\t\t\t", tc.bus_no);
        char destination[BUS_DESTINATION_SIZE];
        search_bus(tc.bus_no,0,0,1,0,0,destination);
        printf("\t\t\t\tBus Destination: %s\n\t\t\t", destination);
        found = 1;
    }
    if(!found){
        printf("\n\t\t\tNo Record has not been found");
    }
    fclose(file_pointer);
    printf("\n\t\t\tEnter any key to continue....");
    getch();
}
// The main menu where you should choose the needed function.
void main_menu(){
    int choice = 0;
    do
    {
        main_text_message("Main Menu");
        printf("\n\n\n\t\t\t1.Add Buses to the System");
        printf("\n\t\t\t2.View Bus List");
        printf("\n\t\t\t3.View Tickets List");
        printf("\n\t\t\t4.Book a Ticket");
        printf("\n\t\t\t5.Check Bus Status");
        printf("\n\t\t\t6.Modify Bus Records");
        printf("\n\t\t\t7.Cancel Booking");
        printf("\n\t\t\t8.Exit");
        printf("\n\n\n\t\t\tEnter choice => ");
        scanf("%d",&choice);
        switch(choice)
        {
            case 1:
                add_buses();
                break;
            case 2:
                view_bus_list();
                break;
            case 3:
                view_tickets_list();
                break;
            case 4:
                book_ticket();
                break;
            case 5:
                check_bus_status();
                break;
            case 6:
                modify_bus_info();
                break;
            case 7:
                cancel_booking();
                break;
            case 8:
                printf("\n\t\t\t ***Bye Bye***");
                exit(1);
            default:
                printf("\n\t\t\tInvalid choice, please try again.");
        }
    }while(choice!=0);
}
// The function which handles the login stuff.
// If the user enters wrong login data for 3 times, the program will give an error and exits.
void login()
{
    main_text_message("Login");
    unsigned char userName[USERNAME_SIZE] = {0};
    unsigned char password[PASSWORD_SIZE] = {0};
    int login_counter=0;
    s_loginData login_header = {0};
    FILE *fp = NULL;
    fp = fopen(MAIN_FILE, "rb");
    if(fp == NULL)
    {
        printf("\t\t\t Error in Opening the File database!!!\n");
        exit(1);
    }
    fread (&login_header, LOGIN_SIZE, 1, fp);
    fclose(fp);
    do
    {
        printf("\n\n\n\t\t\t\tUsername: ");
        fgets(userName, USERNAME_SIZE, stdin);
        printf("\n\t\t\t\tPassword: ");
        fgets(password, PASSWORD_SIZE, stdin);
        if((!strcmp(userName, login_header.username)) && (!strcmp(password, login_header.password)))
        {
            main_menu();
        }
        else
        {
            printf("\t\t\t\tLogin Failed, please try again.\n\n");
            login_counter++;
        }
    }while(login_counter < 3);
    if(login_counter >= 3)
    {
        main_text_message("Login Failed");
        printf("\t\t\t\tError in username or password!! Please, Try Again...");
        getch();
        system("cls");
    }
}
// Checks the existence of an file by passing its path.
int file_exists(const char *path)
{
    FILE *fp = fopen(path, "rb");
    int status = 0;
    if (fp != NULL)
    {
        status = 1;
        fclose(fp);
    }
    return status;
}
// Initialize the file by putting the user name and password in the header.
void init()
{
    FILE *fp = NULL;
    int status = 0;
    const char defaultUsername[] ="zu\n";
    const char defaultPassword[] ="zu\n";
    s_loginData fileHeaderInfo = {0};
    status = file_exists(MAIN_FILE);
    if(!status)
    {
        //Create the binary file
        fp = fopen(MAIN_FILE, "wb");
        if(fp != NULL)
        {
            //Copy default password
            strncpy(fileHeaderInfo.password,defaultPassword,sizeof(defaultPassword));
            strncpy(fileHeaderInfo.username,defaultUsername,sizeof(defaultUsername));
            fwrite(&fileHeaderInfo, LOGIN_SIZE, 1, fp);
            fclose(fp);
        }
    }
}
// The main function, which initiates the program.
int main()
{
    init();
    welcome();
    login();

    return 0;
}

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
using namespace std;
struct Message{
    string sender;
    string reciever;
    int ident;
    string message;
};
struct User{
    string username;
    string password;
};

void LoadMessages(vector<Message>&);
void LoadUsers(vector<User>&);
bool SaveMessages(vector<Message>&);
bool SaveUsers(vector<User>&);
void PrintTitle(string, int);
void Guide(string);
void ExecuteCommand(vector<Message>, vector<User>, string);
int Login(string username, string password);
bool Register(string username, string password);
bool Enter(string username);
bool EnterNew(string username);
bool Leave();
bool New(string message);
bool Edit(int ident, string new_message);
bool Delete(int ident);
void Exit();

//nqma da wipeva redove zasega
int saveProgram(vector<Message>&, ofstream&, vector<User>&, ofstream&);

const string MESSAGE_FILENAME = "messages.txt";
const string USER_FILENAME = "users.txt";
const int MIN_SIZE_MESSAGES = 1024;
const int MIN_SIZE_USERS = 1024;

string MENU_STATE = "";

//window size
const int MAX_COLS = 83; //SMENI SPORED EKRANA
const int CONSOLE_WIDTH = MAX_COLS*2+2;

//console signs
const char UP_RIGHT = (char)187;
const char DOWN_RIGHT = (char)188;
const char DOWN_LEFT = (char)200;
const char UP_LEFT = (char)201;

const char UP_EDGE = (char)203;
const char RIGHT_EDGE = (char)185;
const char DOWN_EDGE = (char)202;
const char LEFT_EDGE = (char)204;

const char HORIZONTAL = (char)205;
const char VERTICAL = '*';

const char MIDDLE = (char)206;

//sled loginvane raboti s vectora, bez faila,
/* LOGIN() i REGISTER() loadvat
EXIT() save-va
*/
int main()
{
    ifstream message_input_file, users_input_file;
    ofstream message_output_file, users_output_file;

    vector<Message> message;
    vector<User> user;
    user.reserve(MIN_SIZE_USERS); //zarezhdaneto e mega bavno, ne mi se razpravq
    message.reserve(MIN_SIZE_MESSAGES);
    LoadMessages(message);
    LoadUsers(user);

    string title = "MOST MARVELOUS MESSENGER [MMM]";
    PrintTitle(title, CONSOLE_WIDTH);

    Guide("LOGIN");
    MENU_STATE = "LOGIN";
    string command;
    while(getline(cin, command, '\n'))
    {
        ExecuteCommand(message, user, command);
    }
    //clearCurrentLine();
    return 0;
}
void LoadMessages(vector<Message> &message)
{
    ifstream message_file;
    Message temp_message;
    string temp_str;
    int message_count=0;
    message_file.open(MESSAGE_FILENAME, ios_base::app);
    while(getline(message_file, temp_str, '\n')) //Each message instance is a line long
    {
        message_count++;
    }
    message_file.close();
    message_file.open(MESSAGE_FILENAME);
    for(int i=0; i<message_count; i++)
    {
        message_file>>temp_message.sender;
        message_file>>temp_message.reciever;
        message_file>>temp_message.ident;
        getline(message_file, temp_message.message, '\n'); //Each message is a line long
        message.push_back(temp_message);
    }
    message_file.close();
}
void LoadUsers(vector<User>& user)
{
    ifstream user_file;
    User temp_user;
    string temp_str;
    int message_count=0;
    user_file.open(MESSAGE_FILENAME, ios_base::app);
    while(getline(user_file, temp_str, '\n')) //Each user instance is a line long
    {
        message_count++;
    }
    user_file.close();
    user_file.open(MESSAGE_FILENAME);
    for(int i=0; i<message_count; i++)
    {
        user_file>>temp_user.username;
        user_file>>temp_user.password;
        user.push_back(temp_user);
    }
    user_file.close();

}
bool SaveMessages(vector<Message> &message)
{
    ofstream message_file;
    message_file.open(MESSAGE_FILENAME, ios_base::trunc);
    for(int i=0; i<message.size(); i++)
    {
        message_file<<message[i].sender<<" "<<message[i].reciever<<" "<<message[i].ident<<" "<<message[i].message<<endl;
    }
    return !(message_file.fail());
}
bool SaveUsers(vector<User> &user)
{
    ofstream user_file;
    user_file.open(USER_FILENAME, ios_base::trunc);
    for(int i=0; i<user.size(); i++)
    {
        user_file<<user[i].username<<" "<<user[i].password<<endl;
    }
    return !(user_file.fail());
}
/*
1 2 2 2 2 2 1
3 0 0 0 0 0 3
3 0 9 9 9 0 3
3 0 0 0 0 0 3
4 2 2 2 2 2 4
*/
/* ima edin mnogo gotin znak - \r, ne zabravqi */
void PrintTitle(string title, int console_size) //работи само на FULLHD, направи аргумент за големина на конзолата мб
{
    int distance = (console_size-title.size()-2)/2;
    /*1 red*/
    cout<<UP_LEFT;
    for(int i=1; i<console_size-1; i++)
    {
        cout<<HORIZONTAL;
    }
    cout<<UP_RIGHT;

    //2 red
    cout<<endl;
    cout<<VERTICAL;

    for(int i=0; i<console_size-2; i++)
    {
        cout<<" ";
    }
    cout<<VERTICAL;

    //3 red
    cout<<endl;
    cout<<VERTICAL;
    for(int i=0; i<distance; i++)
    {
        cout<<" ";
    }
    cout<<title;
    for(int i=0; i<distance; i++)
    {
        cout<<" ";
    }
    cout<<VERTICAL;

    //4 red
    cout<<endl;
    cout<<VERTICAL;
    for(int i=0; i<console_size-2; i++)
    {
        cout<<" ";
    }
    cout<<VERTICAL;

    // 5 red
    cout<<endl;
    cout<<DOWN_LEFT;
    for(int i=1; i<console_size-1; i++)
    {
        cout<<HORIZONTAL;
    }
    cout<<DOWN_RIGHT;
}
void Guide(string menu)
{
    if(menu == "LOGIN")
    {
        cout<<"LOGIN MENU\n";
        cout<<"LOGIN <NAME> <PASSWORD>\n";
        cout<<"REGISTER <NAME> <PASSWORD>\n";
        cout<<"EXIT\n";
    }
    else if(menu == "USERLIST")
    {
        cout<<"USERLIST MENU\n";
        cout<<"ENTER <NAME>\n";
        cout<<"ENTER_NEW <NAME>\n";
        cout<<"LEAVE\n";
    }
    else if(menu == "CHAT")
    {
        cout<<"CHAT MENU\n";
        cout<<"NEW <MESSAGE>\n";
        cout<<"EDIT <IDENTIFICATOR> <MESSAGE>\n";
        cout<<"DELETE <IDENTIFICATOR>\n";
        cout<<"LEAVE\n";
    }
}
void ExecuteCommand(vector<Message> &message, vector<User> &user, string command)
{
    if(MENU_STATE == "LOGIN")
    {
        if(command.find("EXIT") == 0)
        {
            SaveMessages(message);
            SaveUsers(user);
        }
        if(command.find("LOGIN") == 0)
        {
            Login(user, command);
        }
        else if(command.find("REGISTER") == 0)
        {
            Register(user, command);
        }
    }
    else if(MENU_STATE)
    {0

    }
    else if(command.find("ENTER") == 0 AND command.find(" ") == "ENTER".size())
    {
        Enter(command);
    }
    else if(command.find("ENTER_NEW") == 0)
    {
        EnterNew(command);
    }
    else if(command.find("NEW") == 0)
    {
        New(command);
    }
    else if(command.find("EDIT") == 0)
    {
        Edit(command);
    }
    else if(command.find("DELETE") == 0)
    {
        Delete(command);
    }
    else if(command.find("LEAVE") == 0)
    {
        Leave();
    }
}
int Login(vector<User> &users, User user)
{
    for(int i=0; i<users.size(); i++)
    {
        if(users[i].name == user.name)
        {
            if(users[i].password == user.password)
            {
                return 0;
            }
            else
            {
                return 1;
            }
        }
    }
    return 2;
    for(in)
}
bool Delete(vector<Message> &message, int ident);
{
    int message_idx;
    for(int i=0; i<message.size(); i++)
    {
        if(ident == message[i].ident)
        {
            message_idx = message[i]
        }
    }
    ofstream message_file.open(MESSAGE_FILENAME);
}
void Exit(vector<Message> &message, vector<User> &users)
{

    bool success = SaveMessages(message) AND SaveUsers(users);
    if(success)
    {
        exit(EXIT_SUCCESS);
    }
    exit(EXIT_FAILURE);
}

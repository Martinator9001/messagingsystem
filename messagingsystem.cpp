/*
TO-DO:
Намери резолюция на екрана преди да започне програмата
Изчисти кода
Конструктор за структурите
Userlist да проверява дали потребителя съществува в базата данни с потребителите
Update-ни CoutStatus с новите грешки
направи по-малко << операции при сейващите функции
по-добро error handlevane
*/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <string>
// няма namespace std, защото std::messages съществува, което прави конфликт с имената на променливите
struct Message
{
    std::string sender;
    std::string receiver;
    int ident;
    std::string message;
};
struct User
{
    std::string username;
    std::string password;
};
static const char UP_RIGHT = (char)187;
static const char DOWN_RIGHT = (char)188;
static const char DOWN_LEFT = (char)200;
static const char UP_LEFT = (char)201;
static const char UP_EDGE = (char)203;
static const char RIGHT_EDGE = (char)185;
static const char DOWN_EDGE = (char)202;
static const char LEFT_EDGE = (char)204;
static const char HORIZONTAL = (char)205;
static const char VERTICAL = '*';
static const char MIDDLE = (char)206;

enum MENU_STATES
{
    LOGIN = 1,
    USERLIST = 2,
    CHAT = 3
};

enum COMMAND_RETURN_CODES
{
    NO_SUCH_COMMAND = -1,
    COMMAND_SUCCESSFUL = 1,
    INVALID_COMMAND = 2,
    SAVE_FAILED = 100,
    LOAD_FAILED = 101,
    LOGIN_FAILED = 201,
    REGISTER_FAILED = 202,
    ENTER_FAILED = 205,
    ENTER_NEW_FAILED = 206,
    EDIT_MESSAGE_NOT_FOUND = 400,
    EDIT_NOT_ALLOWED = 401,
    DELETE_MESSAGE_NOT_FOUND = 500,
    DELETE_NOT_ALLOWED = 501,
};

static int MENU_STATE = LOGIN;

static const std::string MESSAGE_FILENAME = "messages.txt";
static const std::string USER_FILENAME = "users.txt";
static const int MIN_SIZE_MESSAGES = 1024;
static const int MIN_SIZE_USERS = 1024;

// window size
static const int MAX_COLS = 83; // SMENI SPORED EKRANA
static const int CONSOLE_WIDTH = MAX_COLS * 2 + 2;

bool LoadMessages(std::vector<Message> &);
bool LoadUsers(std::vector<User> &);
bool SaveMessages(std::vector<Message> &);
bool SaveUsers(std::vector<User> &);
void PrintTitle(std::string, int);
void Guide(int);
void ExecuteCommand(std::vector<Message> &, std::vector<User> &, std::string);
bool IsUserInDB(std::vector<User> &, User, bool);
int Login(std::vector<User> &, User);
bool Register(std::vector<User> &, User);
std::vector<std::string> SplitStringIntoTokens(std::string, std::string);
int Enter(std::vector<Message> &, User receiver);
int EnterNew(std::vector<Message> &, User sender, User receiver); // за сега безполезна;
void Leave();

int New(std::vector<Message> &messages, Message message, User sender, User receiver);
int Edit(std::vector<Message> &messages, int ident, Message new_message, User sender, User receiver);
int Delete(std::vector<Message> &messages, int ident, User sender, User receiver);

int Exit(std::vector<Message> &messages, std::vector<User> &users);
int InputCommand(std::vector<Message> &, std::vector<User> &, User &sender, User &receiver, Message &message);
void CoutStatus(int);
void CoutUserlist(std::vector<Message>&, User);
void MessageHistory(std::vector<Message> &, User, User);
// sled loginvane raboti s std::vectora, bez faila,
/* LOGIN() i REGISTER() loadvat
EXIT() save-va
*/
int main()
{
    std::vector<Message> messages;
    std::vector<User> users;
    users.reserve(MIN_SIZE_USERS); // zarezhdaneto e mega bavno, ne mi se razpravq
    messages.reserve(MIN_SIZE_MESSAGES);
    LoadMessages(messages);
    LoadUsers(users);

    std::string title = "MOST MARVELOUS MESSENGER [MMM]";
    PrintTitle(title, CONSOLE_WIDTH);

    Guide(LOGIN);
    MENU_STATE = LOGIN;

    User sender;
    User receiver;
    Message message;
    int status = 1;
    while (status == COMMAND_SUCCESSFUL || status == LOGIN_FAILED || status == REGISTER_FAILED)
    {
        status = InputCommand(messages, users, sender, receiver, message);
    }
    CoutStatus(status);
    return status;
}
bool LoadMessages(std::vector<Message> &messages)
{
    std::ifstream message_file;
    Message temp_message;
    std::string temp_str;
    int message_count = 0;
    message_file.open(MESSAGE_FILENAME, std::ios_base::app);
    while (getline(message_file, temp_str, '\n')) // Each message instance is a line long
    {
        message_count++;
    }
    message_file.close();
    message_file.open(MESSAGE_FILENAME);
    for (size_t i = 0; i < message_count; i++)
    {
        message_file >> temp_message.sender;
        message_file >> temp_message.receiver;
        message_file >> temp_message.ident;
        getline(message_file, temp_message.message, '\n'); // Each message is a line long
        messages.push_back(temp_message);
    }
    message_file.close();
    return !(message_file.fail());
}
bool LoadUsers(std::vector<User> &users)
{
    std::ifstream user_file;
    User temp_user;
    std::string temp_str;
    int message_count = 0;
    user_file.open(MESSAGE_FILENAME, std::ios_base::app);
    while (getline(user_file, temp_str, '\n')) // Each user instance is a line long
    {
        message_count++;
    }
    user_file.close();
    user_file.open(MESSAGE_FILENAME);
    for (size_t i = 0; i < message_count; i++)
    {
        user_file >> temp_user.username;
        user_file >> temp_user.password;
        users.push_back(temp_user);
    }
    user_file.close();
    return !(user_file.fail());
}
bool SaveMessages(std::vector<Message> &message) // препоръчително е да се ползват след всяка операция, но може да е бавно (много << операции)
{
    std::ofstream message_file;
    message_file.open(MESSAGE_FILENAME, std::ios_base::trunc);
    for (size_t i = 0; i < message.size(); i++)
    {
        message_file << message[i].sender << " " << message[i].receiver << " " << message[i].ident << " " << message[i].message << std::endl;
    }
    message_file.close();
    return !(message_file.fail());
}
bool SaveUsers(std::vector<User> &user)
{
    std::ofstream user_file;
    user_file.open(USER_FILENAME, std::ios_base::trunc);
    for (size_t i = 0; i < user.size(); i++)
    {
        user_file << user[i].username << " " << user[i].password << std::endl;
    }
    user_file.close();
    return !(user_file.fail());
}
/*
1 2 2 2 2 2 1
3 0 0 0 0 0 3
3 0 9 9 9 0 3
3 0 0 0 0 0 3
4 2 2 2 2 2 4
*/
void PrintTitle(std::string title, int console_size) // работи само на 1366x768 - селектирай поз
{
    int distance = (console_size - title.size() - 2) / 2;
    /*1 red*/
    std::cout << UP_LEFT;
    for (size_t i = 1; i < console_size - 1; i++)
    {
        std::cout << HORIZONTAL;
    }
    std::cout << UP_RIGHT;

    // 2 red
    std::cout << std::endl;
    std::cout << VERTICAL;

    for (size_t i = 0; i < console_size - 2; i++)
    {
        std::cout << " ";
    }
    std::cout << VERTICAL;

    // 3 red
    std::cout << std::endl;
    std::cout << VERTICAL;
    for (size_t i = 0; i < distance; i++)
    {
        std::cout << " ";
    }
    std::cout << title;
    for (size_t i = 0; i < distance; i++)
    {
        std::cout << " ";
    }
    std::cout << VERTICAL;

    // 4 red
    std::cout << std::endl;
    std::cout << VERTICAL;
    for (size_t i = 0; i < console_size - 2; i++)
    {
        std::cout << " ";
    }
    std::cout << VERTICAL;

    // 5 red
    std::cout << std::endl;
    std::cout << DOWN_LEFT;
    for (size_t i = 1; i < console_size - 1; i++)
    {
        std::cout << HORIZONTAL;
    }
    std::cout << DOWN_RIGHT;
    std::cout<<std::endl;
}
void Guide(int menu)
{
    if (menu == LOGIN)
    {
        std::cout << "Login menu\n";
        std::cout << "LOGIN <NAME> <PASSWORD> - Login as <NAME>\n";
        std::cout << "REGISTER <NAME> <PASSWORD> - Register as <NAME>\n";
        std::cout << "EXIT - Save and exit the program\n";
    }
    else if (menu == USERLIST)
    {
        std::cout << "User list\n";
        std::cout << "ENTER <NAME>\n - Enter a chat with a previous user with name <NAME>";
        std::cout << "ENTER_NEW <NAME>\n - Enter a chat with a new user with name <NAME>";
        std::cout << "LEAVE\n - Go back to the Login menu";
    }
    else if (menu == CHAT)
    {
        std::cout << "Chat menu\n";
        std::cout << "NEW <MESSAGE> - Send a message\n";
        std::cout << "EDIT <IDENTIFICATOR> <MESSAGE> - replace message with identificator <IDENTIFICATOR> with <MESSAGE> (the message must be in this chat)\n";
        std::cout << "DELETE <IDENTIFICATOR> - Delete a message with identificator <IDENTIFICATOR> (the message must be in this chat)\n";
        std::cout << "LEAVE- Go back to the User list\n";
    }
}
// ДЕФАКТО ЕКЗ
int InputCommand(std::vector<Message> &messages, std::vector<User> &users, User &sender, User &receiver, Message &message)
{
    std::string command;
    getline(std::cin, command, '\n');
    std::vector<std::string> command_tokens = SplitStringIntoTokens(command, " "); // няма да работи за команди със съобщения
    int status = 1;
    switch (MENU_STATE)
    {
    case LOGIN:
    {
        Guide(LOGIN);
        if (command_tokens[0] == "EXIT")
        {
            return Exit(messages, users);
        }
        else if (command_tokens[0] == "LOGIN")
        {
            sender.username = command_tokens[1];
            sender.password = command_tokens[2];
            if (Login(users, sender) == COMMAND_SUCCESSFUL)
            {
                MENU_STATE = USERLIST;
                //CoutUserlist(messages, sender);
                return COMMAND_SUCCESSFUL;
            }
            else if (Login(users, sender) == LOGIN_FAILED)
            {
                return LOGIN_FAILED;
            }
        }
        else if (command_tokens[0] == "REGISTER")
        {
            sender.username = command_tokens[1];
            sender.password = command_tokens[2];
            Register(users, sender);
            MENU_STATE = USERLIST;
            //CoutUserlist(messages, sender);
            return COMMAND_SUCCESSFUL;
        }
        break;
    }
    case USERLIST:
    {
        Guide(USERLIST);
        if (command_tokens[0] == "ENTER")
        {
            receiver.username = command_tokens[1];
            //status = Enter(messages, receiver);
            if (status == COMMAND_SUCCESSFUL)
            {
                MessageHistory(messages, sender, receiver);
            }
            return status;
        }
        else if (command_tokens[0] == "ENTER_NEW")
        {
            receiver.username = command_tokens[1];
            return COMMAND_SUCCESSFUL;
        }
        else if (command_tokens[0] == "LEAVE")
        {
            Leave();
            return COMMAND_SUCCESSFUL;
        }
        break;
    }
    case CHAT:
    {
        Guide(CHAT);
        if (command_tokens[0] == "NEW")
        {
            std::string actual_message;
            for (int i = 1; i < command_tokens.size(); i++) // тъй като сплитващата функция ни разделя стринга, съставяме съобщението от разделените стрингове
            {
                actual_message += command_tokens[i] + " ";
            }
            actual_message.pop_back();
            message.message = actual_message;
            New(messages, message, sender, receiver);
        }
        else if (command_tokens[0] == "EDIT")
        {
            int ident = std::stoi(command_tokens[1]);
            std::string actual_message;
            for (int i = 2; i < command_tokens.size(); i++) // тъй като сплитващата функция ни разделя стринга, съставяме съобщението от разделените стрингове
            {
                actual_message += command_tokens[i] + " ";
            }
            actual_message.pop_back();
            message.message = actual_message;
            Edit(messages, ident, message, sender, receiver);
        }
        else if (command_tokens[0] == "DELETE")
        {
            int ident = std::stoi(command_tokens[1]);
            Delete(messages, ident, sender, receiver);
        }
        else if (command_tokens[0] == "LEAVE")
        {
            Leave();
            return COMMAND_SUCCESSFUL;
        }
        break;
    }
    default:
        status = NO_SUCH_COMMAND;
    }
    return status;
}

int Login(std::vector<User> &users, User user)
{
    LoadUsers(users);
    if (IsUserInDB(users, user, true))
    {
        return COMMAND_SUCCESSFUL;
    }
    return LOGIN_FAILED;
}
bool Register(std::vector<User> &users, User user) // bool, ако евевнтуално се намери някаква грешка с register
{
    bool loading_status = LoadUsers(users);
    if (loading_status)
    {
        users.push_back(user);
    }
    else
    {
        CoutStatus(LOAD_FAILED);
    }
    return loading_status;
}
bool IsUserInDB(std::vector<User> &users, User user, bool check_for_pass)
{
    for (size_t i = 0; i < users.size(); i++)
    {
        if (users[i].username == user.username)
        {
            if (check_for_pass && users[i].password == user.password)
            {
                return true;
            }
            return true;
        }
    }
    return false;
}
int New(std::vector<Message> &messages, Message message, User sender, User receiver)
{
    message.sender = sender.username;
    message.receiver = receiver.username;
    message.ident = messages[messages.size() - 1].ident + 1;
    messages.push_back(message);
    return COMMAND_SUCCESSFUL;
}
int Edit(std::vector<Message> &messages, int ident, Message new_message, User sender, User receiver)
{
    for (size_t i = 0; i < messages.size(); i++)
    {
        if (ident == messages[i].ident)
        {
            if (messages[i].sender == sender.username && messages[i].receiver == receiver.username)
            {
                messages[i].message = new_message.message;
                return COMMAND_SUCCESSFUL;
            }
            return EDIT_NOT_ALLOWED;
        }
    }
    return EDIT_MESSAGE_NOT_FOUND;
}
int Delete(std::vector<Message> &messages, int ident, User sender, User receiver)
{
    for (size_t i = 0; i < messages.size(); i++)
    {
        if (ident == messages[i].ident)
        {
            if (messages[i].sender == sender.username && messages[i].receiver == receiver.username)
            {
                messages.erase(messages.begin() + i);
                return COMMAND_SUCCESSFUL;
            }
            return DELETE_NOT_ALLOWED;
        }
    }
    return DELETE_MESSAGE_NOT_FOUND;
}
int Exit(std::vector<Message> &messages, std::vector<User> &users)
{
    if (!SaveMessages(messages) or !SaveUsers(users))
    {
        return SAVE_FAILED;
    }
    else
    {
        return COMMAND_SUCCESSFUL;
    }
}
void CoutStatus(int status)
{
    switch (status)
    {
    case NO_SUCH_COMMAND:
    {
        std::cout << "This command doesn't exist\n";
        break;
    }
    case INVALID_COMMAND:
    {
        std::cout << "You can't use this command in this menu\n";
        break;
    }
    case SAVE_FAILED:
    {
        std::cout << "There was an error while saving the program data to files: " << MESSAGE_FILENAME << " and/or" << USER_FILENAME << std::endl;
        break;
    }
    case LOAD_FAILED:
    {
        std::cout << "Was not able to login" << std::endl;
    }
    case REGISTER_FAILED:
    {
        std::cout << "Was not able to register" << std::endl;
    }
    case ENTER_FAILED:
    {
        std::cout << "There was an error while loading the program data from files: " << MESSAGE_FILENAME << " and/or" << USER_FILENAME << std::endl;
    }
    case ENTER_NEW_FAILED:
    {
        std::cout << "Could not enter a new chat" << std::endl;
    }
    case EDIT_MESSAGE_NOT_FOUND:
    {
        std::cout << "Could not find such message while editing" << std::endl;
    }
    case EDIT_NOT_ALLOWED:
    {
        std::cout << "You are not allowed to edit this message" << std::endl;
        break;
    }
    case DELETE_MESSAGE_NOT_FOUND:
    {
        std::cout << "Could not find such message while deleting" << std::endl;
        break;
    }
    case DELETE_NOT_ALLOWED:
    {
        std::cout << "You are not allowed to delete this message" << std::endl;
        break;
    }
    default:
        break;
    }
}
void Leave()
{
    if (MENU_STATE == USERLIST)
    {
        MENU_STATE = LOGIN;
    }
    else if (MENU_STATE == CHAT)
    {
        MENU_STATE = USERLIST;
    }
}
int Enter(std::vector<User>& users, User receiver)
{
    if (IsUserInDB(users, receiver, false))
    {
        return COMMAND_SUCCESSFUL;
    }
    else
    {
        return ENTER_FAILED;
    }
}
/*int EnterNew(User receiver)
{
    receiver =
}*/
std::vector<std::string> SplitStringIntoTokens(std::string str, std::string delim)
{
    int pos = 0;
    std::vector<std::string> tokens;
    std::string token;
    while (pos = str.find(delim) != std::string::npos)
    {
        token = str.substr(0, pos);
        tokens.push_back(token);
        token.erase(0, pos + delim.size());
    }
    return tokens;
}
void Userlist(std::vector<Message> &messages, User current_user)
{
    bool found_before = false;
    for (int i = 0; i < messages.size(); i++)
    {
        if (messages[i].sender == current_user.username)
        {
            std::string receiver_name = messages[i].receiver;
            for (size_t j = 0; j < i; j++) // Извежда всички потребители към които си изпращал съобщения
            {
                if (messages[j].receiver == receiver_name)
                {
                    found_before = true;
                }
            }
            if (!found_before)
            {
                std::cout << receiver_name << std::endl;
                found_before = true;
            }
        }
        if (messages[i].receiver == current_user.username) // Извежда всички потребители, които са ти изпращали съобщения
        {
            std::string sender_name = messages[i].sender;
            for (size_t j = 0; j < i; j++) // Извежда всички потребители към които си изпращал съобщения
            {
                if (messages[j].sender == sender_name)
                {
                    found_before = true;
                }
            }
            if (!found_before)
            {
                std::cout << sender_name << std::endl;
                found_before = true;
            }
        }
    }
}
void MessageHistory(std::vector<Message> &messages, User current_user, User to_user)
{
    std::cout << "You've entered a chat with " << to_user.username << std::endl;
    for (size_t i = 0; i < messages.size(); i++)
    {
        if (current_user.username == messages[i].sender && to_user.username == messages[i].receiver)
        {
            std::cout << current_user.username << ": " << messages[i].message << std::endl;
        }
        else if (to_user.username == messages[i].sender && current_user.username == messages[i].receiver)
        {
            std::cout << to_user.username << ": " << messages[i].message << std::endl;
        }
    }
}

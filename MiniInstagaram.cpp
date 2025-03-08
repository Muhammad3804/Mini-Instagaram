#include <iostream>
#include <string>
#include <ctime>
#include <windows.h>

using namespace std;

class User;
class UserBST;
class HashTable;

User* usersList = nullptr; //linked list of users
HashTable* loginTable = nullptr; //hash table for login
UserBST* userSearchTree = nullptr; //BST for searching users

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

template <typename T>
class Stack {
public:
    struct Node {
        T data;
        Node* next;
    }*top;

    Stack() : top(nullptr) {}

    void push(T data) {
        Node* newNode = new Node{ data, top };
        top = newNode;
    }

    bool pop(T& data) {
        if (top == nullptr) {
            return false;
        }
        Node* temp = top;
        data = top->data;
        top = top->next;
        delete temp;
        return true;
    }

    bool isEmpty() {
        return top == nullptr;
    }
};

template <typename T>
class Queue {
    struct Node {
        T data;
        Node* next;
    };
    Node* front;
    Node* rear;

public:
    Queue() : front(nullptr), rear(nullptr) {}
    ~Queue() { clear(); }

    bool enqueue(T value) {
        Node* newNode = new Node{ value, nullptr };
        if (rear == nullptr) {
            front = rear = newNode;
        }
        else {
            rear->next = newNode;
            rear = newNode;
        }
        return true;
    }

    bool dequeue(T& value) {
        if (front == nullptr) return false; 
        Node* temp = front;
        value = front->data;
        front = front->next;
        if (front == nullptr) rear = nullptr;
        delete temp;
        return true;
    }

    void clear() {
        while (front != nullptr) {
            Node* temp = front;
            front = front->next;
            delete temp;
        }
        rear = nullptr;
    }

    void display()
    {
        Node* temp = front;
        while (temp != nullptr)
        {
            cout << temp->data << endl;
            temp = temp->next;
        }
    }

    bool isEmpty()
    {
        return front == nullptr;
    }
};

class User {
public:
    string name;
    string password;
    string city;
    time_t lastLoginTimestamp;

    struct Post {
        time_t dateTime;
        string content;
    };

    Stack<Post> postStack; //user's own posts
    Stack<Post> newsfeedStack; //posts from followers

    struct FriendNode {
        User* user;
        string relationType;
        string status;
        FriendNode* next;
    }*friendList;

    Queue<User*> friendRequestQueue;

    struct Conversation {
        User* withUser;
        Stack<string> messageStack;
        Conversation* next;
    }*conversations;

    Queue<string> notificationQueue;

    User* next;

    User(string name, string password, string city) {
        this->name = name;
        this->password = password;
        this->city = city;
        time(&this->lastLoginTimestamp);
        friendList = nullptr;
        conversations = nullptr;
        next = nullptr;
    }

    void addPost(string content);
    void viewOwnPosts();
    void viewNewsfeed();

    void sendFriendRequest(User* toUser);
    void acceptFriendRequest();
    void showPendingFriendRequests();

    void sendMessage(User* toUser, string message);
    void viewMessages(User* withUser);

    void addNotification(string notification);
    void viewNotifications();

    void addFriend(User* newFriend);
    bool isFollowing(User* otherUser);
    void displayFollowers();
};

class HashTable {
public:
    static const int TABLE_SIZE = 100;
    struct HashNode {
        string username;
        User* user;
        HashNode* next;
    }*table[TABLE_SIZE];

    HashTable() {
        for (int i = 0; i < TABLE_SIZE; i++)
            table[i] = nullptr;
    }

    int hashFunction(string username) {
        int sum = 0;
        for (char c : username)
            sum += c;
        return sum % TABLE_SIZE;
    }

    void insert(User* user) {
        int index = hashFunction(user->name);
        HashNode* newNode = new HashNode{ user->name, user, table[index] };
        table[index] = newNode;
    }

    User* search(string username) {
        int index = hashFunction(username);
        HashNode* current = table[index];
        while (current != nullptr) {
            if (current->username == username)
                return current->user;
            current = current->next;
        }
        return nullptr;
    }
};

struct BSTNode {
    string username;
    User* user;
    BSTNode* left;
    BSTNode* right;

    BSTNode(User* user)
    {
        this->username = user->name;
        this->user = user;
        left = right = nullptr;
    }
};

class UserBST {
public:
    BSTNode* root;

    UserBST() : root(nullptr) {}

    void insert(User* user) {
        insert(root, user);
    }

    void insert(BSTNode*& node, User* user) {
        if (node == nullptr) {
            node = new BSTNode(user);
        }
        else if (user->name < node->username) {
            insert(node->left, user);
        }
        else {
            insert(node->right, user);
        }
    }

    User* search(string username) {
        return search(root, username);
    }

    User* search(BSTNode* node, string username) {
        if (node == nullptr)
            return nullptr;
        if (node->username == username)
            return node->user;
        else if (username < node->username)
            return search(node->left, username);
        else
            return search(node->right, username);
    }

    void inorderTraversal() {
        inorderTraversal(root);
    }

    void inorderTraversal(BSTNode* node) {
        if (node != nullptr) {
            inorderTraversal(node->left);
            cout << node->username << endl;
            inorderTraversal(node->right);
        }
    }
};

void User::addPost(string content) {
    Post newPost;
    time(&newPost.dateTime);
    newPost.content = content;
    postStack.push(newPost);

    FriendNode* currentFriend = friendList;
    while (currentFriend != nullptr) {
        if (currentFriend->status == "active") {
            currentFriend->user->newsfeedStack.push(newPost);
        }
        currentFriend = currentFriend->next;
    }
}

void User::viewOwnPosts()
{
    cout << "Your Posts:" << endl;
    Stack<Post> tempStack = postStack;
    Post post;
    while (tempStack.pop(post)) {

        char timeStr[26];
        ctime_s(timeStr, sizeof(timeStr), &post.dateTime);

        cout << "Posted on: " << timeStr;
        cout << post.content << endl << endl;
    }
}

void User::viewNewsfeed()
{
    cout << "Your Newsfeed:" << endl;
    Stack<Post> tempStack = newsfeedStack;
    Post post;
    while (tempStack.pop(post)) {
        char timeStr[26];
        ctime_s(timeStr, sizeof(timeStr), &post.dateTime);

        cout << "Posted on: " << timeStr;
        cout << post.content << endl << endl;
    }
}

void User::sendFriendRequest(User* toUser)
{
    if (toUser == nullptr) {
        cout << "User not found." << endl;
        return;
    }
    if (isFollowing(toUser)) {
        cout << "\nFriend request already sent or already friends." << endl;
        return;
    }
    toUser->friendRequestQueue.enqueue(this);
    cout << "\nFriend request sent to " << toUser->name << endl;
}

void User::acceptFriendRequest() {
    if (friendRequestQueue.isEmpty()) {
        cout << "\nNo pending friend requests." << endl;
        return;
    }
    User* fromUser;
    while (friendRequestQueue.dequeue(fromUser)) {
        addFriend(fromUser);                     
        fromUser->addFriend(this);                
        fromUser->addNotification("Your friend request to " + name + " has been accepted.");
        cout << "\nYou are now friends with " << fromUser->name << endl;
    }
}

void User::showPendingFriendRequests()
{
    if (friendRequestQueue.isEmpty())
    {
        cout << "\nNo pending friend requests." << endl;
        return;
    }
    cout << "\nPending Friend Requests:" << endl;
    friendRequestQueue.display();
}

void User::sendMessage(User* toUser, string message) {
    if (!isFollowing(toUser)) {
        cout << "You can only message to your followers." << endl;
        return;
    }

    //add message to conversation stack
    Conversation* conv = conversations;
    while (conv != nullptr && conv->withUser != toUser)
        conv = conv->next;

    if (conv == nullptr) {
        conv = new Conversation{ toUser, Stack<string>(), conversations };
        conversations = conv;
    }
    conv->messageStack.push("You: " + message);

    //add message to receiver's conversation stack
    Conversation* recvConv = toUser->conversations;
    while (recvConv != nullptr && recvConv->withUser != this)
        recvConv = recvConv->next;

    if (recvConv == nullptr) {
        recvConv = new Conversation{ this, Stack<string>(), toUser->conversations };
        toUser->conversations = recvConv;
    }
    recvConv->messageStack.push(name + ": " + message);

    //add notification to receiver
    toUser->notificationQueue.enqueue("New message from " + name);
}

void User::viewMessages(User* withUser) {
    Conversation* conv = conversations;
    while (conv != nullptr && conv->withUser != withUser)
        conv = conv->next;

    if (conv == nullptr || conv->messageStack.isEmpty()) {
        cout << "\nNo messages with " << withUser->name << endl;
        return;
    }

    cout << "\nMessages from user " << withUser->name << ":" << endl;
    Stack<string> tempStack = conv->messageStack;
    string message;
    while (tempStack.pop(message)) {
        cout << message << endl;
    }
}

void User::addNotification(string notification) {
    notificationQueue.enqueue(notification);
}

void User::viewNotifications() {
    if (notificationQueue.isEmpty()) {
        cout << "\nNo new notifications." << endl;
        return;
    }
    cout << "\nNotifications:" << endl;
    string notification;
    while (notificationQueue.dequeue(notification)) {
        cout << notification << endl;
    }
}

void User::addFriend(User* newFriend) {
    if (newFriend == nullptr) {
        cout << "Error: Trying to add a null friend." << endl;
        return;
    }
    FriendNode* newNode = new FriendNode{ newFriend, "friend", "active", friendList };
    friendList = newNode;
}

bool User::isFollowing(User* otherUser) {
    FriendNode* current = friendList;
    while (current != nullptr) {
        if (current->user == otherUser)
            return true;
        current = current->next;
    }
    return false;
}

void User::displayFollowers() {
    cout << "\nYour Followers:" << endl;
    FriendNode* current = friendList;
    while (current != nullptr) {
        cout << current->user->name << " (" << current->status << ")" << endl;
        current = current->next;
    }
}

void logo()
{
    char a = 220;
    char b = 221;
    char c = 222;
    char d = 223;
    char e = 233;
    cout << "\n\n\n";
    setColor(14);
    cout << "\t" << " INSTAGRAM\n\n";
    setColor(12);
    cout << "\t" << a << a << a << a << a << a << a << a << a << a << a << endl;
    for (int i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            cout << "\t" << b << "\t";
            setColor(7);
            cout << e << " ";
            setColor(12);
            cout << c << endl; continue;
        }
        if (i == 1)
        {
            setColor(9);
            cout << "\t" << b << "  " << a << a << a << a << a << "  " << c << endl;
            cout << "\t" << b << "  " << b << "   " << c << "  " << c << endl;
            cout << "\t" << b << "  " << b << "   " << c << "  " << c << endl;
            setColor(6);
            cout << "\t" << b << "  " << d << d << d << d << d << "  " << c << endl;
        }
        cout << "\t" << b << "\t  " << c << endl;
    }
    cout << "\t" << d << d << d << d << d << d << d << d << d << d << d << endl;


}

void loadingScreen() {
    char a = 177, b = 219;
    cout << "\n\t Loading...\n";
    setColor(8);
    for (int i = 0; i < 25; i++)
    {
        cout << a;
    }
    cout << "\r   ";
    setColor(6);
    for (int i = 0; i < 22; ++i) {
        cout << b;
        Sleep(100);
    }
    cout << endl;
    setColor(7);

}

void signup() {
    system("cls");
    string name, password, city;
    logo();
    setColor(6);
    cout << "\n\n      === Signup ===\n";
    setColor(14);
    cout << "\nEnter username: ";
    setColor(7);
    cin >> name;

    if (loginTable->search(name) != nullptr)
    {
        cout << "Username already exists." << endl;
        system("pause");
        return;
    }
    setColor(14);
    cout << "Enter password (6 characters min): ";
    setColor(7);
    cin >> password;
    if (password.length() < 6)
    {
        cout << "Password too weak." << endl;
        system("pause");
        return;
    }
    setColor(14);
    cout << "Enter city: ";
    setColor(7);
    cin >> city;

    User* newUser = new User(name, password, city);

    newUser->next = usersList;
    usersList = newUser;

    loginTable->insert(newUser);
    userSearchTree->insert(newUser);

    cout << "\nSignup successful!" << endl;
    system("pause");
}

User* login()
{
    system("cls");
    string name, password;
    logo();
    setColor(6);
    cout << "\n\n      --- Login ---\n";
    setColor(14);
    cout << "\nEnter username: ";
    setColor(7);
    cin >> name;
    User* user = loginTable->search(name);
    if (user == nullptr) {
        cout << "\nInvalid username." << endl;
        system("pause");
        return nullptr;
    }

    int attempts = 0;
    bool success = false;
    while (attempts < 3)
    {
        setColor(14);
        cout << "Enter password: ";
        setColor(7);
        cin >> password;
        if (user->password == password)
        {
            success = true;
            break;
        }
        else {
            cout << "\nIncorrect password. Try again." << endl;
            attempts++;
        }
    }

    if (!success)
    {
        cout << "\nToo many failed attempts. Redirecting to main menu." << endl;
        system("pause");
        return nullptr;
    }

    time(&user->lastLoginTimestamp);
    loadingScreen();
    cout << "\nLogin successful!" << endl;
    system("pause");
    return user;
}

void searchUsers() {
    system("cls");
    setColor(6);
    cout << "======= Search Users =======\n";
    setColor(7);
    cout << "Users in the network:" << endl;
    userSearchTree->inorderTraversal();
    system("pause");
}

void menu(User* currentUser)
{

    setColor(12);
    cout << "========== Instagram ==========\n";
    setColor(7);
    cout << "Welcome ";
    setColor(13);
    cout << currentUser->name << endl << endl;
    setColor(14);
    cout << "1. View feed\n";
    cout << "2. View Own Posts\n";
    cout << "3. Add Post\n";
    cout << "4. Send Friend Request\n";
    cout << "5. Accept Friend Requests\n";
    cout << "6. View Notifications\n";
    cout << "7. Send Message\n";
    cout << "8. View Messages\n";
    cout << "9. Search Users\n";
    cout << "10. View Followers List\n";
    cout << "11. Logout\n";
    cout << "\nEnter choice: ";
}

int main()
{
    loginTable = new HashTable();
    userSearchTree = new UserBST();

    int choice;
    User* currentUser = nullptr;
    do
    {
        system("cls");
        logo();
        setColor(8);
        cout << "\n\t1. Sign-up: ";
        cout << "\n\t2. Log-in: ";
        cout << "\n\t0. Exit\n";
        setColor(14);
        cout << "\n      Enter choice: ";
        setColor(7);
        cin >> choice;

        if (choice == 1)
        {
            signup();
        }
        else if (choice == 2)
        {
            currentUser = login();
            while (currentUser != nullptr)
            {
                system("cls");
                menu(currentUser);
                setColor(7);
                cin >> choice;
                cout << endl;

                if (choice == 1)
                {
                    currentUser->viewNewsfeed();
                    system("pause");
                }
                else if (choice == 2)
                {
                    currentUser->viewOwnPosts();
                    system("pause");
                }

                else if (choice == 3)
                {
                    string content;
                    cout << "\nEnter post content: ";
                    cin.ignore();
                    getline(cin, content);
                    currentUser->addPost(content);
                    cout << "Post added." << endl;
                    system("pause");
                }

                else if (choice == 4)
                {
                    string username;
                    cout << "\nEnter username to send friend request: ";
                    cin >> username;
                    User* toUser = loginTable->search(username);
                    currentUser->sendFriendRequest(toUser);
                    system("pause");
                }

                else if (choice == 5)
                {
                    currentUser->showPendingFriendRequests();
                    currentUser->acceptFriendRequest();
                    system("pause");
                }
                else if (choice == 6)
                {
                    currentUser->viewNotifications();
                    system("pause");
                }

                else if (choice == 7)
                {
                    string username;
                    cout << "\nEnter username to message: ";
                    cin >> username;
                    User* toUser = loginTable->search(username);
                    if (toUser == nullptr)
                    {
                        cout << "\nUser not found." << endl;
                    }
                    else
                    {
                        string message;
                        cout << "Enter message: ";
                        cin.ignore();
                        getline(cin, message);
                        currentUser->sendMessage(toUser, message);
                    }
                    system("pause");
                }

                else if (choice == 8)
                {
                    string username;
                    cout << "\nEnter username to view messages: ";
                    cin >> username;

                    User* withUser = loginTable->search(username);
                    if (withUser == nullptr)
                    {
                        cout << "\nUser not found." << endl;
                    }
                    else
                    {
                        currentUser->viewMessages(withUser);
                    }
                    system("pause");
                }
                else if (choice == 9)
                {
                    searchUsers();
                }
                else if (choice == 10)
                {
                    currentUser->displayFollowers();
                    system("pause");
                }
                else if (choice == 11)
                {
                    cout << "Logged out from " << currentUser->name << endl;
                    currentUser = nullptr;
                    system("pause");
                }
                else
                {
                    cout << "\nInvalid choice." << endl;
                    system("pause");
                }
            }
        }
        else if (choice == 0)
        {
            cout << "\nExiting Program" << endl;
        }
        else
        {
            cout << "\nInvalid choice." << endl;
            system("pause");
        }
    } while (choice != 0);

    cout << endl;
    system("pause");
    return 0;
}
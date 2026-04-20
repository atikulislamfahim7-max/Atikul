#include <iostream>
#include <fstream>
#include <string>
using namespace std;
const int MAX = 100;
const int HASH_SIZE = 200;

//Book structure
struct Book
{
    string id;
    string name;
    string author;
    bool issued;
};

//Student struct
struct Student
{
    string id;
    string name;
};

//Linked List of student
struct StudentNode
{
    string id;
    string name;
    StudentNode* next;
};
StudentNode* head = NULL;

//Undo structure
struct undo_data
{
    string action;
    Book oldBook;
    int position;
};

// Hash table struct
struct Hash
{
    string id;
    int index;
    bool occupied = false;
};

//Global arrays
Book books[MAX];
int bookCount = 0;

// Hash table Global
Hash hash_table[HASH_SIZE];

// Simple hash function
int getHash(string id)
{
    unsigned int hash = 0;
    for (char c : id)
    {
        hash = (hash * 31) + c;
    }
    return hash % HASH_SIZE;
}

void func_hash_table()
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        hash_table[i].occupied = false;
    }
    for (int i = 0; i < bookCount; i++)
    {
        int h = getHash(books[i].id);
        while (hash_table[h].occupied)
        {
            h = (h + 1) % HASH_SIZE;
        }
        hash_table[h].id = books[i].id;
        hash_table[h].index = i;
        hash_table[h].occupied = true;
    }
}

//Undo stack
undo_data undo_stack[MAX];
int top = -1;

//push to stack
void push(string action, Book book, int pos)
{
    if (top < MAX - 1)
    {
        top++;
        undo_stack[top].action = action;
        undo_stack[top].oldBook = book;
        undo_stack[top].position = pos;
    }
}

//Undo operation
void undo()
{
    if (top < 0)
    {
        cout << "Nothing to undo" << endl;
        return;
    }

    undo_data last = undo_stack[top];
    top--;

    if (last.action == "add")
    {
        bookCount--;
        cout << "Last added book removed" << endl;
    }
    else if (last.action == "remove")
    {
        for (int i = bookCount; i > last.position; i--)
        {
            books[i] = books[i - 1];
        }
        books[last.position] = last.oldBook;
        bookCount++;
        cout << "Removed book restored" << endl;
    }
    else if (last.action == "update")
    {
        books[last.position] = last.oldBook;
        cout << "Previous book data restored" << endl;
    }

    func_hash_table();

    ofstream file("books.txt");
    for (int i = 0; i < bookCount; i++)
    {
        file << books[i].id << endl;
        file << books[i].name << endl;
        file << books[i].author << endl;
        file << books[i].issued << endl;
    }
    file.close();
}

//save books to file
void saveBooks()
{
    ofstream file("books.txt");
    for (int i = 0; i < bookCount; i++)
    {
        file << books[i].id << endl;
        file << books[i].name << endl;
        file << books[i].author << endl;
        file << books[i].issued << endl;
    }
    file.close();
}

//load books from file
void Load_book()
{
    ifstream file("books.txt");
    while (file >> books[bookCount].id)
    {
        file.ignore();
        getline(file, books[bookCount].name);
        getline(file, books[bookCount].author);
        file >> books[bookCount].issued;
        bookCount++;
    }
    file.close();
    func_hash_table();
}

//Load students into linked list
void load_students_list()
{
    ifstream file("students.txt");
    string id, name;

    while (file >> id)
    {
        file.ignore();
        getline(file, name);

        StudentNode* newNode = new StudentNode;
        newNode->id = id;
        newNode->name = name;
        newNode->next = head;
        head = newNode;
    }
    file.close();
}

//Student registration
void Student_regi()
{
    ofstream file("students.txt", ios::app);
    string id, name;

    cout << "Enter student id: ";
    cin >> id;
    cin.ignore();
    cout << "Enter name: ";
    getline(cin, name);

    file << id << endl;
    file << name << endl;
    file.close();

    //Insert into linked list
    StudentNode* newNode = new StudentNode;
    newNode->id = id;
    newNode->name = name;
    newNode->next = head;
    head = newNode;
}

//Add book
void addBook()
{
    if (bookCount >= MAX)
    {
        cout << "Storage Full! " << endl;
        return;
    }

    cout << "Enter book ID: ";
    cin >> books[bookCount].id;
    cin.ignore();
    cout << "Enter book name: ";
    getline(cin, books[bookCount].name);
    cout << "Enter author name: ";
    getline(cin, books[bookCount].author);

    books[bookCount].issued = false;

    push("add", books[bookCount], bookCount);
    bookCount++;
    func_hash_table();
    saveBooks();
}

//Remove book
void removeBook()
{
    string id;
    cout << "Enter book ID for remove: ";
    cin >> id;

    for (int i = 0; i < bookCount; i++)
    {
        if (books[i].id == id)
        {
            push("remove", books[i], i);

            for (int j = i; j < bookCount - 1; j++)
            {
                books[j] = books[j + 1];
            }

            bookCount--;
            func_hash_table();
            saveBooks();
            cout << "Removed! " << endl;
            return;
        }
    }
    cout << "Book not found" << endl;
}

//Search by book id using hash table
void search_by_id()
{
    string id;
    cout << "Enter Book ID: ";
    cin >> id;

    int h = getHash(id);
    int startH = h;

    while (hash_table[h].occupied)
    {
        if (hash_table[h].id == id)
        {
            int idx = hash_table[h].index;
            cout << books[idx].name << " - " << books[idx].author << endl;
            return;
        }
        h = (h + 1) % HASH_SIZE;
        if (h == startH) break;
    }

    cout << "Not found" << endl;
}

//Search by book name using binary search
void search_by_name()
{
    string name;
    cin.ignore();
    cout << "Enter Book Name: ";
    getline(cin, name);

    //Sort by name (bubble sort)
    for (int i = 0; i < bookCount - 1; i++)
    {
        for (int j = 0; j < bookCount - i - 1; j++)
        {
            if (books[j].name > books[j + 1].name)
            {
                swap(books[j], books[j + 1]);
            }
        }
    }

    //Binary search
    int left = 0, right = bookCount - 1;
    while (left <= right)
    {
        int mid = (left + right) / 2;

        if (books[mid].name == name)
        {
            cout << books[mid].name << " - " << books[mid].author << endl;
            return;
        }
        else if (books[mid].name < name)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }
    cout << "Not found" << endl;
}

//Display books
void displayBooks()
{
    for (int i = 0; i < bookCount; i++)
    {
        cout << books[i].id << ". " << books[i].name << " - " << books[i].author << endl;
    }
}

//Display students using linked list
void display_student()
{
    cout << "Student registered: " << endl;

    StudentNode* temp = head;

    while (temp != NULL)
    {
        cout << temp->id << ". " << temp->name << endl;
        temp = temp->next;
    }
}

//Update book
void updateBook()
{
    string id;
    cout << "Enter book ID: ";
    cin >> id;
    cin.ignore();

    for (int i = 0; i < bookCount; i++)
    {
        if (books[i].id == id)
        {
            push("update", books[i], i);

            cout << "New name: ";
            getline(cin, books[i].name);
            cout << "New author name: ";
            getline(cin, books[i].author);
            saveBooks();
            return;
        }
    }
    cout << "Not found" << endl;
}

//Issue book
void issueBook()
{
    string book_id, student_ID;

    cout << "Enter Book ID: ";
    cin >> book_id;

    cout << "Enter Student ID: ";
    cin >> student_ID;

    for (int i = 0; i < bookCount; i++)
    {
        if (books[i].id == book_id && !books[i].issued)
        {
            books[i].issued = true;
            saveBooks();

            ofstream file("issue.txt", ios::app);
            file << book_id << " " << student_ID << endl;
            file.close();

            cout << "Issue successful! " << endl;
            return;
        }
    }
    cout << "Not available" << endl;
}

//Return book
void returnBook()
{
    string book_id, student_ID;

    cout << "Enter book ID: ";
    cin >> book_id;
    cout << "Enter student ID: ";
    cin >> student_ID;

    bool found = false;

    for (int i = 0; i < bookCount; i++)
    {
        if (books[i].id == book_id && books[i].issued)
        {
            books[i].issued = false;
            found = true;
            break;
        }
    }

    if (!found)
    {
        cout << "Return can't be done. Something wrong." << endl;
        return;
    }

    saveBooks();

    ifstream file("issue.txt");
    ofstream temp("temp.txt");

    string b_id, stu_id;

    while (file >> b_id >> stu_id)
    {
        if (!(b_id == book_id && stu_id == student_ID))
        {
            temp << b_id << " " << stu_id << endl;
        }
    }

    file.close();
    temp.close();

    remove("issue.txt");
    rename("temp.txt", "issue.txt");

    cout << "Return Successful! " << endl;
}

//Bubble sort
void bubble_sort()
{
    for (int i = 0; i < bookCount - 1; i++)
    {
        for (int j = 0; j < bookCount - i - 1; j++)
        {
            if (books[j].id > books[j + 1].id)
            {
                swap(books[j], books[j + 1]);
            }
        }
    }
    func_hash_table();
    cout << "Sorted by ID" << endl;
}

int main()
{
    Load_book();
    load_students_list();
    int choice;
    while (true)
    {
        cout << endl;
        cout << "1. Register Student" << endl;
        cout << "2. Add book" << endl;
        cout << "3. Remove book" << endl;
        cout << "4. Search book by ID" << endl;
        cout << "5. Search book by name" << endl;
        cout << "6. Display Books" << endl;
        cout << "7. Display Students" << endl;
        cout << "8. Update Book" << endl;
        cout << "9. Undo" << endl;
        cout << "10. Issue Book" << endl;
        cout << "11. Return Book" << endl;
        cout << "12. Sort Books" << endl;
        cout << "13. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1)
        {
            Student_regi();
        }
        else if (choice == 2)
        {
            addBook();
        }
        else if (choice == 3)
        {
            removeBook();
        }
        else if (choice == 4)
        {
            search_by_id();
        }
        else if (choice == 5)
        {
            search_by_name();
        }
        else if (choice == 6)
        {
            displayBooks();
        }
        else if (choice == 7)
        {
            display_student();
        }
        else if (choice == 8)
        {
            updateBook();
        }
        else if (choice == 9)
        {
            undo();
        }
        else if (choice == 10)
        {
            issueBook();
        }
        else if (choice == 11)
        {
            returnBook();
        }
        else if (choice == 12)
        {
            bubble_sort();
        }
        else if (choice == 13)
        {
            break;
        }
        else
        {
            cout << "Invalid choice" << endl;
        }
    }
}


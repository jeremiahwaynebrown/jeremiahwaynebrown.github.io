/*
    Author: Jeremiah Brown
    Date: 2/23/25
    Enhanced for CS-499

    Scenario:
    The academic advisors in the Computer Science department at ABCU are very happy
    with the planning and design you have completed for the advising assistance software.
    Now, they would like the application to be more robust, efficient, and maintainable.

    Enhancements made:
    - Added validation while loading CSV data
    - Normalized course numbers to uppercase
    - Added BST destructor for memory cleanup
    - Added unordered_map for faster O(1) course lookup
    - Added prerequisite validation
    - Improved error handling and user feedback
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <limits>
#include <set>

using namespace std;

// Structure to store course details
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Node structure for BST
struct Node {
    Course course;
    Node* left;
    Node* right;

    Node(Course c) : course(c), left(nullptr), right(nullptr) {}
};

// Helper function to normalize course numbers to uppercase
string normalizeCourseNumber(string courseNumber) {
    transform(courseNumber.begin(), courseNumber.end(), courseNumber.begin(),
        [](unsigned char c) { return static_cast<char>(toupper(c)); });
    return courseNumber;
}

// Binary Search Tree class
class BST {
private:
    Node* root;

    // Helper function to insert a course into the BST
    void insert(Node*& node, const Course& course) {
        if (!node) {
            node = new Node(course);
        }
        else if (course.courseNumber < node->course.courseNumber) {
            insert(node->left, course);
        }
        else if (course.courseNumber > node->course.courseNumber) {
            insert(node->right, course);
        }
        // Duplicate course numbers are ignored in BST
    }

    // Helper function to print courses in alphanumeric order
    void printInOrder(Node* node) const {
        if (node) {
            printInOrder(node->left);
            cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
            printInOrder(node->right);
        }
    }

    // Helper function to destroy BST and free memory
    void destroyTree(Node* node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

public:
    BST() : root(nullptr) {}

    ~BST() {
        destroyTree(root);
        root = nullptr;
    }

    // Public method to insert a course into the BST
    void insert(const Course& course) {
        insert(root, course);
    }

    // Public method to print all courses in alphanumeric order
    void printAllCourses() const {
        printInOrder(root);
    }
};

// Function to load course data from a file into the BST and hash map
bool loadCourses(const string& filename, BST& bst, unordered_map<string, Course>& courseMap) {
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Error: Unable to open file!" << endl;
        return false;
    }

    vector<Course> tempCourses;
    unordered_set<string> courseNumbers;
    string line;
    int lineNumber = 0;

    // First pass: read and validate structure of each row
    while (getline(file, line)) {
        lineNumber++;

        if (line.empty()) {
            cout << "Warning: Skipping empty line " << lineNumber << "." << endl;
            continue;
        }

        stringstream ss(line);
        string courseNumber;
        string courseTitle;
        string prerequisite;
        vector<string> prerequisites;

        getline(ss, courseNumber, ',');
        getline(ss, courseTitle, ',');

        courseNumber = normalizeCourseNumber(courseNumber);

        // Basic validation for required fields
        if (courseNumber.empty() || courseTitle.empty()) {
            cout << "Warning: Skipping malformed line " << lineNumber
                << " due to missing course number or title." << endl;
            continue;
        }

        // Read prerequisites
        while (getline(ss, prerequisite, ',')) {
            prerequisite = normalizeCourseNumber(prerequisite);
            if (!prerequisite.empty()) {
                prerequisites.push_back(prerequisite);
            }
        }

        // Prevent duplicate course numbers
        if (courseNumbers.find(courseNumber) != courseNumbers.end()) {
            cout << "Warning: Duplicate course number '" << courseNumber
                << "' found on line " << lineNumber << ". Skipping duplicate." << endl;
            continue;
        }

        Course course{ courseNumber, courseTitle, prerequisites };
        tempCourses.push_back(course);
        courseNumbers.insert(courseNumber);
    }

    file.close();

    // Second pass: validate prerequisites and load into structures
    for (const Course& course : tempCourses) {
        bool validCourse = true;

        for (const string& prereq : course.prerequisites) {
            if (courseNumbers.find(prereq) == courseNumbers.end()) {
                cout << "Warning: Course " << course.courseNumber
                    << " has invalid prerequisite '" << prereq
                    << "'. This course will still be loaded, but the prerequisite reference is invalid."
                    << endl;
            }

            // Prevent self-referencing prerequisite
            if (prereq == course.courseNumber) {
                cout << "Warning: Course " << course.courseNumber
                    << " cannot list itself as a prerequisite." << endl;
                validCourse = false;
            }
        }

        if (validCourse) {
            bst.insert(course);
            courseMap[course.courseNumber] = course;
        }
    }

    cout << "Course data loaded successfully!" << endl;
    return true;
}

bool hasCycle(const string& courseNumber,
    const unordered_map<string, Course>& courseMap,
    unordered_set<string>& visited,
    unordered_set<string>& recursionStack);

void printPrereqChain(const string& courseNumber,
    const unordered_map<string, Course>& courseMap,
    unordered_set<string>& printed);

bool hasCycle(const string& courseNumber,
    const unordered_map<string, Course>& courseMap,
    unordered_set<string>& visited,
    unordered_set<string>& recursionStack) {
    if (recursionStack.find(courseNumber) != recursionStack.end()) {
        return true;
    }

    if (visited.find(courseNumber) != visited.end()) {
        return false;
    }

    visited.insert(courseNumber);
    recursionStack.insert(courseNumber);

    auto it = courseMap.find(courseNumber);
    if (it != courseMap.end()) {
        for (const string& prereq : it->second.prerequisites) {
            if (hasCycle(prereq, courseMap, visited, recursionStack)) {
                return true;
            }
        }
    }

    recursionStack.erase(courseNumber);
    return false;
}

void printPrereqChain(const string& courseNumber,
    const unordered_map<string, Course>& courseMap,
    unordered_set<string>& printed) {
    auto it = courseMap.find(courseNumber);
    if (it == courseMap.end()) {
        return;
    }

    for (const string& prereq : it->second.prerequisites) {
        if (printed.find(prereq) == printed.end()) {
            cout << "-> " << prereq << endl;
            printed.insert(prereq);
            printPrereqChain(prereq, courseMap, printed);
        }
    }
}

int main() {
    BST bst;
    unordered_map<string, Course> courseMap;

    int choice;
    string filename, courseNumber;
    bool dataLoaded = false;

    cout << "Welcome to the course planner." << endl;

    do {
        // Display menu options
        cout << "\n1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "4. Print Course Dependency Chain." << endl;
        cout << "9. Exit." << endl;
        cout << "\nWhat would you like to do? ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            if (dataLoaded) {
                cout << "Data already loaded!" << endl;
            }
            else {
                filename = "CS 300 ABCU_Advising_Program_Input.csv";
                dataLoaded = loadCourses(filename, bst, courseMap);
            }
            break;

        case 2:
            if (!dataLoaded) {
                cout << "Error: Load course data first (Option 1)." << endl;
            }
            else {
                bst.printAllCourses();
            }
            break;

        case 3:
            if (!dataLoaded) {
                cout << "Error: Load course data first (Option 1)." << endl;
            }
            else {
                cout << "What course do you want to know about? ";
                cin >> courseNumber;
                courseNumber = normalizeCourseNumber(courseNumber);

                auto it = courseMap.find(courseNumber);
                if (it != courseMap.end()) {
                    const Course& course = it->second;
                    cout << course.courseNumber << ", " << course.courseTitle << endl;

                    if (!course.prerequisites.empty()) {
                        cout << "Prerequisites: ";
                        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
                            cout << course.prerequisites[i];
                            if (i < course.prerequisites.size() - 1) {
                                cout << ", ";
                            }
                        }
                        cout << endl;
                    }
                    else {
                        cout << "Prerequisites: None" << endl;
                    }
                }
                else {
                    cout << "Course not found!" << endl;
                }
            }
            break;

        case 4:
            if (!dataLoaded) {
                cout << "Error: Load course data first (Option 1)." << endl;
            }
            else {
                cout << "Enter course number to view full dependency chain: ";
                cin >> courseNumber;
                courseNumber = normalizeCourseNumber(courseNumber);

                if (courseMap.find(courseNumber) == courseMap.end()) {
                    cout << "Course not found!" << endl;
                }
                else {
                    unordered_set<string> visited;
                    unordered_set<string> recursionStack;

                    if (hasCycle(courseNumber, courseMap, visited, recursionStack)) {
                        cout << "Warning: Circular dependency detected for "
                            << courseNumber << "." << endl;
                    }
                    else {
                        cout << "Full prerequisite chain for " << courseNumber << ":" << endl;
                        unordered_set<string> printed;
                        printPrereqChain(courseNumber, courseMap, printed);

                        if (printed.empty()) {
                            cout << "This course has no prerequisites." << endl;
                        }
                    }
                }
            }
            break;

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << "Invalid option. Please try again." << endl;
            break;
        }

    } while (choice != 9);

    return 0;
}
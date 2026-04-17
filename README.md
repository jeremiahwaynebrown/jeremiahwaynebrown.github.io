# Artifact 2 – Course Planner

## Overview
This project is a course planning application developed in C++. It allows users to load course data, search for courses, and analyze prerequisite relationships.

Originally developed in a data structures course, this artifact was enhanced to demonstrate advanced algorithmic concepts and improved data handling.

---

## Original Functionality
The application allowed users to:
- Load course data from a CSV file
- Print a list of all courses
- Search for a specific course
- Display direct prerequisites

---

## Enhancements

### 1. Hash Map Integration
A hash map (`unordered_map`) was added to:
- Improve lookup performance
- Enable constant-time course retrieval

---

### 2. Recursive Prerequisite Chain Traversal
A recursive function was implemented to:
- Display the full prerequisite chain for a course
- Traverse dependencies beyond just direct prerequisites

---

### 3. Circular Dependency Detection
A cycle detection algorithm was added using:
- A visited set
- A recursion stack

This prevents invalid course structures and demonstrates graph-based logic.

---

### 4. Input Validation and Normalization
Enhancements include:
- Normalizing course IDs (uppercase consistency)
- Validating CSV input data
- Handling missing or malformed data

---

## Skills Demonstrated
- Algorithm design and analysis
- Recursion and graph traversal
- Data structures (hash maps, trees)
- Input validation and error handling

---

## How to Run
1. Compile the program using a C++ compiler
2. Run the executable
3. Follow the menu prompts

Example:
- Load data
- Search for a course
- View dependency chain

---

## Sample Data
The application uses a CSV file containing:
- Course ID
- Course name
- Prerequisites

---

## Author
Jeremy Brown  
Southern New Hampshire University  
CS-499 Capstone

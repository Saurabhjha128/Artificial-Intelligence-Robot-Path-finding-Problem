#ifndef HEURISTICS_H
#define HEURISTICS_H

#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <string>

using namespace std;

struct Room {
    int id;
    string name;
    int capacity;
    vector<int> busy_slots;
};

struct Teacher {
    int id;
    string name;
    vector<int> busy_slots;
};

struct Course {
    int id;
    string name;
    int teacher_id;
    int students;
    vector<int> student_groups;
};

struct Assignment {
    int timeslot;
    int room;
    
    Assignment() : timeslot(-1), room(-1) {}
    Assignment(int t, int r) : timeslot(t), room(r) {}
};

struct Variable {
    int course_id;
    int timeslot;
    int room;
};

struct Value {
    int timeslot;
    int room;
    int course_id;
    
    Value(int t, int r, int cid = -1) : timeslot(t), room(r), course_id(cid) {}
};

typedef vector<Assignment> TimetableAssignment;

struct TimetableProblem {
    vector<Course> courses;
    vector<Teacher> teachers;
    vector<Room> rooms;
    int timeslots;
    
    TimetableProblem() : timeslots(8) {}
};

// Helper function to check if two courses share student groups
bool shareStudentGroups(const Course& c1, const Course& c2) {
    for (int group1 : c1.student_groups) {
        for (int group2 : c2.student_groups) {
            if (group1 == group2) {
                return true;
            }
        }
    }
    return false;
}

// Variable ordering: Minimum Remaining Values (MRV)
int selectUnassignedVariableMRV(const TimetableProblem& problem, 
                               const TimetableAssignment& assignment,
                               const vector<vector<bool>>& domain) {
    int min_remaining = problem.timeslots * problem.rooms.size() + 1;
    int selected_course = -1;
    
    for (int i = 0; i < problem.courses.size(); ++i) {
        if (assignment[i].timeslot != -1) continue; // Skip assigned courses
        
        int remaining = 0;
        for (int t = 0; t < problem.timeslots; ++t) {
            for (int r = 0; r < problem.rooms.size(); ++r) {
                if (domain[i][t * problem.rooms.size() + r]) {
                    remaining++;
                }
            }
        }
        
        if (remaining < min_remaining) {
            min_remaining = remaining;
            selected_course = i;
        }
    }
    
    return selected_course;
}

// Value ordering: Least Constraining Value
vector<Value> orderDomainValues(const TimetableProblem& problem, 
                               int course_id, 
                               const vector<vector<bool>>& domain) {
    vector<Value> values;
    const Course& course = problem.courses[course_id];
    
    // Collect all valid values
    for (int t = 0; t < problem.timeslots; ++t) {
        for (int r = 0; r < problem.rooms.size(); ++r) {
            if (domain[course_id][t * problem.rooms.size() + r]) {
                values.push_back(Value(t, r, course_id));
            }
        }
    }
    
    // Sort by how constraining they are (least constraining first)
    // For simplicity, we'll use a basic ordering - you can enhance this
    sort(values.begin(), values.end(), [](const Value& a, const Value& b) {
        return a.timeslot < b.timeslot; // Simple time-based ordering
    });
    
    return values;
}

TimetableProblem createRandomTimetableProblem() {
    TimetableProblem problem;
    problem.timeslots = 8; // Reduced for easier solving
    
    random_device rd;
    mt19937 gen(rd());
    
    // Create rooms
    uniform_int_distribution<> room_cap(25, 50);
    for (int i = 0; i < 4; ++i) { // Reduced rooms
        problem.rooms.push_back({i, "Room " + to_string(i), room_cap(gen), {}});
    }
    
    // Create teachers
    for (int i = 0; i < 6; ++i) { // Reduced teachers
        // Fewer busy slots to make problem easier
        vector<int> busy_slots;
        uniform_int_distribution<> busy_count(0, 2);
        uniform_int_distribution<> slot_dist(0, problem.timeslots - 1);
        
        int num_busy = busy_count(gen);
        for (int j = 0; j < num_busy; ++j) {
            busy_slots.push_back(slot_dist(gen));
        }
        
        problem.teachers.push_back({i, "Teacher " + to_string(i), busy_slots});
    }
    
    // Create courses - fewer and with smaller student counts
    uniform_int_distribution<> student_dist(15, 35);
    uniform_int_distribution<> teacher_dist(0, problem.teachers.size() - 1);
    uniform_int_distribution<> group_dist(0, 3); // Fewer student groups
    
    int num_courses = 8; // Reduced number of courses
    for (int i = 0; i < num_courses; ++i) {
        Course course;
        course.id = i;
        course.name = "Course " + to_string(i);
        course.teacher_id = teacher_dist(gen);
        course.students = student_dist(gen);
        
        // Add 1-2 student groups
        int num_groups = uniform_int_distribution<>(1, 2)(gen);
        for (int j = 0; j < num_groups; ++j) {
            course.student_groups.push_back(group_dist(gen));
        }
        
        problem.courses.push_back(course);
    }
    
    return problem;
}

TimetableProblem createStandardTimetableProblem() {
    TimetableProblem problem;
    problem.timeslots = 8; // 8 timeslots
    
    // Create rooms with higher capacities
    problem.rooms = {
        {0, "Room A", 40, {1, 3}}, // Busy at timeslots 1 and 3
        {1, "Room B", 35, {0, 4}},
        {2, "Room C", 50, {2}},
        {3, "Room D", 45, {5, 7}}
    };
    
    // Create teachers with fewer conflicts
    problem.teachers = {
        {0, "Dr. Smith", {0, 4}},    // Busy at timeslots 0 and 4
        {1, "Dr. Johnson", {1}},
        {2, "Dr. Williams", {2, 6}},
        {3, "Dr. Brown", {3}},
        {4, "Dr. Davis", {5}},
        {5, "Dr. Miller", {7}}
    };
    
    // Create courses with fewer student group conflicts
    problem.courses = {
        {0, "AI", 0, 25, {0, 1}},
        {1, "Database", 1, 30, {0, 2}},
        {2, "Networks", 2, 20, {1, 3}},
        {3, "OS", 3, 35, {2}},
        {4, "Security", 4, 28, {0, 3}},
        {5, "ML", 5, 22, {1}},
        {6, "Web Dev", 0, 32, {2, 3}}, // Same teacher as AI but different groups
        {7, "Mobile App", 1, 26, {1, 2}}
    };
    
    return problem;
}

#endif
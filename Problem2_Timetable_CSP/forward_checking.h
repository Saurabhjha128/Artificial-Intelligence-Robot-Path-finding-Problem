#ifndef FORWARD_CHECKING_H
#define FORWARD_CHECKING_H

#include <vector>
#include <algorithm>
#include <iostream>
#include "heuristics.h"

using namespace std;

class ForwardCheckingSolver {
private:
    TimetableProblem problem;
    TimetableAssignment assignment;
    vector<vector<bool>> domain;
    int backtrack_count;
    int assignment_count;
    
    void initializeDomain() {
        domain.resize(problem.courses.size());
        for (int i = 0; i < problem.courses.size(); ++i) {
            domain[i].resize(problem.timeslots * problem.rooms.size(), true);
        }
    }
    
    bool isConsistent(int course_id, int timeslot, int room) {
        const Course& course = problem.courses[course_id];
        const Room& room_obj = problem.rooms[room];
        const Teacher& teacher = problem.teachers[course.teacher_id];
        
        // Check room capacity
        if (course.students > room_obj.capacity) {
            return false;
        }
        
        // Check teacher availability
        for (int busy_slot : teacher.busy_slots) {
            if (busy_slot == timeslot) {
                return false;
            }
        }
        
        // Check room availability
        for (int busy_slot : room_obj.busy_slots) {
            if (busy_slot == timeslot) {
                return false;
            }
        }
        
        // Check for teacher double-booking
        for (int other_course_id = 0; other_course_id < problem.courses.size(); ++other_course_id) {
            if (other_course_id == course_id) continue;
            
            if (assignment[other_course_id].timeslot == timeslot && 
                assignment[other_course_id].timeslot != -1) {
                
                const Course& other_course = problem.courses[other_course_id];
                if (other_course.teacher_id == course.teacher_id) {
                    return false;
                }
            }
        }
        
        // Check for student group conflicts
        for (int other_course_id = 0; other_course_id < problem.courses.size(); ++other_course_id) {
            if (other_course_id == course_id) continue;
            
            if (assignment[other_course_id].timeslot == timeslot && 
                assignment[other_course_id].timeslot != -1) {
                
                const Course& other_course = problem.courses[other_course_id];
                if (shareStudentGroups(course, other_course)) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
    bool forwardCheck(int assigned_course_id, int timeslot, int room) {
        const Course& assigned_course = problem.courses[assigned_course_id];
        
        for (int other_course_id = 0; other_course_id < problem.courses.size(); ++other_course_id) {
            if (other_course_id == assigned_course_id || assignment[other_course_id].timeslot != -1) {
                continue;
            }
            
            const Course& other_course = problem.courses[other_course_id];
            
            for (int t = 0; t < problem.timeslots; ++t) {
                for (int r = 0; r < problem.rooms.size(); ++r) {
                    int domain_index = t * problem.rooms.size() + r;
                    
                    if (!domain[other_course_id][domain_index]) continue;
                    
                    bool consistent = true;
                    
                    // Check if this value would conflict with the new assignment
                    if (t == timeslot) {
                        // Same timeslot conflict checks
                        if (other_course.teacher_id == assigned_course.teacher_id) {
                            consistent = false;
                        }
                        
                        if (r == room) {
                            consistent = false;
                        }
                        
                        if (shareStudentGroups(assigned_course, other_course)) {
                            consistent = false;
                        }
                    }
                    
                    if (!consistent) {
                        domain[other_course_id][domain_index] = false;
                        
                        // Check for domain wipeout
                        bool has_valid_values = false;
                        for (int check_t = 0; check_t < problem.timeslots; ++check_t) {
                            for (int check_r = 0; check_r < problem.rooms.size(); ++check_r) {
                                if (domain[other_course_id][check_t * problem.rooms.size() + check_r]) {
                                    has_valid_values = true;
                                    break;
                                }
                            }
                            if (has_valid_values) break;
                        }
                        
                        if (!has_valid_values) {
                            return false; // Domain wipeout
                        }
                    }
                }
            }
        }
        
        return true;
    }
    
    bool backtrack() {
        int course_id = selectUnassignedVariableMRV(problem, assignment, domain);
        
        if (course_id == -1) {
            return true; // All courses assigned
        }
        
        vector<Value> ordered_values = orderDomainValues(problem, course_id, domain);
        
        for (const Value& value : ordered_values) {
            assignment_count++;
            
            if (isConsistent(course_id, value.timeslot, value.room)) {
                // Save current domain state
                vector<vector<bool>> old_domain = domain;
                
                // Make assignment
                assignment[course_id] = Assignment(value.timeslot, value.room);
                
                // Apply forward checking
                if (forwardCheck(course_id, value.timeslot, value.room)) {
                    if (backtrack()) {
                        return true;
                    }
                }
                
                // Restore domain and backtrack
                domain = old_domain;
                assignment[course_id] = Assignment(-1, -1);
                backtrack_count++;
            }
        }
        
        return false;
    }

public:
    ForwardCheckingSolver(const TimetableProblem& prob) 
        : problem(prob), backtrack_count(0), assignment_count(0) {
        assignment.resize(problem.courses.size(), Assignment(-1, -1));
        initializeDomain();
    }
    
    bool solve() {
        return backtrack();
    }
    
    int getBacktrackCount() const { return backtrack_count; }
    int getAssignmentCount() const { return assignment_count; }
    const TimetableAssignment& getAssignment() const { return assignment; }
    
    void printSolution() const {
        cout << "Timetable Solution (Forward Checking):" << endl;
        cout << "=====================================" << endl;
        
        bool all_assigned = true;
        for (int i = 0; i < assignment.size(); ++i) {
            if (assignment[i].timeslot != -1) {
                const Course& course = problem.courses[i];
                cout << "Course " << course.id << " (" << course.name << "): "
                     << "Timeslot " << assignment[i].timeslot 
                     << ", Room " << problem.rooms[assignment[i].room].name 
                     << " (Teacher: " << problem.teachers[course.teacher_id].name 
                     << ", Students: " << course.students << ")" << endl;
            } else {
                cout << "Course " << i << " NOT ASSIGNED!" << endl;
                all_assigned = false;
            }
        }
        
        cout << "\nPerformance:" << endl;
        cout << "Backtracks: " << backtrack_count << endl;
        cout << "Assignments attempted: " << assignment_count << endl;
        cout << "All courses assigned: " << (all_assigned ? "YES" : "NO") << endl;
    }
};

#endif
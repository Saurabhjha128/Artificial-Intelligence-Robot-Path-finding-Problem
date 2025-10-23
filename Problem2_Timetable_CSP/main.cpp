#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <algorithm>
#include "backtracking.h"
#include "forward_checking.h"
#include "heuristics.h"

using namespace std;

void runPerformanceComparison() {
    const int NUM_RUNS = 20;
    
    ofstream data_file("performance_data.csv");
    data_file << "Run,Method,TimeTaken,BacktrackCount,Assignments,SolutionFound\n";
    
    int backtracking_success = 0;
    int forward_checking_success = 0;
    
    for (int run = 0; run < NUM_RUNS; ++run) {
        cout << "Running test " << run + 1 << "/" << NUM_RUNS << endl;
        
        // Create a timetable problem
        TimetableProblem problem = createRandomTimetableProblem();
        
        // Run Backtracking with heuristics
        auto start = chrono::high_resolution_clock::now();
        BacktrackingSolver bt_solver(problem);
        bool bt_success = bt_solver.solve();
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> bt_duration = end - start;
        
        if (bt_success) backtracking_success++;
        
        data_file << run + 1 << ",Backtracking,"
                 << bt_duration.count() << ","
                 << bt_solver.getBacktrackCount() << ","
                 << bt_solver.getAssignmentCount() << ","
                 << (bt_success ? "Yes" : "No") << "\n";
        
        // Run Forward Checking
        start = chrono::high_resolution_clock::now();
        ForwardCheckingSolver fc_solver(problem);
        bool fc_success = fc_solver.solve();
        end = chrono::high_resolution_clock::now();
        chrono::duration<double> fc_duration = end - start;
        
        if (fc_success) forward_checking_success++;
        
        data_file << run + 1 << ",ForwardChecking,"
                 << fc_duration.count() << ","
                 << fc_solver.getBacktrackCount() << ","
                 << fc_solver.getAssignmentCount() << ","
                 << (fc_success ? "Yes" : "No") << "\n";
    }
    
    data_file.close();
    
    cout << "\nSuccess Rates:" << endl;
    cout << "Backtracking: " << backtracking_success << "/" << NUM_RUNS 
         << " (" << (backtracking_success * 100.0 / NUM_RUNS) << "%)" << endl;
    cout << "Forward Checking: " << forward_checking_success << "/" << NUM_RUNS 
         << " (" << (forward_checking_success * 100.0 / NUM_RUNS) << "%)" << endl;
    
    cout << "Performance data saved to performance_data.csv" << endl;
}

int main() {
    // Run performance comparison
    runPerformanceComparison();
    
    // Demonstrate solution for one instance
    cout << "\n=== Example Solution ===" << endl;
    TimetableProblem problem = createStandardTimetableProblem();
    
    cout << "Problem Details:" << endl;
    cout << "Timeslots: " << problem.timeslots << endl;
    cout << "Rooms: " << problem.rooms.size() << endl;
    cout << "Teachers: " << problem.teachers.size() << endl;
    cout << "Courses: " << problem.courses.size() << endl;
    
    cout << "\nTrying Backtracking..." << endl;
    BacktrackingSolver bt_solver(problem);
    if (bt_solver.solve()) {
        bt_solver.printSolution();
    } else {
        cout << "No solution found with backtracking!" << endl;
    }
    
    cout << "\nTrying Forward Checking..." << endl;
    ForwardCheckingSolver fc_solver(problem);
    if (fc_solver.solve()) {
        fc_solver.printSolution();
    } else {
        cout << "No solution found with forward checking!" << endl;
    }
    
    return 0;
}
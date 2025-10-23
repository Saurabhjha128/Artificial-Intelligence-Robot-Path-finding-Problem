# Timetable Generation using Constraint Satisfaction Problem (CSP)

## Project Overview
This project implements a university timetable scheduling system modeled as a Constraint Satisfaction Problem (CSP). It provides two constraint solving techniques with performance comparison and analysis.

## Problem Description
Generate an optimal university timetable by assigning courses to timeslots and rooms while satisfying all hard constraints:
- Each course must be assigned to one timeslot and one room
- No teacher can teach multiple courses simultaneously
- No room can host multiple courses simultaneously
- Room capacity must accommodate course students
- Student groups cannot have overlapping courses
- Teacher and room availability constraints must be respected

## Algorithms Implemented

### 1. Backtracking with Heuristics
- **Variable Ordering**: Minimum Remaining Values (MRV)
- **Value Ordering**: Least Constraining Value (LCV)
- Chronological backtracking when constraints are violated

### 2. Forward Checking
- Maintains current domains for unassigned variables
- Propagates constraints after each assignment
- Early detection of dead ends through domain wipeout

## Code Structure

# compiltion
```bash
g++ -std=c++11 main.cpp -o csp_solver
./csp_solver
python performance_plot.py
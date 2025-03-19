# eVTOL Aircraft Simulation

## Overview
This project simulates an electric Vertical Take-Off and Landing (eVTOL) aircraft system. The simulation runs for 3 hours, deploying 20 randomly selected vehicles from five different manufacturers. The simulation models flight time, charging station usage, and faults encountered.

## Features
- Simulates **20 eVTOL vehicles** from 5 different manufacturers.
- Each vehicle operates with distinct **speed, battery capacity, energy consumption, and fault probability**.
- **3 Charging Stations** are available, shared among all vehicles.
- Vehicles **fly until their battery is drained** and immediately **queue for charging**.
- The simulation collects and outputs **key performance statistics per vehicle type**:
  - Average flight time per flight
  - Average distance traveled per flight
  - Average time spent charging per session
  - Total number of faults encountered
  - Total passenger miles traveled

## Assumptions
- All vehicles start with a **fully charged battery**.
- Vehicles instantly reach their **cruise speed**.
- Vehicles **remain airborne** until they deplete their battery.
- After landing, vehicles **queue for charging** immediately.

## Vehicle Properties
| Company | Cruise Speed (mph) | Battery Capacity (kWh) | Time to Charge (hours) | Energy Use (kWh/mile) | Passenger Count | Fault Probability per Hour |
|---------|-------------------|--------------------|----------------|----------------|----------------|--------------------------|
| Alpha   | 120               | 320                | 0.6            | 1.6            | 4              | 0.25                     |
| Bravo   | 100               | 100                | 0.2            | 1.5            | 5              | 0.10                     |
| Charlie | 160               | 220                | 0.8            | 2.2            | 3              | 0.05                     |
| Delta   | 90                | 120                | 0.62           | 0.8            | 2              | 0.22                     |
| Echo    | 30                | 150                | 0.3            | 5.8            | 2              | 0.61                     |

## Simulation Details
- Vehicles **fly until their battery depletes**.
- Once landed, they **queue for charging** if no chargers are available.
- Charging is performed **one vehicle at a time per charger** (3 available chargers).
- After charging, vehicles take off again and repeat the cycle.
- The simulation runs for **3 hours** with a **time increment of 0.2 hours**.

## Output Statistics
After the simulation, the program outputs the following statistics for each vehicle type:
- **Average Flight Time per Flight**
- **Average Distance Traveled per Flight**
- **Average Charging Time per Session**
- **Total Number of Faults**
- **Total Passenger Miles**

## Example Output
```
Randomly selecting 20 vehicles from the vehicle list: 
Pick 1: Charlie
Pick 2: Delta
Pick 3: Bravo
Pick 4: Echo
Pick 5: Alpha
Pick 6: Charlie
Pick 7: Echo
Pick 8: Charlie
Pick 9: Echo
Pick 10: Echo
Pick 11: Alpha
Pick 12: Delta
Pick 13: Bravo
Pick 14: Charlie
Pick 15: Delta
Pick 16: Delta
Pick 17: Bravo
Pick 18: Alpha
Pick 19: Charlie
Pick 20: Bravo

==============================
 Final Vehicle Statistics
==============================

Statistics for Alpha:
Avg. Flight Time per Flight: 0 hours
Avg. Distance Traveled per Flight: 0 miles
Avg. Charging Time per Session: 0 hours
Total Faults: 3
Total Passenger Miles: 0 miles

Statistics for Charlie:
Avg. Flight Time per Flight: 1.375 hours
Avg. Distance Traveled per Flight: 220 miles
Avg. Charging Time per Session: 1.76 hours
Total Faults: 0
Total Passenger Miles: 3300 miles

Statistics for Delta:
Avg. Flight Time per Flight: 3.75 hours
Avg. Distance Traveled per Flight: 337.5 miles
Avg. Charging Time per Session: 1.395 hours
Total Faults: 13
Total Passenger Miles: 2700 miles

Statistics for Bravo:
Avg. Flight Time per Flight: 1.16667 hours
Avg. Distance Traveled per Flight: 116.667 miles
Avg. Charging Time per Session: 0.35 hours
Total Faults: 0
Total Passenger Miles: 2333.33 miles

Statistics for Echo:
Avg. Flight Time per Flight: 0.431034 hours
Avg. Distance Traveled per Flight: 12.931 miles
Avg. Charging Time per Session: 0.15 hours
Total Faults: 6
Total Passenger Miles: 103.448 miles
```

## How to Run
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/eVTOL-Simulation.git
   cd eVTOL-Simulation
   ```
2. Compile the program:
   ```bash
   g++ Joby_C_assignment.cpp -o Joby_C_assignment
   ```
3. Run the simulation:
   ```bash
   ./eVTOL_Simulation
   ```

## Author
**Shivam Shah**

## License
This project is licensed under the MIT License - see the LICENSE file for details.


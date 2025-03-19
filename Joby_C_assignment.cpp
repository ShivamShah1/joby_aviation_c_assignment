/* 
Author: Shivam Shah
Reason: Joby Aviation C assignment
Date: 03/18/25
Time: 8:07 PM

To perform an eVTOL Simulation Problem

There are five companies developing eVTOL aircraft with the some different characteristics.
Trying to simulate using 20 random vehicles with 3 chargers from the given 5 companies for 3 hours,
to calculate the following
- average flight time per flight
- average distance traveled per flight
- average time charging per charge session
- total number of faults
- total number of passenger miles. 

with these assumptions:
- Each vehicle starts the simulation with a fully-charged battery
- Each vehicle instantaneously reaches Cruise Speed
- Each vehicle is airborne for the full use of the battery, and is immediately in line for the charger after running out of battery power.

Intuition and ideology:
All the selected vehicles will have full battery power so all the 20 vehicles will be flying in the air,
so calculating the flight time for all vehicles with full charged battery and the vehicles with the respective 
flight range and time will land respectively and will try to accuire the charging station if free or will wait for its avaibility and 
then set its flight again. We will try to run this scenario for 3 hours.

*/

#include <iostream>
#include <random>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

/* global variables */

/* time for simulation */
double GLOBAL_TIME = 0.0;

/* duration of for the simulation of the system */
double GLOBAL_END_TIME = 3.0;

/* random numbers of vehicles taken for the test simulation */
int NUMBER_OF_VEHICLES = 20;

/* increment of time for the simulation */
double TIME_INCREMENT = 0.2;

/* number of charging station to charge the battery of vehicles */
int CHARGING_STATION_PROVIDED = 3;

/* small tolerance value for floating-point comparison */
const double EPSILON = 1e-6;

/* structure to store statistics of vehicle for data analysis 
    count - is taken for maintaing number of vehicles used of sepcific class during the simulation;
    total_flight_time - to get the commulative flight duration of the vehicle;
    total_distance_traveled - total distance covered by all the vehicles of same class;
    total_charging_time - time taken by during charging of the battery;
    total_faults - faults encountered during the simulation;
    total_passenger_miles - total miles coverd by passenges commulatively;

    display_average_stats - to display the statistics of the vehicle all together of same class. 
*/
struct Statistics {
    int count = 0;
    double total_flight_time = 0;
    double total_distance_traveled = 0;
    double total_charging_time = 0;
    int total_faults = 0;
    double total_passenger_miles = 0;

    /* compute and display averages of the vehicles */
    void display_average_stats(const string& vehicle_type) {

        /* avoiding division by zero */
        if (count == 0) return;
        cout << "\nStatistics for " << vehicle_type << ":\n";
        cout << "Avg. Flight Time per Flight: " << (total_flight_time / count) << " hours\n";
        cout << "Avg. Distance Traveled per Flight: " << (total_distance_traveled / count) << " miles\n";
        cout << "Avg. Charging Time per Session: " << (total_charging_time / count) << " hours\n";
        cout << "Total Faults: " << total_faults << "\n";
        cout << "Total Passenger Miles: " << total_passenger_miles << " miles\n";
    }
};

/* vehicle class encapsulating eVTOL properties and statistics
    company - name of the vehicle;
    cruise_speed - speed of the vehicle;
    battery_capacity - battery capcity of the vehicle;
    time_to_charge - time taken to charge the battery;
    energy_per_mile - energy consumed per mile;
    passenger_count - passenge travelling in the vehicle;
    fault_probability - the passibility of fault happening;
    flight_timing - time in which it stays in air;
    distance_traveled - total distance travelled;
    faults - fault counts;
    passenger_miles - miles travelled by passenger commulatively;
    start_charge_time - time when lastest charge for battery started;
    in_charging_station - checking if it is in charging station for charging;
    waiting_in_charging_waiting_queue - checking if it is waiting in the queue for acquiring the charge;
    landed_from_flight - time when it landed;
    number_of_charge - number of times it charged its battery;
    updated_flight_timing - the time when it will again land;

    Vechicle(...) - constructor;

    simualte_flight() - to simulate the flight conditions and updates its parameters.
*/
class Vehicle {
    public:
        string company;
        int cruise_speed;
        double battery_capacity;
        double time_to_charge;
        double energy_per_mile;
        int passenger_count;
        double fault_probability;
        double flight_timing = 0;
        double distance_traveled = 0;
        int faults = 0;
        double passenger_miles = 0;
        double start_charge_time = 0;
        bool in_charging_station = false;
        bool waiting_in_charging_waiting_queue = false;
        double landed_from_flight = 0;
        int number_of_charge = 0;
        int updated_flight_timing = 0;
        
        /* creating a constructor with some default values */
        Vehicle(string comp = "Unknown", int speed = 0, double capacity = 0, double charge = 0, double energy = 0, int passengers = 0, double fault = 0)
            : company(comp), cruise_speed(speed), battery_capacity(capacity), time_to_charge(charge), energy_per_mile(energy),
            passenger_count(passengers), fault_probability(fault) {}

        /* to simulate flight */
        void simulate_flight();
};

/* simulate the flight and using bernoulli distribution for getting fault from fault probability */
void Vehicle::simulate_flight() {
    flight_timing = battery_capacity / (energy_per_mile * cruise_speed);
    updated_flight_timing = flight_timing;
    distance_traveled = flight_timing * cruise_speed;
    passenger_miles = passenger_count * distance_traveled;

    random_device rd;
    mt19937 gen(rd());
    bernoulli_distribution fault_chances(fault_probability * flight_timing);

    for (double t = 0; t <= flight_timing; t += TIME_INCREMENT) {
        if (fault_chances(gen)) {
            faults++;
        }
    }
}

/* charger class to manage charging stations 
    vehicles_assigned - notes how many times vehicles have used charger;
    vehicles_released - notes how many times vehicles have released charger after acquiring them;
    charging_station_map - to map the address of vehicles which are accessign them;

    Charger(int num_chargers) - to create a construct fucntion having default value 0;

    get_charging_station_map() - to get the charging station mapped addresses;

    is_charging_station_free() - to check if the any charging station is available;

    get_free_charger_index() - to know which charging station is available for the connection;

    assign_vehicle_to_charger() - acquiring the charging station;

    release_charger() - to release the charging station;
*/
class Charger {
public:

    /* tracks how many times vehicles are assigned to a charger */
    int vehicles_assigned = 0;

    /* tracks how many times vehicles are released */
    int vehicles_released = 0;

    /* maps a vehicle's address to itself */
    unordered_map<Vehicle*, Vehicle*> charging_station_map;

    /* constructor with default value of 0 */
    Charger(int num_chargers = 0) {}
    
    /* getter function to access charging_station_map */
    unordered_map<Vehicle*, Vehicle*>& get_charging_station_map() {
        return charging_station_map;
    }

    /* checks if any charging slot is free */
    bool is_charging_station_free() {
        return charging_station_map.size() < CHARGING_STATION_PROVIDED;
    }

    /* finds the first available charging slot */
    int get_free_charger_index() {
        return charging_station_map.size() < CHARGING_STATION_PROVIDED ? charging_station_map.size() : -1;
    }

    /* assigns a vehicle to a free charging slot */
    bool assign_vehicle_to_charger(Vehicle** vehicle) {
        vehicles_assigned++;
        int free_slot = get_free_charger_index();
        if (free_slot != -1) {
            charging_station_map[*vehicle] = *vehicle;
            (*vehicle)->in_charging_station = true;
            (*vehicle)->waiting_in_charging_waiting_queue = false;

            /* here setting to greater than 3 is beacuse there are 3 stations, as the timer hits 0 for the charging vehicle, due to simulation it will not be available for other vehicles */
            /* so after 0.2 time increment this station will be assigned to the queued vehicle and will start changing and will note that time as charge start time. */
            /* so to save the 0.2 time of being ideal in I considering that time to the next vehicle which will connect to it, and understand that it was connected in between this time increment */
            /* which is ideal in real scenarios */
            if(vehicles_assigned>3){

                /* makes sure that the charging station is acquired when the acquired vehicle time is 0 */
                (*vehicle)->start_charge_time = GLOBAL_TIME - TIME_INCREMENT;
            }
            else{
                (*vehicle)->start_charge_time = GLOBAL_TIME;
            }
            return true;
        } 
        else {
            return false;
        }
    }    

    /* releases a charger when charging is complete */
    void release_charger(Vehicle** vehicle) {
        vehicles_released++;
        if (charging_station_map.find(*vehicle) != charging_station_map.end()) {
            (*vehicle)->in_charging_station = false;
            (*vehicle)->number_of_charge++;

            /* here setting to greater than 3 is beacuse there are 3 stations, as the timer hits 0 for the charging vehicle, due to simulation it will not be available for other vehicles */
            /* so after 0.2 time increment this station will be assigned to the queued vehicle and will start changing and will note that time as charge start time. */
            /* so to save the 0.2 time of being ideal in I considering that time to the next vehicle which will connect to it, and understand that it was connected in between this time increment */
            /* which is ideal in real scenarios */
            if(vehicles_released>3){

                /* makes sure that the charging station is acquired when the acquired vehicle time is 0 */
                (*vehicle)->updated_flight_timing += GLOBAL_TIME - TIME_INCREMENT + EPSILON;
            }
            else{
                (*vehicle)->updated_flight_timing += GLOBAL_TIME;
            }

            /* erassing the memory address from the queue so that it does not fall back in getting the charger */
            charging_station_map.erase(*vehicle);
        }
    }    
};

/* map to store statistics for each vehicle type */
unordered_map<string, Statistics> vehicle_stats;

int main() {

    /* instantiating 5 objects of vehicles */
    Vehicle* Alpha = new Vehicle("Alpha", 120, 320, 0.6, 1.6, 4, 0.25);
    Vehicle* Bravo = new Vehicle("Bravo", 100, 100, 0.2, 1.5, 5, 0.10);
    Vehicle* Charlie = new Vehicle("Charlie", 160, 220, 0.8, 2.2, 3, 0.05);
    Vehicle* Delta = new Vehicle("Delta", 90, 120, 0.62, 0.8, 2, 0.22);
    Vehicle* Echo = new Vehicle("Echo", 30, 150, 0.3, 5.8, 2, 0.61);
    
    /* using random functino to picking 20 vehicles form the objects created above with equal picking probability */
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> random_number(1, 5);

    /* creating a vector to store the information of the 20 vehicles */
    vector<Vehicle*> random_selected_vehicles;

    /* ceating unordered_map for address references */
    unordered_map<Vehicle*, Vehicle*> current_vehicle;
    
    /* track vehicles that are in chraging station or in waiting queue for acquiring the charging station */
    unordered_map<Vehicle*, Vehicle*> in_charging_queue;
    
    /* instantiating a charging station */
    Charger charging_station(CHARGING_STATION_PROVIDED);

    /* selecting 20 random vehicles */
    cout << "Randomly selecting 20 vehicles from the vehicle list: " << endl;
    for (int i = 0; i < NUMBER_OF_VEHICLES; i++) {
        int choice = random_number(gen);
        Vehicle* select_vehicle = nullptr;

        switch (choice) {
            case 1: 
                select_vehicle = new Vehicle("Alpha", 120, 320, 0.6, 1.6, 4, 0.25);
                break;
            case 2: 
                select_vehicle = new Vehicle("Bravo", 100, 100, 0.2, 1.5, 5, 0.10);
                break;
            case 3: 
                select_vehicle = new Vehicle("Charlie", 160, 220, 0.8, 2.2, 3, 0.05);
                break;
            case 4: 
                select_vehicle = new Vehicle("Delta", 90, 120, 0.62, 0.8, 2, 0.22);
                break;
            case 5: 
                select_vehicle = new Vehicle("Echo", 30, 150, 0.3, 5.8, 2, 0.61);
                break;
        }

        /* storing the selected vehicle in the vector */
        random_selected_vehicles.push_back(select_vehicle);
        random_selected_vehicles[i]->simulate_flight();
        cout << "Pick " << i + 1 << ": " << select_vehicle->company << endl;
    } 
    
    /* running the simulation till the desired time period */
    while(GLOBAL_TIME <= GLOBAL_END_TIME + EPSILON){

        /* processes each vehicle */
        for(int i = 0; i < NUMBER_OF_VEHICLES; i++) {
            Vehicle* latest_vehicle = random_selected_vehicles[i];

            /* if the vehicle is new than add that to the map */
            if(current_vehicle.find(latest_vehicle) == current_vehicle.end()) {
                current_vehicle[latest_vehicle] = latest_vehicle;
            }

            /* checks if the vehicle is still in the air */
            if(GLOBAL_TIME <= current_vehicle[latest_vehicle]->updated_flight_timing) {
                continue;
            }
            else {

                /* ensures the vehicle enters the queue only once */
                if (!in_charging_queue[current_vehicle[latest_vehicle]]) {
                    in_charging_queue[current_vehicle[latest_vehicle]] = latest_vehicle;
                    (in_charging_queue[current_vehicle[latest_vehicle]])->waiting_in_charging_waiting_queue = true;
                }
                
                /* if it the vehicle is in charging station */
                if((in_charging_queue[current_vehicle[latest_vehicle]])->in_charging_station == true){

                    /* checking if it can release the charging station */
                    if((GLOBAL_TIME - (in_charging_queue[current_vehicle[latest_vehicle]])->updated_flight_timing) >= (in_charging_queue[current_vehicle[latest_vehicle]])->time_to_charge){
                        
                        /* battery full, ready to take the flight with this stats */
                        (in_charging_queue[current_vehicle[latest_vehicle]])->simulate_flight();
                        charging_station.release_charger(&in_charging_queue[current_vehicle[latest_vehicle]]);
                        (in_charging_queue[current_vehicle[latest_vehicle]])->in_charging_station = false;
                        (in_charging_queue[current_vehicle[latest_vehicle]])->waiting_in_charging_waiting_queue = false;
                        (in_charging_queue[current_vehicle[latest_vehicle]])->updated_flight_timing = GLOBAL_TIME;
                        
                        /* ensuring that the vehicle is removed from the queue for charging */
                        if (in_charging_queue.find(current_vehicle[latest_vehicle]) != in_charging_queue.end()) {
                            in_charging_queue.erase(current_vehicle[latest_vehicle]);
                        }
                    }
                    else{
                        /* ensuring that the vehicle is removed from the queue for charging */
                        if (in_charging_queue.find(current_vehicle[latest_vehicle]) != in_charging_queue.end()) {
                            in_charging_queue.erase(current_vehicle[latest_vehicle]);
                        }
                    }
                }

                /* if a charger is available, assign it it is next in waiting vehicle */
                else if (charging_station.is_charging_station_free() && (in_charging_queue[current_vehicle[latest_vehicle]])->in_charging_station == false && (in_charging_queue[current_vehicle[latest_vehicle]])->waiting_in_charging_waiting_queue == true && in_charging_queue.size()==1){
                    charging_station.assign_vehicle_to_charger(&in_charging_queue[current_vehicle[latest_vehicle]]);
                    (in_charging_queue[current_vehicle[latest_vehicle]])->in_charging_station = true;
                    (in_charging_queue[current_vehicle[latest_vehicle]])->waiting_in_charging_waiting_queue = false;
                }               
                else {
                    continue;
                }
            }
        }
        
        /* making sure that the charger is not in ideal condition when there are vehicles in the queue */
        while (charging_station.is_charging_station_free() && !in_charging_queue.empty()) {
            auto first_entry = in_charging_queue.begin();
            Vehicle* next_vehicle = first_entry->first;
            if (charging_station.assign_vehicle_to_charger(&next_vehicle)) {
                first_entry = in_charging_queue.erase(first_entry);
            }
        }

        /* updating global time for creating a simulation */
        GLOBAL_TIME += TIME_INCREMENT;
    }

    /* using structure to store the stats of the same vehicle for data analysis */
    for (Vehicle* vehicle : random_selected_vehicles) {
        Statistics& stats = vehicle_stats[vehicle->company];
        stats.count++;
        stats.total_flight_time += vehicle->number_of_charge * vehicle->flight_timing;
        stats.total_distance_traveled += vehicle->number_of_charge * vehicle->distance_traveled;
        stats.total_faults += vehicle->faults;
        stats.total_passenger_miles += vehicle->number_of_charge * vehicle->passenger_miles;
        stats.total_charging_time += vehicle->number_of_charge * vehicle->time_to_charge;
    }

    /* Displaying statistics of each vehicle which are in the random selection */
    cout << "\n==============================";
    cout << "\n Final Vehicle Statistics";
    cout << "\n==============================\n";

    for (auto it = vehicle_stats.begin(); it != vehicle_stats.end(); ++it) {
        it->second.display_average_stats(it->first);
    }

    /* freeing the allocated memory which were used by vehicle class */
    delete Alpha;
    delete Bravo;
    delete Charlie;
    delete Delta;
    delete Echo;
    
    return 0;
}

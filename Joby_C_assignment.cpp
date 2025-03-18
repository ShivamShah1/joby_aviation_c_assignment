/* 
Author: Shivam Shah
Reason: Joby Aviation C assignment
Date: 03/18/25
Time: 12:07 PM

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
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <algorithm>

using namespace std;

/* global variable */
double GLOBAL_TIME = 0.0;
double GLOBAL_END_TIME = 3.0;
int NUMBER_OF_VEHICLES = 20;
double TIME_INCREMENT = 0.2;
int CHARGING_STATION_PROVIDED = 3;

/* vehicle count */
int vehicle_alpha = 0;
int vehicle_bravo = 0;
int vehicle_chralie = 0;
int vehicle_delta = 0;
int vehicle_echo = 0;

/* small tolerance value for floating-point comparison */
const double EPSILON = 1e-6;

/* vehicle class encapsulating eVTOL properties and statistics */
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
        
        /* creating a constructor */
        Vehicle(string comp, int speed, double capacity, double charge, double energy, int passengers, double fault)
            : company(comp), cruise_speed(speed), battery_capacity(capacity), time_to_charge(charge), energy_per_mile(energy),
            passenger_count(passengers), fault_probability(fault) {}

        /* to simulate flight */
        void simulate_flight();

        /* to know if the flight is still in air at the given time */
        bool is_in_flight(double current_time);
};

/* simulate the flight */
void Vehicle::simulate_flight() {
    flight_timing = battery_capacity / (energy_per_mile * cruise_speed);
    distance_traveled = flight_timing * cruise_speed;
    passenger_miles = passenger_count * distance_traveled;

    random_device rd;
    mt19937 gen(rd());
    bernoulli_distribution fault_chances(fault_probability * flight_timing);
    if (fault_chances(gen)) {
        faults++;
    }
    //cout << "Flight in air: "<< flight_timing <<", distance traveled: "<< distance_traveled << ", passenger_miles: " << passenger_miles << endl;
}

/* to check if the vehicle is still in flight */
bool Vehicle::is_in_flight(double current_time) {
    return flight_timing > current_time;
}

/* charger class to manage charging stations */
class Charger {
private:
    unordered_map<Vehicle*, Vehicle*> charging_station_map;  // Maps a vehicle's address to itself

public:
    /* constructor */
    Charger(int num_chargers) {} // Empty constructor since we use unordered_map

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
        int free_slot = get_free_charger_index();
        if (free_slot != -1) {
            charging_station_map[*vehicle] = *vehicle;  // Store the vehicle's memory address

            // Print debug info to confirm unique address usage
            cout << "Assigned " << (*vehicle)->company 
                    << " (Address: " << *vehicle << ") to charging slot " << free_slot << endl;

            (*vehicle)->start_charge_time = GLOBAL_TIME;
            return true;
        } else {
            cout << "No free charging slots available for " 
                    << (*vehicle)->company << " (Address: " << *vehicle << ")" << endl;
            return false;
        }
    }    

    /* releases a charger when charging is complete */
    void release_charger(Vehicle* vehicle) {
        if (charging_station_map.find(vehicle) != charging_station_map.end()) {
            charging_station_map.erase(vehicle);
            cout << vehicle->company << " (Address: " << vehicle << ") has finished charging and left the station." << endl;
        }
    }
};    

/* comparator to prioritize the vehicle with min-heap based on flight time to add the flight in queue accordingly */
struct vehicle_landing_comparator {
    bool operator()(Vehicle* v1, Vehicle* v2) {
        return v1->flight_timing > v2->flight_timing;
    }
};

int main() {
    /* creating 5 objects of vehicles */
    Vehicle* Alpha = new Vehicle("Alpha", 120, 320, 0.6, 1.6, 4, 0.25);
    Vehicle* Bravo = new Vehicle("Bravo", 100, 100, 0.2, 1.5, 5, 0.10);
    Vehicle* Charlie = new Vehicle("Charlie", 160, 220, 0.8, 2.2, 3, 0.05);
    Vehicle* Delta = new Vehicle("Delta", 90, 120, 0.62, 0.8, 2, 0.22);
    Vehicle* Echo = new Vehicle("Echo", 30, 150, 0.3, 5.8, 2, 0.61);
    
    /* picking random 20 vehicles form the objects created above with equal picking probability */
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> random_number(1, 5);

    /* creating a vector to store the information of the 20 vehicles */
    vector<Vehicle*> random_selected_vehicles;
    
    /* creating a vector to store the information of the charging queue vehicles */
    vector<Vehicle*> charge_waiting_vehicles;
    
    /* creating a charging station */
    Charger charging_station(CHARGING_STATION_PROVIDED);

    /* picking 20 random vehicles */
    cout << "Randomly selecting 20 vehicles from the vehicle list: " << endl;
    for (int i = 0; i < NUMBER_OF_VEHICLES; i++) {
        int choice = random_number(gen);
        Vehicle* select_vehicle = nullptr;

        switch (choice) {
            case 1: 
                select_vehicle = new Vehicle("Alpha", 120, 320, 0.6, 1.6, 4, 0.25); 
                vehicle_alpha++;
                break;
            case 2: 
                select_vehicle = new Vehicle("Bravo", 100, 100, 0.2, 1.5, 5, 0.10); 
                vehicle_bravo++;
                break;
            case 3: 
                select_vehicle = new Vehicle("Charlie", 160, 220, 0.8, 2.2, 3, 0.05);
                vehicle_chralie++;
                break;
            case 4: 
                select_vehicle = new Vehicle("Delta", 90, 120, 0.62, 0.8, 2, 0.22); 
                vehicle_delta++;
                break;
            case 5: 
                select_vehicle = new Vehicle("Echo", 30, 150, 0.3, 5.8, 2, 0.61); 
                vehicle_echo++;
                break;
        }

        /* storing the selected vehicle in the vector */
        random_selected_vehicles.push_back(select_vehicle);
        random_selected_vehicles[i]->simulate_flight();
        cout << "Pick " << i + 1 << ": " << select_vehicle->company << endl;
    }
    
    /* ceating unorder_map for address references */
    unordered_map<Vehicle*, Vehicle*> current_vehicle;

    /* running the simulation till the desired time period */
    while(GLOBAL_TIME <= GLOBAL_END_TIME + EPSILON){
        /* processes each vehicle */
        for(int i = 0; i < NUMBER_OF_VEHICLES; i++) {
            Vehicle* latest_vehicle = random_selected_vehicles[i];
            if(current_vehicle.find(latest_vehicle) == current_vehicle.end()) {
                current_vehicle[latest_vehicle] = latest_vehicle;
            }            
            
            /* checks if the vehicle is still in the air */
            if(GLOBAL_TIME <= current_vehicle[latest_vehicle]->flight_timing) {
                cout << "Flight time left for vehicle " << current_vehicle[latest_vehicle]->company 
                     << " at " << latest_vehicle << ": " 
                     << current_vehicle[latest_vehicle]->flight_timing - GLOBAL_TIME << " hours." << endl;
            } else {
                /* adds vehicle to the charging queue when its flight is over */
                if(find(charge_waiting_vehicles.begin(), charge_waiting_vehicles.end(), current_vehicle[latest_vehicle]) == charge_waiting_vehicles.end()) {
                    charge_waiting_vehicles.push_back(current_vehicle[latest_vehicle]);
                    cout << current_vehicle[latest_vehicle]->company << " at " << latest_vehicle << " has landed and is waiting for charging." << endl;
                }
                
                /* if a charger is available, assign the vehicle to it */
                if(charging_station.is_charging_station_free()) {
                    for(auto it = charge_waiting_vehicles.begin(); it != charge_waiting_vehicles.end(); ) {
                        if(charging_station.assign_vehicle_to_charger(&(*it))) {
                            it = charge_waiting_vehicles.erase(it); // Remove from queue once assigned
                        } else {
                            ++it;
                        }
                    }
                } else {
                    cout << "No available chargers for "<< current_vehicle[latest_vehicle]->company 
                         << " at " << latest_vehicle << ". Waiting in queue." << endl;
                }
            }
        }
        
        /* after processing, handle charging completions */
        for(auto it = charge_waiting_vehicles.begin(); it != charge_waiting_vehicles.end(); ) {
            Vehicle* vehicle = *it;
            if((GLOBAL_TIME - vehicle->start_charge_time) >= vehicle->time_to_charge) {
                charging_station.release_charger(vehicle);
                /* adjusting flight timing after charging */
                vehicle->flight_timing = GLOBAL_TIME + (vehicle->battery_capacity / (vehicle->energy_per_mile * vehicle->cruise_speed)); 
                cout << vehicle->company << " at " << vehicle << " completed charging and is ready for takeoff." << endl;
                it = charge_waiting_vehicles.erase(it); // Remove from queue after charging
            } else {
                cout << vehicle->company << " at " << vehicle << " is charging. Time left: " 
                     << vehicle->time_to_charge - (GLOBAL_TIME - vehicle->start_charge_time) << " hours." << endl;
                ++it;
            }
        }        
    
        /* updating global time */ 
        cout << "Global time: " << GLOBAL_TIME << endl;
        GLOBAL_TIME += TIME_INCREMENT;
    }

    /* printing the result */

    /* freeing the allocated memory */
    delete Alpha;
    delete Bravo;
    delete Charlie;
    delete Delta;
    delete Echo;
    
    return 0;
}

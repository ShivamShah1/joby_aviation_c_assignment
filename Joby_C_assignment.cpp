/* 
Author: Shivam Shah
Reason: Joby Aviation C assignment
Date: 03/18/25
Time: 2:57 PM

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
        bool in_charging_station = false;
        bool waiting_in_charging_waiting_queue = false;
        double landed_from_flight = 0;
        
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
public:
    /* tracks how many times vehicles are assigned to a charger */
    int vehicles_assigned = 0;
    /* tracks how many times vehicles are released */
    int vehicles_released = 0;
    /* maps a vehicle's address to itself */
    unordered_map<Vehicle*, Vehicle*> charging_station_map;
    /* constructor */
    Charger(int num_chargers) {}
    
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
            if(vehicles_assigned>3){
                (*vehicle)->start_charge_time = GLOBAL_TIME - TIME_INCREMENT;
            }
            else{
                (*vehicle)->start_charge_time = GLOBAL_TIME;
            }
            cout << "Assigned " << (*vehicle)->company << " (Address: " << *vehicle << ") to charging slot " << free_slot << " at time: " << (*vehicle)->start_charge_time << " and need time: "<<(*vehicle)->time_to_charge << endl;

            return true;
        } else {
            cout << "No free charging slots available for " << (*vehicle)->company << " (Address: " << *vehicle << ")" << endl;
            return false;
        }
    }    

    /* releases a charger when charging is complete */
    void release_charger(Vehicle** vehicle) {
        vehicles_released++;
        if (charging_station_map.find(*vehicle) != charging_station_map.end()) {
            (*vehicle)->in_charging_station = false;
            if(vehicles_released>3){
                (*vehicle)->flight_timing += GLOBAL_TIME - TIME_INCREMENT + EPSILON;
            }
            else{
                (*vehicle)->flight_timing += GLOBAL_TIME;
            }
            cout << (*vehicle)->company << " (Address: " << vehicle << ") has finished charging and left the station and fly till: "<< (*vehicle)->flight_timing << endl;
            charging_station_map.erase(*vehicle);
        }
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

    /* ceating unorder_map for address references */
    unordered_map<Vehicle*, Vehicle*> current_vehicle;
    
    /* track vehicles already that are in cahrging station */
    unordered_map<Vehicle*, Vehicle*> in_charging_queue;
    
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


    /* running the simulation till the desired time period */
    while(GLOBAL_TIME <= GLOBAL_END_TIME + EPSILON){
        cout << "Global time: " << GLOBAL_TIME << endl;

        /* processes each vehicle */
        for(int i = 0; i < NUMBER_OF_VEHICLES; i++) {
            Vehicle* latest_vehicle = random_selected_vehicles[i];
            if(current_vehicle.find(latest_vehicle) == current_vehicle.end()) {
                current_vehicle[latest_vehicle] = latest_vehicle;
            }            
            cout<< "checking flight time for flight "<< i<< " "<< current_vehicle[latest_vehicle]->company<< " at "<< latest_vehicle << ": "<< current_vehicle[latest_vehicle]->flight_timing<<endl;
            /* checks if the vehicle is still in the air */
            if(GLOBAL_TIME <= current_vehicle[latest_vehicle]->flight_timing) {
                cout << "Flight " << i << " time in air left for " << current_vehicle[latest_vehicle]->company << " at " << latest_vehicle << ": " << current_vehicle[latest_vehicle]->flight_timing - GLOBAL_TIME << " hours." << endl;
            }
            else {
                /* ensure the vehicle enters the queue only once */
                if (!in_charging_queue[current_vehicle[latest_vehicle]]) {
                    in_charging_queue[current_vehicle[latest_vehicle]] = latest_vehicle;
                    cout << "Flight "<< i << " "<< (in_charging_queue[current_vehicle[latest_vehicle]])->company << " at " << in_charging_queue[current_vehicle[latest_vehicle]] << " has landed and is waiting for charging." << endl;
                    (in_charging_queue[current_vehicle[latest_vehicle]])->waiting_in_charging_waiting_queue = true;
                }

                if((in_charging_queue[current_vehicle[latest_vehicle]])->in_charging_station == true){
                    if((GLOBAL_TIME - (in_charging_queue[current_vehicle[latest_vehicle]])->flight_timing) >= (in_charging_queue[current_vehicle[latest_vehicle]])->time_to_charge){
                        cout<<"Flight "<< i << " ";
                        charging_station.release_charger(&in_charging_queue[current_vehicle[latest_vehicle]]);
                        (in_charging_queue[current_vehicle[latest_vehicle]])->in_charging_station = false;
                        (in_charging_queue[current_vehicle[latest_vehicle]])->waiting_in_charging_waiting_queue = false;
                        (in_charging_queue[current_vehicle[latest_vehicle]])->flight_timing = GLOBAL_TIME;
                        if (in_charging_queue.find(current_vehicle[latest_vehicle]) != in_charging_queue.end()) {
                            in_charging_queue.erase(current_vehicle[latest_vehicle]);
                        }
                    }
                    else{
                        cout << "Flight "<< i << " " << (in_charging_queue[current_vehicle[latest_vehicle]])->company << " at " << in_charging_queue[current_vehicle[latest_vehicle]] << " is still in charging with time left: "<< (in_charging_queue[current_vehicle[latest_vehicle]])->time_to_charge - GLOBAL_TIME + (in_charging_queue[current_vehicle[latest_vehicle]])->start_charge_time << "h" << endl;
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
                    cout << "No available chargers for flight "<< i << " "<< (in_charging_queue[current_vehicle[latest_vehicle]])->company << " at " << in_charging_queue[current_vehicle[latest_vehicle]] << ". Waiting in queue." << endl;
                }
            }
        }
        
        /* assign waiting vehicles at the end if free charger found */
        while (charging_station.is_charging_station_free() && !in_charging_queue.empty()) {
            auto first_entry = in_charging_queue.begin();
            Vehicle* next_vehicle = first_entry->first;
            if (charging_station.assign_vehicle_to_charger(&next_vehicle)) {
                first_entry = in_charging_queue.erase(first_entry);
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

/* 
Author: Shivam Shah
Reason: Joby Aviation C assignment
Date: 03/17/25
Time: 10:33 PM

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

using namespace std;

/* global time variable */
double GLOBAL_TIME = 0.0;

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
        double charging_time = 0;
        int faults = 0;
        double passenger_miles = 0;
        
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

    /* picking 20 random vehicles */
    cout << "Randomly selecting 20 vehicles from the vehicle list: " << endl;
    for (int i = 0; i < 20; i++) {
        int choice = random_number(gen);
        Vehicle* select_vehicle = nullptr;

        switch (choice) {
            case 1: select_vehicle = Alpha; break;
            case 2: select_vehicle = Bravo; break;
            case 3: select_vehicle = Charlie; break;
            case 4: select_vehicle = Delta; break;
            case 5: select_vehicle = Echo; break;
        }

        /* storing the selected vehicle in the vector */
        random_selected_vehicles.push_back(select_vehicle);

        cout << "Pick " << i + 1 << ": " << select_vehicle->company << endl;
    }
    
    /* trying to simulate the flight using multi-threading so that all flights starts at the same time and the first to land will get the charging station */
    vector<thread> flight_threads;

    /* creating a priority queue so that shortest flight will get the charging station first */
    priority_queue<Vehicle*, vector<Vehicle*>, vehicle_landing_comparator> landing_queue;

    /* protect access to priority queue */
    mutex queue_mutex;

    /* protect access to global time */
    mutex global_time_mutex;

    /* multi-threading to run the all the vehicles parallelly */
    for (auto& vehicle : random_selected_vehicles) {
        flight_threads.emplace_back([&]() {
            vehicle->simulate_flight();
            
            /* lock queue while adding vehicle */
            {
                lock_guard<mutex> lock(queue_mutex);
                landing_queue.push(vehicle);
            }
        });
    }

    /* to update global time and vehicle status every 0.2 hours */
    thread time_updater([&]() {
        while (GLOBAL_TIME <= 3.0) {
            this_thread::sleep_for(chrono::milliseconds(200)); // Update every 0.2 hours
            lock_guard<mutex> lock(global_time_mutex);
            GLOBAL_TIME += 0.2;

            /* check and update vehicle statuses */
            cout << "Global time: " << GLOBAL_TIME << " hours" << endl;
            for (auto& vehicle : random_selected_vehicles) {
                if (vehicle->is_in_flight(GLOBAL_TIME)) {
                    cout << vehicle->company << " is still in flight with " 
                         << vehicle->flight_timing - GLOBAL_TIME << " hours left." << endl;
                }
            }
        }
    });

    /* waiting for all flights to complete */
    for (auto& t : flight_threads) {
        t.join();
    }

    /* wait for the time updater to complete */
    time_updater.join();

    /* printing the queue */
    cout << "landing logs" << endl;
    while (!landing_queue.empty()) {
        Vehicle* landed_vehicle = landing_queue.top();
        landing_queue.pop();
        cout << landed_vehicle->company << " landed first with flight time: " << landed_vehicle->flight_timing << "h\n";
    }
    
    /* allocating the charging stations */
    
    /* updating the global vehicle values */
    
    /* printing the result */

    /* freeing the allocated memory */
    delete Alpha;
    delete Bravo;
    delete Charlie;
    delete Delta;
    delete Echo;
    
    return 0;
}
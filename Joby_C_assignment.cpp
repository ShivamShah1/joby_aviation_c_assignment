/* 
Author: Shivam Shah
Reason: Joby Aviation C assignment
Date: 03/17/25
Time: 8:42 PM

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

using namespace std;

/* Vehicle class encapsulating eVTOL properties and statistics */
class Vehicle {
    public:
        string company;
        int cruise_speed;
        double battery_capacity;
        double time_to_charge;
        double energy_per_mile;
        int passenger_count;
        double fault_probability;
        
        /* creating a constructor */
        Vehicle(string comp, int speed, double capacity, double charge, double energy, int passengers, double fault)
            : company(comp), cruise_speed(speed), battery_capacity(capacity), time_to_charge(charge), energy_per_mile(energy),
            passenger_count(passengers), fault_probability(fault) {}
};

/* Simulate the flight */
void simulate_flight() {
    
}

/* Charger class to manage charging stations */
class Charger {

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
    uniform_int_distribution<int> random_number(1, 5); // Generates random number from 1 to 5

    /* creating a vector to store the information of the 20 vehicles */
    vector<Vehicle*> random_selected_vehicles;

    /* Pick 20 random vehicles */
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

        /* Store the selected vehicle in the vector */
        random_selected_vehicles.push_back(select_vehicle);

        cout << "Pick " << i + 1 << ": " << select_vehicle->company << endl;
    }
    
    /* try to simulate the flight till it reaches its capacity */
    
    /* creating a vehicle chaging queue */
    
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
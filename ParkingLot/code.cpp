#include <bits/stdc++.h>
using namespace std;

/*
SpotSize
-size
Vehicle
-getType()
Spot
-vehicle
-spotSize
Level
-spots
ParkingTicket
-id
-vehicle
-spot
FeeStrategy
ParkingService
*/

/*
We will create a parking lot with some parking levels and parking spots for some cars, trucks and motorbikes
Then when a vehicle arrives we can try to park it
-If we are able to park it we return the address of the spot for ticket creation 
-Else we return nullptr
Then when the vehicle has to go, we can checkout and free the spot
*/

enum class SpotSize { SMALL, MEDIUM, LARGE };
enum class VehicleType { CAR, BIKE, TRUCK };

/* ---------- VEHICLE ---------- */
class Vehicle {
public:
    virtual VehicleType getType() = 0;
    virtual ~Vehicle() {}
};

class Car : public Vehicle {
public:
    VehicleType getType() override { return VehicleType::CAR; }
};

class Bike : public Vehicle {
public:
    VehicleType getType() override { return VehicleType::BIKE; }
};

class Truck : public Vehicle {
public:
    VehicleType getType() override { return VehicleType::TRUCK; }
};

/* ---------- SPOT ---------- */
class Spot {
    Vehicle* vehicle;
    SpotSize spotSize;

public:
    Spot(SpotSize s) : spotSize(s), vehicle(nullptr) {}

    bool isFree() { return vehicle == nullptr; }

    bool canPark(Vehicle* v) {
        if (!isFree()) return false;

        if (v->getType() == VehicleType::BIKE) return true;
        if (v->getType() == VehicleType::CAR)
            return spotSize == SpotSize::MEDIUM || spotSize == SpotSize::LARGE;
        if (v->getType() == VehicleType::TRUCK)
            return spotSize == SpotSize::LARGE;

        return false;
    }

    void park(Vehicle* v) { vehicle = v; }
    void unpark() { vehicle = nullptr; }
};

/* ---------- LEVEL ---------- */
class Level {
    vector<Spot> spots;

public:
    Level(int car, int bike, int truck) {
        for (int i = 0; i < bike; i++) spots.push_back(Spot(SpotSize::SMALL));
        for (int i = 0; i < car; i++) spots.push_back(Spot(SpotSize::MEDIUM));
        for (int i = 0; i < truck; i++) spots.push_back(Spot(SpotSize::LARGE));
    }

    Spot* parkVehicle(Vehicle* v) {
        for (auto& spot : spots) {
            if (spot.canPark(v)) {
                spot.park(v);
                return &spot;
            }
        }
        return nullptr;
    }
};

/* ---------- TICKET ---------- */
class ParkingTicket {
public:
    string ticketId;
    Vehicle* vehicle;
    Spot* spot;
    int entryTime;

    ParkingTicket(string id, Vehicle* v, Spot* s, int time)
        : ticketId(id), vehicle(v), spot(s), entryTime(time) {}
};

/* ---------- PRICING ---------- */
class FeeStrategy {
public:
    virtual int calculateFee(ParkingTicket* ticket, int exitTime) = 0;
};

class FlatFeeStrategy : public FeeStrategy {
public:
    int calculateFee(ParkingTicket* ticket, int exitTime) override {
        return (exitTime - ticket->entryTime) * 10;
    }
};

/* ---------- PARKING SERVICE ---------- */
class ParkingService {
    vector<Level> levels;
    unordered_map<string, ParkingTicket*> activeTickets;
    FeeStrategy* feeStrategy;
    int timeCounter;
    int ticketCounter;

public:
    ParkingService(int levelCount, int car, int bike, int truck)
        : feeStrategy(nullptr), timeCounter(0), ticketCounter(1) {
        for (int i = 0; i < levelCount; i++)
            levels.emplace_back(car, bike, truck);
    }

    void setFeeStrategy(FeeStrategy* f) { feeStrategy = f; }

    ParkingTicket* park(Vehicle* v) {
        for (auto& level : levels) {
            Spot* spot = level.parkVehicle(v);
            if (spot) {
                string tid = "T" + to_string(ticketCounter++);
                ParkingTicket* ticket =
                    new ParkingTicket(tid, v, spot, timeCounter++);
                activeTickets[tid] = ticket;
                return ticket;
            }
        }
        return nullptr;
    }

    int unpark(string ticketId) {
        ParkingTicket* ticket = activeTickets[ticketId];
        ticket->spot->unpark();

        int fee = feeStrategy? feeStrategy->calculateFee(ticket, timeCounter++): 0;

        activeTickets.erase(ticketId);
        return fee;
    }
};

/* ---------- MAIN ---------- */
int main() {
    ParkingService service(2, 2, 2, 1);
    service.setFeeStrategy(new FlatFeeStrategy());

    Vehicle* car = new Car();
    ParkingTicket* ticket = service.park(car);

    cout << "Fee: " << service.unpark(ticket->ticketId) << endl;
}

/*
Things to learn
1. Treat each class like an enity first, dont overthink much
2. First understand the flow before implementing 
*/
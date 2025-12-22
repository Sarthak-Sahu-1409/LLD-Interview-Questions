#include <bits/stdc++.h>
using namespace std;

/*
FLOW
1. User provides cityId and preferred startTime
2. BookMyShow searches shows
3. User selects a show
4. User selects seats
5. Payment is done
6. Seats are booked
7. Ticket is returned
*/

/*
Movie
-name

Show
-id
-theatreId
-movie
-startTime
-seats

Screen
-id
-shows

Theatre
-id
-screens

City
-id
-theatres

Ticket
-ticketId
-showId
-theatreId
-seats

BookMyShow
-search
-book

*/

enum class SeatStatus { FREE, BOOKED };

/* ---------- MOVIE ---------- */
class Movie {
public:
    string name;
    Movie(string n) : name(n) {}
};

/* ---------- SHOW ---------- */
class Show {
    int id;
    int theatreId; //because we want to show it in ticket 
    Movie movie;
    int startTime;
    vector<SeatStatus> seats;

public:
    Show(int showId, int tId, Movie m, int time, int seatCount)
        : id(showId), theatreId(tId), movie(m), startTime(time) { seats.assign(seatCount, SeatStatus::FREE);}

    int getId() { return id; }
    int getTheatreId() { return theatreId; }
    int getStartTime() { return startTime; }

    bool areSeatsFree(vector<int>& chosenSeats) 
    {
        for (int s : chosenSeats) {
            if (s < 0 || s >= seats.size()) return false;
            if (seats[s] == SeatStatus::BOOKED) return false;
        }
        return true;
    }

    void bookSeats(vector<int>& chosenSeats) 
    {
        for (int s : chosenSeats)
            seats[s] = SeatStatus::BOOKED;
    }
};

/* ---------- SCREEN ---------- */
class Screen {
    int id;
    vector<Show> shows;

public:
    Screen(int i) : id(i) {}

    vector<Show*> getShows(int startTime) { //we want to mutate hence returning pointers instead of simple vector<Show>
        vector<Show*> res;
        for (auto& s : shows)
            if (s.getStartTime() == startTime)
                res.push_back(&s);
        return res;
    }
};

/* ---------- THEATRE ---------- */
class Theatre {
    int id;
    vector<Screen> screens;

public:
    Theatre(int i) : id(i) {}

    vector<Show*> getShows(int startTime) {
        vector<Show*> res;
        for (auto& s : screens) {
            auto temp = s.getShows(startTime);
            res.insert(res.end(), temp.begin(), temp.end());
        }
        return res;
    }
};

/* ---------- CITY ---------- */
class City {
    int id;
    vector<Theatre> theatres;

public:
    City(int i) : id(i) {}

    vector<Show*> getShows(int startTime) {
        vector<Show*> res;
        for (auto& t : theatres) {
            auto temp = t.getShows(startTime);
            res.insert(res.end(), temp.begin(), temp.end());
        }
        return res;
    }
};

/* ---------- PAYMENT SERVICE ---------- */
class PaymentService {
public:
    bool makePayment(int amount) {
        return true;
    }
};

/* ---------- TICKET ---------- */
class Ticket {
public:
    int ticketId;
    int showId;
    int theatreId;
    vector<int> seats;

    Ticket(int id, int sId, int tId, vector<int> seatList) : ticketId(id), showId(sId), theatreId(tId), seats(seatList) {}
};

/* ---------- SEARCH SERVICE ---------- */
class SearchService {
    map<int, City*> cities;

public:
    vector<Show*> searchShows(int cityId, int startTime) {
        return cities[cityId]->getShows(startTime);
    }
};

/* ---------- BOOKING SERVICE ---------- */
class BookingService {
    PaymentService* paymentService;
    int ticketCounter;

public:
    BookingService(PaymentService* ps) : paymentService(ps), ticketCounter(1) {}

    Ticket* bookShow(Show* show, vector<int> seats) {
        if (!show->areSeatsFree(seats)) return nullptr;

        int amount = seats.size() * 200;
        if (!paymentService->makePayment(amount)) return nullptr;

        show->bookSeats(seats);
        return new Ticket(
            ticketCounter++,
            show->getId(),
            show->getTheatreId(),
            seats
        );
    }
};

/* ---------- BOOKMYSHOW ---------- */
class BookMyShow {
    SearchService searchService;
    PaymentService paymentService;
    BookingService bookingService;

public:
    BookMyShow() : bookingService(&paymentService) {}

    vector<Show*> search(int cityId, int startTime) {
        return searchService.searchShows(cityId, startTime);
    }

    Ticket* book(Show* show, vector<int> seats) {
        return bookingService.bookShow(show, seats);
    }
};

/* ---------- MAIN ---------- */
int main() {
    BookMyShow bms;

    int cityId = 1;
    int startTime = 1800;

    auto shows = bms.search(cityId, startTime);
    if (shows.empty()) return 0;

    Ticket* ticket = bms.book(shows[0], {1, 2});

    if (ticket) {
        cout << "Ticket ID: " << ticket->ticketId << endl;
        cout << "Theatre ID: " << ticket->theatreId << endl;
    }
}

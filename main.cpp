#include<bits/stdc++.h>
using namespace std;

#include<pthread.h>

#define fast_io  ios_base::sync_with_stdio(false); cin.tie(NULL)
#define pb push_back
#define int long long int

class User{
private:
    string name;
    char sex;
    int age;
    int loc_x, loc_y;

public:
    User(string name, char sex, int age){

        this -> name = name;
        this -> sex = sex;
        this -> age = age;
        this -> loc_x = 0;
        this -> loc_y = 0;
    }

    void set_location(int loc_x, int loc_y){
        this -> loc_x = loc_x;
        this -> loc_y = loc_y;
    }
};

class Driver{
private:
    string name;
    char sex;
    int age;
    string vehicle_no;

    int loc_x, loc_y;
    bool is_available;

public:
    Driver(string name, char sex, int age, string vehicle_no){

        this -> name = name;
        this -> sex = sex;
        this -> age = age;
        this -> vehicle_no = vehicle_no;
        this -> is_available = true;
    }

    void set_location(int loc_x, int loc_y){
        this -> loc_x = loc_x;
        this -> loc_y = loc_y;
    }

    void change_status(bool available){
        this -> is_available = available;
    }

    string get_driverName(){

        return name;
    }

    pair<int, int> get_location(){

        return make_pair(loc_x, loc_y);
    }

    bool get_availability(){

        return is_available;
    }
};

/*
 * ASSUMPTIONS :
 * user can only book other ride after completion of current_ride.
 * driver is not available if he is currently booked by some other user,
 */

class CabBookingSystem{
private:

    vector<User*> users;
    unordered_map<string, User*> username_to_users;

    vector<Driver*> drivers;
    unordered_map<string, Driver*> drivername_to_drivers;

    map<string, pair<pair<int, int>, pair<int, int> > > users_req_ride;
    map<string, string> users_ride_driver;

    map<string, int> earning;
    pthread_mutex_t driver_lock;

public:
    CabBookingSystem(){

       pthread_mutex_init(&driver_lock, nullptr);
    }

    User* add_user(string name, char sex, int age){

        User* user = new User(name, sex, age);
        users.pb(user);

        username_to_users[name] = user;

        return user;
    }

    void update_userLocation(string name, int loc_x, int loc_y){

        User* user = nullptr;
        if(username_to_users.find(name) == username_to_users.end()){

            cout << "No user with name : " << name << '\n';
            return;
        }

        user = username_to_users[name];
        user -> set_location(loc_x, loc_y);

        cout << "Updated user location successfully \n";
    }

    Driver* add_driver(string name, char sex, int age, string vehicle_no){

        Driver* driver = new Driver(name, sex, age, vehicle_no);
        drivers.pb(driver);

        drivername_to_drivers[name] = driver;

        return driver;
    }

    void update_driverLocation(string name, int loc_x, int loc_y){

        Driver* driver = nullptr;

        if(drivername_to_drivers.find(name) == drivername_to_drivers.end()){

            cout << "No driver with name : " << name << '\n';
            return;
        }

        driver = drivername_to_drivers[name];
        driver -> set_location(loc_x, loc_y);

        cout << "Updated Driver location successfully \n";
    }

    void change_driver_status(string driver_name, bool status){

        Driver* driver = nullptr;

        if(drivername_to_drivers.find(driver_name) == drivername_to_drivers.end()){

            cout << "No driver with name : " << driver_name << '\n';
            return;
        }

        driver = drivername_to_drivers[driver_name];
        driver -> change_status(status);

        cout << "Driver status updated sucessfully \n";
    }


    void find_ride(string user_name, pair<int, int> src, pair<int, int> dest){

        vector<string> available_drivers;

        for(Driver* d : drivers){

            if(!(d -> get_availability())){
                continue;
            }

            pair<int, int> loc = d -> get_location();
            double dis = (double)(loc.first - src.first) * (loc.first - src.first) + (loc.second - src.second) * (loc.second - src.second);
            dis = sqrt(dis);

            if(dis > 5){
                continue;
            }
            available_drivers.pb(d -> get_driverName());
        }

        users_req_ride[user_name] = make_pair(src, dest);

        if(available_drivers.size() == 0){
            cout << "No Driver available nearby !! \n";
            return;
        }

        for(auto &ele : available_drivers){
            cout << ele << '\n';
        }

        //return available_drivers;
    }

    // make it thread safe
    void chose_ride(string user_name, string driver_name){

        Driver* driver = drivername_to_drivers[driver_name];
        if(driver -> get_availability() == false){
            cout << "Driver already booked, Please find a ride again !!";
            return;
        }

        pthread_mutex_lock(&driver_lock);
        change_driver_status(driver_name, false);
        users_ride_driver[user_name] = driver_name;
        pthread_mutex_unlock(&driver_lock);

        cout << "Ride Started \n";
    }

    void calculate_bill(string user_name){

        if(users_req_ride.find(user_name) == users_req_ride.end()){

            cout << "Ride not found for user : " << user_name << '\n';
            return;
        }

        auto src = users_req_ride[user_name].first;
        auto dest = users_req_ride[user_name].second;

        string driver_name = users_ride_driver[user_name];

        double dis = (double)(dest.first - src.first) * (dest.first - src.first) + (dest.second - src.second) * (dest.second - src.second);
        int dd = (ceil)(sqrt(dis));

        if(earning.find(driver_name) == earning.end()){
            earning[driver_name] = dd;
        } else{
            earning[driver_name] += dd;
        }

        users_req_ride.erase(user_name);
        users_ride_driver.erase(user_name);

        change_driver_status(driver_name, true);

        update_userLocation(user_name, dest.first, dest.second);
        update_driverLocation(driver_name, dest.first, dest.second);

        cout << "Bill : " << dd << '\n';
    }

    void get_total_earnings(){

        for(auto ele : drivers){
            string driver_name = ele -> get_driverName();

            if(earning.find(driver_name) == earning.end()){
                earning[driver_name] = 0;
            }

            cout << driver_name << " : " << "$" << earning[driver_name] << '\n';
        }
    }
};

signed main(){

    fast_io;

    CabBookingSystem system;

    system.add_user("Abhishek", 'M', 23);
    system.add_user("Rahul", 'M', 29);
    system.add_user("Nandini", 'F', 22);

    system.update_userLocation("Abhishek", 0, 0);
    system.update_userLocation("Rahul", 10, 0);
    system.update_userLocation("Nandini", 15, 6);

    system.add_driver("Driver1", 'M', 22, "KA-01-12345");
    system.add_driver("Driver2", 'M', 29, "KA-01-12355");
    system.add_driver("Driver3", 'M', 24, "KA-01-12365");

    system.update_driverLocation("Driver1", 10, 1);
    system.update_driverLocation("Driver2", 11, 10);
    system.update_driverLocation("Driver3", 5, 3);

    system.find_ride("Abhishek", make_pair(0, 0), make_pair(20, 1));

    system.find_ride("Rahul", make_pair(10, 0), make_pair(15, 3));
    system.chose_ride("Rahul", "Driver1");
    system.calculate_bill("Rahul");


    system.change_driver_status("Driver1", false);
    system.find_ride("Nandini", make_pair(15, 6), make_pair(20, 4));

    system.get_total_earnings();

    return 0;
}

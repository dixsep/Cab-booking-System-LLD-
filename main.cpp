#include<bits/stdc++.h>
using namespace std;

#include<pthread.h>

#include<memory>

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
    }

    string get_name() const{

        return name;
    }

    pair<int, int> get_location() const{

        return make_pair(loc_x, loc_y);
    }

    void set_location(int loc_x, int loc_y){

        this -> loc_x = loc_x;
        this -> loc_y = loc_y;
    }

    void set_name(string name){

        this -> name = name;
    }

    void set_sex(char sex){
        this -> sex = sex;
    }

    void set_age(int age){
        this -> age = age;
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
    Driver(string name, char sex, int age, string vehicle_no, int loc_x, int loc_y){

        this -> name = name;
        this -> sex = sex;
        this -> age = age;
        this -> vehicle_no = vehicle_no;
        this -> loc_x = loc_x;
        this -> loc_y = loc_y;
        this -> is_available = true;
    }

    string get_name(){

        return name;
    }

    pair<int, int> get_location() const{

        return make_pair(loc_x, loc_y);
    }

    void set_location(int loc_x, int loc_y){
        this -> loc_x = loc_x;
        this -> loc_y = loc_y;
    }

    void set_availability(bool is_available){
        this -> is_available = is_available;
    }

    bool check_availability() const{

        return is_available;
    }
};

class UserManager{
private:

    unordered_map<string, User*> name_to_user;

public:
    void add_user(string name, char sex, int age){

         User* user = new User(name, sex, age);
         name_to_user[name] = user;
         cout << "User added successfully \n";
    }

    void update_user(string name, string updated_name, char updated_sex, int updated_age){

        if(name_to_user.find(name) == name_to_user.end()){
            cout << "User with name : " << name << " Not found !! \n";
            return;
        }

        User* user = name_to_user[name];
        name_to_user.erase(name);
        user -> set_name(updated_name);
        user -> set_sex(updated_sex);
        user -> set_age(updated_age);

        name_to_user[updated_name] = user;

        cout << "User updated successfully \n";
    }

    void updated_user_location(string name, pair<int, int> updated_loc){

        if(name_to_user.find(name) == name_to_user.end()){
            cout << "User with name : " << name << " Not found !! \n";
            return;
        }

        User* user = name_to_user[name];
        user -> set_location(updated_loc.first, updated_loc.second);

        cout << "Updated User location successfully \n";
    }

    User* get_user_by_name(string name){

        if(name_to_user.find(name) == name_to_user.end()){
            cout << "User with name : " << name << " Not found !! \n";
            return nullptr;
        }

        return name_to_user[name];
    }
};

class DriverManager{
private:

    unordered_map<string, Driver*> name_to_driver;
    vector<string> drivers;

public:
    void add_driver(string name, char sex, int age, string vehicle_no, int loc_x, int loc_y){

        Driver* driver = new Driver(name, sex, age, vehicle_no, loc_x, loc_y);
        name_to_driver[name] = driver;
        drivers.pb(name);
        cout << "Driver added successfully \n";
    }

    void updated_driver_location(string name, pair<int, int> updated_loc){

        if(name_to_driver.find(name) == name_to_driver.end()){
            cout << "Driver with name : " << name << " Not found !! \n";
            return;
        }

        Driver* driver = name_to_driver[name];
        driver -> set_location(updated_loc.first, updated_loc.second);

        cout << "Updated Driver location successfully \n";
    }

    void change_driver_status(string name, bool status){

        if(name_to_driver.find(name) == name_to_driver.end()){
            cout << "Driver with name : " << name << " Not found !! \n";
            return;
        }

        Driver* driver = name_to_driver[name];
        driver -> set_availability(status);

        cout << "Driver's Status updated successfully \n";
    }

    Driver* get_driver_by_name(string name){

        if(name_to_driver.find(name) == name_to_driver.end()){
            cout << "User with name : " << name << " Not found !! \n";
            return nullptr;
        }

        return name_to_driver[name];
    }

    vector<string> get_all_drivers(){

        return drivers;
    }
};

class RideManager{
private:

    unordered_map<string, pair<string, vector<int> > > ongoing_rides;
    DriverManager* driver_manager;

    //

    unordered_map<string, vector<int> > user_drop_loc;

    unordered_map<string, int> earnings;

    pthread_mutex_t driver_lock;

    // user_name : rider_name , {sx, sy, dx, dy}
public:

    RideManager(DriverManager* driver_manager){

        this -> driver_manager = driver_manager;
        pthread_mutex_init(&driver_lock, nullptr);

    }

    void find_ride(string user_name, int sx, int sy, int dx, int dy){

        if(ongoing_rides.find(user_name) != ongoing_rides.end()){

            cout << "Can only book ride after completing the ongoing ride \n";
            return;
        }

        vector<int> v;
        v.pb(sx); v.pb(sy); v.pb(dx); v.pb(dy);
        user_drop_loc[user_name] = v;

        vector<string> drivers = driver_manager -> get_all_drivers();
        vector<string> available_drivers;

        for(auto ele : drivers){

            Driver* d = driver_manager -> get_driver_by_name(ele);
            auto loc = d -> get_location();

            if(d -> check_availability() == false){
                continue;
            }

            double dis = (double)(sx - loc.first) * (sx - loc.first) + (sy - loc.second) * (sy - loc.second);
            dis = sqrt(dis);

            if(dis < 5){
                available_drivers.pb(ele);
            }
        }

        for(string name : available_drivers){
            cout << name << " ";
        }
        cout << '\n';
    }

    void chose_ride(string user_name, string driver_name){

        pthread_mutex_lock(&driver_lock);

        Driver* driver = driver_manager -> get_driver_by_name(driver_name);

        if(driver -> check_availability() == false){

            cout << "Driver booked , find a ride again !! \n";
            return;
        }

        auto loc_cord = user_drop_loc[user_name];
        ongoing_rides[user_name] = make_pair(driver_name, loc_cord);


        driver ->set_availability(false);
        cout << "Ride Started \n";

        pthread_mutex_unlock(&driver_lock);
    }

    void calculate_bill(string user_name){

        auto ele = ongoing_rides[user_name];
        string driver_name = ele.first;

        int sx = ele.second[0]; int sy = ele.second[1]; int dx = ele.second[2]; int dy = ele.second[3];

        ongoing_rides.erase(user_name);
        user_drop_loc.erase(user_name);

        double bill = (sx - dx) * (sx - dx) + (sy - dy) * (sy - dy);

        bill = sqrt(bill);
        bill = ceil(bill);

        earnings[driver_name] += bill;
    }

    void find_total_earnings(){

        vector<string> drivers = driver_manager -> get_all_drivers();

        for(string driver : drivers){

            if(earnings.find(driver) == earnings.end()){
                earnings[driver] = 0;
            }

            cout << driver << ": $" << earnings[driver] << '\n';
        }
    }

    ~RideManager(){

        pthread_mutex_destroy(&driver_lock);
    }

};

class CabBookingSystem{
private:

    UserManager* user_manager;
    DriverManager* driver_manager;
    RideManager* ride_manager;

    /*
      unique_ptr<UserManager> user_manager;
      unique_ptr<DriverManager> driver_manager;
      unique_ptr<RideManager> ride_manager;
     */

public:

    CabBookingSystem(){

        user_manager = new UserManager();
        driver_manager = new DriverManager();
        ride_manager = new RideManager(driver_manager);

        /*
         *  user_manager = make_unique<UserManager>();
         *  driver_manager = make_unique<DriverManager>();
         *  ride_manager = make_unique<RideManager>(driver_manager.get());
         */
    }

    //USER
    void add_user(string name, char sex, int age){
        user_manager->add_user(name, sex, age);
    }
    void update_user(string name, string updated_name, char updated_sex, int updated_age){
        user_manager->update_user(name, updated_name, updated_sex, updated_age);
    }
    void updated_user_location(string name, pair<int, int> updated_loc){
        user_manager->updated_user_location(name, updated_loc);
    }

    //DRIVER
    void add_driver(string name, char sex, int age, string vehicle_no, int loc_x, int loc_y){
        driver_manager->add_driver(name, sex, age, vehicle_no, loc_x, loc_y);
    }

    void updated_driver_location(string name, pair<int, int> updated_loc){
        driver_manager->updated_driver_location(name, updated_loc);
    }

    void change_driver_status(string name, bool status){
        driver_manager->change_driver_status(name, status);
    }

    //RIDE
    void find_ride(string user_name, int sx, int sy, int dx, int dy){
        ride_manager->find_ride(user_name, sx, sy, dx, dy);
    }

    void chose_ride(string user_name, string driver_name){
        ride_manager->chose_ride(user_name, driver_name);
    }

    void calculate_bill(string user_name){
        ride_manager->calculate_bill(user_name);
    }

    void find_total_earnings(){
        ride_manager->find_total_earnings();
    }

    ~CabBookingSystem(){

        //destructor
        delete user_manager;
        delete driver_manager;
        delete ride_manager;
    }
};


signed main(){

    fast_io;

    CabBookingSystem system;

    system.add_user("Abhishek", 'M', 23);
    system.add_user("Rahul", 'M', 29);
    system.add_user("Nandini", 'F', 24);

    system.updated_user_location("Abhishek", make_pair(0, 0));
    system.updated_user_location("Rahul", make_pair(10, 0));
    system.updated_user_location("Nandini", make_pair(15, 6));

    system.add_driver("Driver1", 'M', 26, "KA-01-12345", 10, 1);
    system.add_driver("Driver2", 'M', 23, "KA-01-12346", 11, 10);
    system.add_driver("Driver3", 'M', 36, "KA-01-12347", 5, 3);

    system.find_ride("Abhishek", 0, 0, 20, 1);

    system.find_ride("Rahul", 10, 0, 15, 3);
    system.chose_ride("Rahul", "Driver1");
    system.calculate_bill("Rahul");

    system.updated_user_location("Rahul", make_pair(15, 3));
    system.updated_driver_location("Driver1", make_pair(15, 3));

    system.change_driver_status("Driver1", false);

    system.find_ride("Nandini", 15, 6, 20, 4);

    system.find_total_earnings();

    // we can also use smart pointers to avoid memory leaks
    // unique_ptr or shared_ptr;
}

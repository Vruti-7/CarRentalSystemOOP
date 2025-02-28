#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <limits> // For numeric_limits
#include <iomanip> // For setw
#include <stdexcept>
#include <conio.h>
#include <ctime>
#include <chrono>
#include "LoginUser.h"

using namespace std;


int main() {
    LoginUser l;
    l.loadCarsFromFile(); // Load car data from the file when the program starts
    l.loadBookingsFromFile(); // Load booking data from the file when the program starts

    // Load user data from file
    ifstream userFile("users.txt");

    string userLine;
    while (getline(userFile, userLine)) {
        stringstream ss(userLine);
        string username, password, role;
        ss >> username >> password >> role;
        l.users.push_back({ username, password, role });
    }
    userFile.close();

    while (true) {

        if (!l.isLoggedIn) {
            system("cls"); // Clear the screen (for Windows)
            cout << "**********************************" << endl;
            cout << "   Car Rental System_VG" << endl;
            cout << "**********************************" << endl;
            cout << "Select an option:\n";
            cout << "1. Login\n";
            cout << "2. Register User\n";
            cout << "3. Exit\n";

            int choice;
            cin >> choice;

            switch (choice) {
            case 1:
                l.login();
                break;
            case 2:
                l.registerUser();
                break;
            case 3:
                cout << "Exiting...\n";
                return 0;
            default:
                cout << "Invalid choice. Please select again.\n";
            }
        }
        else {
            cout << "Logged in as: " << l.currentUser.username << " (" << l.currentUser.role << ")" << endl;
            cout << "Select an option:\n";

            if (l.currentUser.role == "admin") {
                cout << "1. Add Car\n";
                cout << "2. Update Car\n";
                cout << "3. Delete Car\n";
                cout << "9. Manage Bookings\n"; // Added option for admin to manage bookings
            }
            else if (l.currentUser.role == "customer") {
                cout << "4. Show Available Cars\n";
                cout << "5. Book Car\n";
                cout << "6. Show My Bookings\n";
            }

            cout << "7. Logout\n";
            cout << "8. Exit\n";

            int choice;
            cin >> choice;

            switch (choice) {
            case 1:
                if (l.currentUser.role == "admin") {
                    l.addCar();
                }
                else {
                    cout << "Only admin can add cars." << endl;
                }
                break;
            case 2:
                if (l.currentUser.role == "admin") {
                    l.updateCar();
                }
                else {
                    cout << "Only admin can update cars." << endl;
                }
                break;
            case 3:
                if (l.currentUser.role == "admin") {
                    l.deleteCar();
                }
                else {
                    cout << "Only admin can delete cars." << endl;
                }
                break;
            case 4:
                if (l.currentUser.role == "customer") {
                    l.showAvailableCars();
                }
                else {
                    cout << "Only customers can view available cars." << endl;
                }
                break;
            case 5:
                if (l.currentUser.role == "customer") {
                    l.bookCar();
                }
                else {
                    cout << "Only customers can book cars." << endl;
                }
                break;
            case 6:
                if (l.currentUser.role == "customer") {
                    l.showBookings();
                }
                else {
                    cout << "Only customers can view bookings." << endl;
                }
                break;
            case 7:
                l.isLoggedIn = false;
                cout << "Logged out.\n";
                break;
            case 8:
                cout << "Exiting...\n";
                return 0;
            case 9:
                if (l.currentUser.role == "admin") {
                    l.manageBookings(); // Call the new function for managing bookings
                }
                else {
                    cout << "Only admins can manage bookings." << endl;
                }
                break;
            default:
                cout << "Invalid choice. Please select again.\n";
            }
        }
    }
    // Save car data to the file before exiting
    l.saveCarsToFile();

    // Save booking data to the file before exiting
    l.saveBookingsToFile();

    return 0;
}



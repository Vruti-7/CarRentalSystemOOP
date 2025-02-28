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

using namespace std;
class LoginUser {

public:
    struct User {
        string username;
        string password;
        string role;
    };

    struct Car {
        int id = 0;
        string make = "";
        string model = "";
        int year = 0;
        double mileage = 0.0;
        bool availableNow = false;
        int minRentPeriod = 0;
        int maxRentPeriod = 0;
    };

    enum class BookingStatus {
        Pending,
        Approved,
        Rejected
    };

    struct Booking {
        int bookingID = 0;
        int carID = 0;
        string username = "";
        string startDate = "";
        string endDate = "";
        BookingStatus status = BookingStatus::Pending; // Default status is Pending
        double rentalFee = 0.0; // New field for rental fee
    };

    vector<User> users;
    vector<Car> cars;
    vector<Booking> bookings;
    int nextCarID = 1;
    int nextBookingID = 1;

    bool isLoggedIn = false;
    User currentUser;

    string getPasswordInput() {
        string password;
#ifdef _WIN32
        char ch;
        while ((ch = _getch()) != '\r') {
            if (ch == '\b') {
                if (!password.empty()) {
                    cout << "\b \b"; // Clear the character on backspace
                    password.pop_back();
                }
            }
            else {
                cout << '*';
                password += ch;
            }
        }
        cout << endl;
#else
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~ECHO; // Turn off echoing of characters
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        getline(cin, password);

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
        return password;
    }

    bool login() {
        string username, password;
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter Password: ";
        password = getPasswordInput(); // Call the getPasswordInput() function

        for (const User& user : users) {
            if (user.username == username && user.password == password) {
                isLoggedIn = true;
                currentUser = user;
                cout << "Logged in successfully." << endl;
                return true;
            }
        }

        cout << "Invalid username or password. Login failed." << endl;
        return false;
    }

    void registerUser() {
        string username, password, role;
        cout << "Enter username: ";
        cin >> username;

        // Check if the entered username already exists
        for (const User& user : users) {
            if (user.username == username) {
                cout << "Username not available. Please choose a different username.\n";
                return;
            }
        }

        cout << "Enter password: ";
        password = getPasswordInput(); // Call the getPasswordInput() function

        cout << "Enter role (admin or customer): ";
        cin >> role;

        users.push_back({ username, password, role });
        cout << "User registered successfully.\n";

        ofstream userOutFile("users.txt", ios::app);
        if (userOutFile.is_open()) {
            userOutFile << username << " " << password << " " << role << "\n";
            userOutFile.close();
            cout << "User data saved to file.\n";
        }
        else {
            cerr << "Error opening user file for writing.\n";
        }
    }

    void saveCarsToFile() {
        ofstream file("cars.csv");

        if (!file) {
            cerr << "Error opening file for writing." << endl;
            return;
        }

        file << "ID,Make,Model,Year,Mileage,AvailableNow,MinRentPeriod,MaxRentPeriod\n";

        for (const Car& car : cars) {
            file << car.id << "," << car.make << "," << car.model << "," << car.year << ","
                << car.mileage << "," << (car.availableNow ? 1 : 0) << "," << car.minRentPeriod << ","
                << car.maxRentPeriod << "\n";
        }

        file.close();
        cout << "Car records saved successfully." << endl;
    }

    void addCar() {
        if (!isLoggedIn) {
            cout << "You need to be logged in to add a car." << endl;
            return;
        }

        if (currentUser.role != "admin") {
            cout << "Only admin can add cars." << endl;
            return;
        }

        Car car;
        int highestID = 0;

        // Load existing car data from the file and find the highest ID
        ifstream file("cars.csv");
        if (!file.fail()) {
            string line;
            while (getline(file, line)) {
                stringstream ss(line);
                string field;
                getline(ss, field, ',');

                try {
                    int carID = stoi(field);
                    highestID = max(highestID, carID);
                }
                catch (const std::exception& e) {
                    // Do nothing here, just catch the exception
                }
            }
            file.close();
        }

        car.id = highestID + 1; // Assign a unique ID

        // Display the generated car ID to the admin
        if (currentUser.role == "admin") {
            cout << "Generated Car ID: " << car.id << endl;
        }

        cout << "Enter car make: ";
        cin >> ws; // Consume whitespace
        getline(cin, car.make);

        cout << "Enter car model: ";
        cin >> ws; // Consume whitespace
        getline(cin, car.model);

        while (true) {
            cout << "Enter car year: ";
            cin >> car.year;

            if (cin.fail() || car.year < 2000 || car.year > 2023) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Year must be between 2000 and 2023." << endl;
            }
            else {
                break;
            }
        }

        while (true) {
            cout << "Enter car mileage: ";
            cin >> car.mileage;

            if (cin.fail() || car.mileage < 1) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Mileage must be a non-negative number(It can not be string or negative number)." << endl;
            }
            else {
                break;
            }
        }

        while (true) {
            cout << "Is the car available now? (1 for yes, 0 for no): ";
            cin >> car.availableNow;

            if (cin.fail() || (car.availableNow != 0 && car.availableNow != 1)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter 0 or 1." << endl;
            }
            else {
                break;
            }
        }

        // Get and validate minRentPeriod (in hours)
        while (true) {
            cout << "Enter minimum rent period (in hours, >= 24): ";
            cin >> car.minRentPeriod;

            if (cin.fail() || car.minRentPeriod < 24) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Minimum rent period must be at least 24 hours." << endl;
            }
            else {
                break;
            }
        }

        // Get and validate maxRentPeriod (in hours)
        while (true) {
            cout << "Enter maximum rent period (in hours, <= 120): ";
            cin >> car.maxRentPeriod;

            if (cin.fail() || car.maxRentPeriod < 24 || car.maxRentPeriod > 120) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Maximum rent period must be between 24 and 120 hours." << endl;
            }
            else {
                break;
            }
        }

        cars.push_back(car);

        // Append new car data to the CSV file
        ofstream outFile("cars.csv", ios::app);
        if (!outFile) {
            cerr << "Error opening cars.csv for writing." << endl;
            return;
        }
        outFile << car.id << "," << car.make << "," << car.model << "," << car.year << ","
            << car.mileage << "," << car.availableNow << "," << car.minRentPeriod << ","
            << car.maxRentPeriod << "\n";
        outFile.close();

        cout << "Car added successfully." << endl;
    }

    void updateCar() {
        if (!isLoggedIn) {
            cout << "You need to be logged in to update a car." << endl;
            return;
        }

        if (currentUser.role != "admin") {
            cout << "Only admin can update cars." << endl;
            return;
        }

        int carID;
        cout << "Enter car ID to update: ";
        cin >> carID;

        bool carFound = false; // Flag to track if the car was found for updating

        // Load existing car data from the file and find the highest ID
        ifstream file("cars.csv");
        ofstream tempFile("temp_cars.csv"); // Temporary file to hold updated data
        string line;

        while (getline(file, line)) {
            stringstream ss(line);
            string field;
            getline(ss, field, ','); // Extract the ID field

            int currentCarID;
            try {
                currentCarID = stoi(field);
            }
            catch (const std::exception& e) {
                tempFile << line << "\n"; // Copy the line to the temporary file as is
                continue; // Skip processing this line and move to the next iteration
            }

            if (currentCarID == carID) {
                // Car found, update its data
                Car car;
                car.id = currentCarID;
                ss >> car.make >> car.model >> car.year >> car.mileage
                    >> car.availableNow >> car.minRentPeriod >> car.maxRentPeriod;

                cout << "Enter updated car make: ";
                cin >> ws; // Consume whitespace
                getline(cin, car.make);

                cout << "Enter updated car model: ";
                cin >> ws; // Consume whitespace
                getline(cin, car.model);

                while (true) {
                    cout << "Enter updated car year: ";
                    cin >> car.year;

                    if (cin.fail() || car.year < 2000 || car.year > 2023) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid input. Year must be between 2000 and 2023." << endl;
                    }
                    else {
                        break;
                    }
                }

                while (true) {
                    cout << "Enter updated car mileage: ";
                    cin >> car.mileage;

                    if (cin.fail() || car.mileage < 1) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid input. Mileage must be a non-negative number (It cannot be a string or negative number)." << endl;
                    }
                    else {
                        break;
                    }
                }

                while (true) {
                    cout << "Is the car available now? (1 for yes, 0 for no): ";
                    cin >> car.availableNow;

                    if (cin.fail() || (car.availableNow != 0 && car.availableNow != 1)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid input. Please enter 0 or 1." << endl;
                    }
                    else {
                        break;
                    }
                }

                // Get and validate updated minRentPeriod (in hours)
                while (true) {
                    cout << "Enter updated minimum rent period (in hours, >= 24): ";
                    cin >> car.minRentPeriod;

                    if (cin.fail() || car.minRentPeriod < 24) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid input. Minimum rent period must be at least 24 hours (1 day)." << endl;
                    }
                    else {
                        break;
                    }
                }

                // Get and validate updated maxRentPeriod (in hours)
                while (true) {
                    cout << "Enter updated maximum rent period (in hours, <= 120): ";
                    cin >> car.maxRentPeriod;

                    if (cin.fail() || car.maxRentPeriod < 24 || car.maxRentPeriod > 120) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid input. Maximum rent period must be between 24 and 120 hours (1 and 5 days)." << endl;
                    }
                    else {
                        break;
                    }
                }

                // Update the car data in the temporary file
                tempFile << car.id << "," << car.make << "," << car.model << ","
                    << car.year << "," << car.mileage << ","
                    << car.availableNow << "," << car.minRentPeriod << ","
                    << car.maxRentPeriod << "\n";

                carFound = true; // Mark car as found
            }
            else {
                // Copy other car data to the temporary file as is
                tempFile << line << "\n";
            }
        }

        file.close();
        tempFile.close();

        // Replace the original cars.csv file with the temporary file
        if (carFound) {
            remove("cars.csv");
            rename("temp_cars.csv", "cars.csv");
            cout << "Car updated successfully." << endl;
        }
        else {
            cout << "Car not found with ID " << carID << "." << endl;
            remove("temp_cars.csv"); // Remove the temporary file
        }
    }

    void deleteCar() {
        if (!isLoggedIn) {
            cout << "You need to be logged in to delete a car." << endl;
            return;
        }

        if (currentUser.role != "admin") {
            cout << "Only admin can delete cars." << endl;
            return;
        }

        int carID;
        cout << "Enter car ID to delete: ";
        cin >> carID;

        bool carFound = false; // Flag to track if the car was found for deletion

        // Load existing car data from the file and find the highest ID
        ifstream file("cars.csv");
        if (!file) {
            cerr << "Error opening cars.csv for reading." << endl;
            return;
        }

        ofstream tempFile("temp_cars.csv"); // Temporary file to hold updated data
        if (!tempFile) {
            cerr << "Error opening temp_cars.csv for writing." << endl;
            return;
        }

        string line;

        while (getline(file, line)) {
            stringstream ss(line);
            string field;
            if (getline(ss, field, ',')) {
                try {
                    int currentCarID = stoi(field);

                    if (currentCarID == carID) {
                        // Car found, mark it as deleted
                        carFound = true;
                        continue; // Skip writing this line to temp file
                    }
                }
                catch (const std::exception& e) {
                    // Handle conversion errors if needed
                }
            }

            // Copy other car data to the temporary file as is
            tempFile << line << "\n";
        }

        file.close();
        tempFile.close();

        // Replace the original cars.csv file with the temporary file
        if (carFound) {
            remove("cars.csv");
            rename("temp_cars.csv", "cars.csv");
            cout << "Car deleted successfully." << endl;
        }
        else {
            cout << "Car not found with ID " << carID << "." << endl;
            remove("temp_cars.csv"); // Remove the temporary file
        }
    }
    // Function to save bookings data to the file
    void saveBookingsToFile() {
        ofstream file("bookings.csv");

        if (!file) {
            cerr << "Error opening file for writing." << endl;
            return;
        }

        file << "BookingID,CarID,Username,StartDate,EndDate,Status,RentalFee\n";

        for (const Booking& booking : bookings) {
            // Map the BookingStatus enum back to an integer for storage
            int status = static_cast<int>(booking.status);
            file << booking.bookingID << "," << booking.carID << "," << booking.username << ","
                << booking.startDate << "," << booking.endDate << "," << status << ","
                << booking.rentalFee << "\n";
        }

        file.close();
        cout << "Booking records saved successfully." << endl;
    }

    // Function to load cars data from the cars.csv file
    void loadCarsFromFile() {
        ifstream file("cars.csv");

        if (!file) {
            cerr << "Error opening file for reading." << endl;
            return;
        }

        cars.clear(); // Clear existing car data

        string line;
        getline(file, line); // Read and discard the header line

        while (getline(file, line)) {
            stringstream ss(line);
            Car car;
            string data;

            getline(ss, data, ',');
            car.id = stoi(data);

            getline(ss, car.make, ',');
            getline(ss, car.model, ',');
            getline(ss, data, ',');
            car.year = stoi(data);

            getline(ss, data, ',');
            car.mileage = stod(data);

            getline(ss, data, ',');
            car.availableNow = (data == "1");

            getline(ss, data, ',');
            car.minRentPeriod = stoi(data);

            getline(ss, data, '\n');
            car.maxRentPeriod = stoi(data);

            cars.push_back(car);
        }

        file.close();
        cout << "Car records loaded successfully." << endl;
    }

    void manageBookings() {
        cout << "Select an option:\n";
        cout << "1. List All Bookings\n";
        cout << "2. Approve Booking\n";
        cout << "3. Reject Booking\n";
        cout << "4. Go back\n";

        int choice;
        cin >> choice;

        switch (choice) {
        case 1: {
            cout << "All Bookings:\n";
            cout << "-------------------------------------------------------------------------------------------\n";
            cout << setw(10) << "Booking ID" << " | " << setw(6) << "Car ID"
                << " | " << setw(10) << "Start Date" << " | " << setw(10) << "End Date"
                << " | " << setw(15) << "Username" << " | " << setw(8) << "Status" << endl;
            cout << "-------------------------------------------------------------------------------------------\n";

            for (const Booking& booking : bookings) {
                string status;
                if (booking.status == BookingStatus::Pending) {
                    status = "Pending";
                }
                else if (booking.status == BookingStatus::Approved) {
                    status = "Approved";
                }
                else if (booking.status == BookingStatus::Rejected) {
                    status = "Rejected";
                }

                cout << setw(10) << booking.bookingID << " | " << setw(6) << booking.carID
                    << " | " << setw(10) << booking.startDate << " | " << setw(10) << booking.endDate
                    << " | " << setw(15) << booking.username << " | " << setw(8) << status << endl;
            }
            break;
        }
        case 2: {
            int bookingID;
            cout << "Enter the Booking ID to approve: ";
            cin >> bookingID;

            Booking* selectedBooking = nullptr;
            for (Booking& booking : bookings) {
                if (booking.bookingID == bookingID) {
                    selectedBooking = &booking;
                    break;
                }
            }

            if (selectedBooking) {
                if (selectedBooking->status == BookingStatus::Pending) {
                    selectedBooking->status = BookingStatus::Approved;
                    cout << "Booking approved successfully.\n";
                    // Update the booking status in the file
                    saveBookingsToFile();
                }
                else {
                    cout << "This booking has already been approved.\n";
                }
            }
            else {
                cout << "Booking with ID " << bookingID << " not found.\n";
            }
            break;
        }
        case 3: {
            int bookingID;
            cout << "Enter the Booking ID to reject: ";
            cin >> bookingID;

            Booking* selectedBooking = nullptr;
            for (Booking& booking : bookings) {
                if (booking.bookingID == bookingID) {
                    selectedBooking = &booking;
                    break;
                }
            }

            if (selectedBooking) {
                if (selectedBooking->status == BookingStatus::Pending) {
                    selectedBooking->status = BookingStatus::Rejected;
                    cout << "Booking rejected successfully.\n";

                    // Update the booking status in the file
                    saveBookingsToFile();
                }
                else {
                    cout << "This booking has already been approved or rejected.\n";
                }
            }
            else {
                cout << "Booking with ID " << bookingID << " not found.\n";
            }
            break;
        }
        case 4:
            cout << "Going back...\n";
            break;
        default:
            cout << "Invalid choice. Please select again.\n";
            break;
        }
    }

    void showAvailableCars() {
        if (!isLoggedIn) {
            cout << "You need to be logged in to view available cars." << endl;
            return;
        }

        if (currentUser.role != "customer") {
            cout << "Only customers can view available cars." << endl;
            return;
        }

        ifstream carFile("cars.csv");
        if (!carFile) {
            cerr << "Error opening cars.csv for reading." << endl;
            return;
        }

        cout << "Available Cars:\n";
        cout << "-----------------------------------------------------------------------------------\n";
        cout << setw(4) << "ID" << " | " << setw(10) << "Make" << " | "
            << setw(8) << "Model" << " | " << setw(4) << "Year" << " | "
            << setw(8) << "Mileage" << " | " << setw(13) << "MinRentPeriod" << " | "
            << setw(13) << "MaxRentPeriod" << endl;
        cout << "-----------------------------------------------------------------------------------\n";

        string line;
        getline(carFile, line); // Skip the header line
        while (getline(carFile, line)) {
            stringstream ss(line);
            Car car;
            string data;

            getline(ss, data, ',');
            car.id = stoi(data);

            getline(ss, data, ',');
            car.make = data;

            getline(ss, data, ',');
            car.model = data;

            getline(ss, data, ',');
            car.year = stoi(data);

            getline(ss, data, ',');
            car.mileage = stod(data);

            getline(ss, data, ',');
            car.availableNow = stoi(data);

            getline(ss, data, ',');
            car.minRentPeriod = stoi(data);

            getline(ss, data, ',');
            car.maxRentPeriod = stoi(data);

            if (car.availableNow) {
                cout << setw(4) << car.id << " | " << setw(10) << car.make << " | "
                    << setw(8) << car.model << " | " << setw(4) << car.year << " | "
                    << setw(8) << car.mileage << " | " << setw(13) << car.minRentPeriod << " | "
                    << setw(13) << car.maxRentPeriod << endl;
            }
        }

        carFile.close();
    }

    double calculateRentalFee(int durationInHours) {

        return 20 * (durationInHours + 24);
    }
    // Function to calculate the rental duration in hours
    int calculateDurationInHours(const string& start, const string& end) {
        // Parse the date strings
        tm startTime = {};
        istringstream(start) >> get_time(&startTime, "%Y-%m-%d");
        time_t startTimestamp = mktime(&startTime);

        tm endTime = {};
        istringstream(end) >> get_time(&endTime, "%Y-%m-%d");
        time_t endTimestamp = mktime(&endTime);


        // Calculate duration in hours
        double seconds = difftime(endTimestamp, startTimestamp);
        return static_cast<int>(seconds / 3600); // 3600 seconds in an hour
    }

    void bookCar() {
        if (!isLoggedIn) {
            cout << "You need to be logged in to book a car." << endl;
            return;
        }

        if (currentUser.role != "customer") {
            cout << "Only customers can book cars." << endl;
            return;
        }

        int carID;
        cout << "Enter the ID of the car you want to book: ";
        cin >> carID;

        bool carFound = false;

        for (Car& car : cars) {
            if (car.id == carID) {
                if (car.availableNow) {
                    // Found the car to book
                    Booking booking;
                    booking.carID = car.id;
                    booking.username = currentUser.username;

                    // Get the current date
                    time_t now = time(0);
                    tm currentDate;
                    localtime_s(&currentDate, &now);

                    cout << "Enter start date (YYYY-MM-DD): ";
                    cin >> booking.startDate;

                    // Parse the start date
                    tm start;
                    istringstream(booking.startDate) >> get_time(&start, "%Y-%m-%d");

                    // Check if the start date is valid
                    if (!istringstream(booking.startDate) || start.tm_year != currentDate.tm_year || start.tm_mon != currentDate.tm_mon || start.tm_mday <= currentDate.tm_mday) {
                        cout << "Invalid start date." << endl;
                        return;
                    }

                    cout << "Enter end date (YYYY-MM-DD): ";
                    cin >> booking.endDate;

                    // Parse the end date
                    tm end;
                    istringstream(booking.endDate) >> get_time(&end, "%Y-%m-%d");

                    // Check if the end date is valid
                    if (!istringstream(booking.endDate) || end.tm_year != currentDate.tm_year || end.tm_mon != currentDate.tm_mon || end.tm_mday <= currentDate.tm_mday) {
                        cout << "Invalid end date." << endl;
                        return;
                    }

                    // Find the maximum booking ID and increment it
                    int maxBookingID = 0;
                    for (const Booking& existingBooking : bookings) {
                        if (existingBooking.bookingID > maxBookingID) {
                            maxBookingID = existingBooking.bookingID;
                        }
                    }
                    booking.bookingID = maxBookingID + 1;

                    // Calculate the rental duration in hours (you may need to adjust this logic)
                    int durationInHours = calculateDurationInHours(booking.startDate, booking.endDate);

                    // Calculate the rental fee
                    booking.rentalFee = calculateRentalFee(durationInHours);

                    bookings.push_back(booking);

                    // Update the car's availability in memory
                    car.availableNow = false;

                    cout << "Car booked successfully. Rental fee: $" << fixed << setprecision(2) << booking.rentalFee << endl;
                    carFound = true;
                    break;
                }
                else {
                    cout << "Car with ID " << carID << " is not available for booking." << endl;
                    carFound = true;
                    break;
                }
            }
        }

        if (!carFound) {
            cout << "Car with ID " << carID << " was not found." << endl;
        }

        // Display available cars after booking
        showAvailableCars();

        // Save all car data to the file after booking
        saveCarsToFile(); // Update the car data in the file
        saveBookingsToFile(); // Update the booking data in the file
    }

    void loadBookingsFromFile() {
        ifstream file("bookings.csv");

        if (!file) {
            cerr << "Error opening file for reading." << endl;
            return;
        }

        bookings.clear();

        string line;
        getline(file, line); // Read and discard the header line

        while (getline(file, line)) {
            stringstream ss(line);
            Booking booking;
            string data;

            getline(ss, data, ',');
            booking.bookingID = stoi(data);

            getline(ss, data, ',');
            booking.carID = stoi(data);

            getline(ss, booking.username, ',');
            getline(ss, booking.startDate, ',');
            getline(ss, booking.endDate, ',');

            getline(ss, data, ',');
            int status = stoi(data);
            // Map the loaded status to the BookingStatus enum
            booking.status = static_cast<BookingStatus>(status);

            getline(ss, data, '\n');
            booking.rentalFee = stod(data);

            bookings.push_back(booking);
        }

        file.close();
        cout << "Booking records loaded successfully." << endl;
    }
    void showBookings() {
        if (!isLoggedIn) {
            cout << "You need to be logged in to view bookings." << endl;
            return;
        }

        if (currentUser.role != "customer") {
            cout << "Only customers can view bookings." << endl;
            return;
        }

        cout << "Your Bookings:\n";
        cout << "------------------------------------------------------------\n";
        cout << setw(10) << "Booking ID" << " | " << setw(6) << "Car ID"
            << " | " << setw(10) << "Start Date" << " | " << setw(10) << "End Date" << endl;
        cout << "------------------------------------------------------------\n";

        for (const Booking& booking : bookings) {
            if (booking.username == currentUser.username) {
                cout << setw(10) << booking.bookingID << " | " << setw(6) << booking.carID
                    << " | " << setw(10) << booking.startDate << " | " << setw(10) << booking.endDate << endl;
            }
        }
    }
};
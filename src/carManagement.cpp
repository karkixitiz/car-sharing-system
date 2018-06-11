#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "log.cpp"
#include "util.cpp"
#include "car.cpp"
#include "../include/carManagement.hpp"

CarManagement::CarManagement()
{
    Car c1;
    c1.setName("Bugatti Veyron");
    c1.setMfdYear(2016);
    c1.setColor("Matte Black");
    c1.setPrice(45.00);
    c1.setDoorsQty(4);
    c1.setTransmission("Automatic");

    cars.push_back(c1);
}

void CarManagement::run()
{
    char input_response;
    bool continue_loop = true;

    do {
        Util::showLines("=");
        Util::centerText("Car Management Interface");
        Util::showLines("=");
        std::cout << "1. View All Cars" << std::endl;
        std::cout << "2. Add New Car" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Type one of the options above: ";

        std::cin >> input_response;
        std::cin.ignore();  // ignore the newline character

        std::cout << std::endl;

        switch(input_response) {
            case '1':
            listAllCarsUI();
            break;

            case '2':
            addNewCarUI();
            break;

            case '0':
            continue_loop = false;
            std::cout << "Bye!" << std::endl;
            break;

            default:
            Log::logError("Invalid option");
            break;
        }

        Util::showLines("=");
        std::cout << std::endl;
    } while (continue_loop);
}

void CarManagement::listAllCarsUI()
{
    std::string input_response;

    Util::showLines("-");
    Util::centerText("Listing all Cars");
    Util::showLines("-");

    for (int i=0; i<cars.size(); i++) {
        std::cout << std::to_string(i + 1) + ". " + cars[i].getBriefInfo() << std::endl;
        Util::showLines(".");
    }

    while(true) {
        std::cout << "Press the car number to view details, or 0 to exit to home screen" << std::endl;

        std::getline(std::cin, input_response);

        if (input_response == "0")
        {
            break;
        }

        try {
            viewCarDetailsUI(std::stoi(input_response));
        } catch(const std::invalid_argument& e) {
            Log::logError("Please select a valid number");
        } catch(const std::range_error& e) {
            Log::logError(e.what());
        }
        Util::showLines("-");
    }
}

void CarManagement::viewCarDetailsUI(int car_number)
{
    if (car_number > cars.size())
    {
        throw std::range_error("Car number invalid");
    }

    car_number = car_number - 1;    // because array index start from 0

    Util::showLines("-");
    Util::centerText("Viewing Car Details");
    Util::showLines("-");
    std::cout << cars[car_number].getAllInfo() << std::endl;
    Util::showLines("-");
}

void CarManagement::addNewCarUI()
{
    std::string input_name, input_mfd_year, input_color, input_price, input_doors_qty, input_transmission;
    char save_response;

    Util::showLines("-");
    Util::centerText("Add New Car");
    Util::showLines("-");

    std::cout << "Enter new car details below" << std::endl;
    std::cout << std::endl;

    std::cout << "Name: ";
    std::getline(std::cin, input_name);

    std::cout << "Manufactured year: ";
    std::getline(std::cin, input_mfd_year);

    std::cout << "Color: ";
    std::getline(std::cin, input_color);

    std::cout << "Price/day(€): ";
    std::getline(std::cin, input_price);

    std::cout << "No. of doors: ";
    std::getline(std::cin, input_doors_qty);

    std::cout << "Transmission: ";
    std::getline(std::cin, input_transmission);

    std::cout << "Press y to save details, any other key to discard ";
    std::cin.get(save_response);
    std::cout << std::endl;

    // Save details of the new car, if user wants it to be saved
    if (save_response == 'y')
    {
        try {
            addCar(input_name, input_mfd_year, input_color, input_price, input_doors_qty, input_transmission);

            Log::logSuccess("Car details saved!");
        } catch (std::invalid_argument e) {
            Log::logError("Data not saved. Please make sure\ndata is entered in correct format.");
        } catch (...) {
            Log::logError("Data not saved. Please try again.");
        }
    } else {
        std::cout << "Car details not saved!" << std::endl;
    }

    Util::showLines("-");
}

void CarManagement::addCar(std::string input_name, std::string input_mfd_year, std::string input_color, std::string input_price, std::string input_doors_qty, std::string input_transmission)
{
    if (input_name == "")
    {
        throw std::invalid_argument("Car name not supplied");
    }

    try {
        Car new_car;

        int year = input_mfd_year == "" ? 0 : std::stoi(input_mfd_year);
        int price = input_price == "" ? 0.0 : std::stof(input_price);
        int doors_qty = input_doors_qty == "" ? 0 : std::stoi(input_doors_qty);

        new_car.setName(input_name);
        new_car.setMfdYear(year);
        new_car.setColor(input_color);
        new_car.setPrice(price);
        new_car.setDoorsQty(doors_qty);
        new_car.setTransmission(input_transmission);

        cars.push_back(new_car);
    } catch (...) {
        throw;
    }
}

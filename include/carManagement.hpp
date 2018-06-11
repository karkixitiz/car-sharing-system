#ifndef _CAR_MGMT_HPP_
#define _CAR_MGMT_HPP_

/**
 * Class for car management interface
 */
class CarManagement
{
public:
    CarManagement();

    void run();

    int getCarsQty() {
        return cars.size();
    }

    void viewCarDetailsUI(int car_number);
    void listAllCarsUI();
    void addNewCarUI();

    void addCar(std::string input_name, std::string input_mfd_year, std::string input_color, std::string input_price, std::string input_doors_qty, std::string input_transmission);

private:
     std::vector<Car> cars = {};
};

#endif // _CAR_MGMT_HPP_

#ifndef _CAR_HPP_
#define _CAR_HPP_

#include <string>

/**
 * Class for car related information
 */
class Car
{
public:
    Car();

    std::string getName() const {
        return m_name;
    }

    int getMfdYear() const {
        return m_mfd_year;
    }

    std::string getColor() const {
        return m_color;
    }

    float getPrice() const {
        return m_price;
    }

    std::string getTransmission() const {
        return m_transmission;
    }

    int getDoorsQty() const {
        return m_doors_qty;
    }

    std::string getBriefInfo();
    std::string getAllInfo();

    void setName(std::string name);
    void setMfdYear(int mfd_year);
    void setColor(std::string color);
    void setPrice(float price);
    void setDoorsQty(int doors_qty);
    void setTransmission(std::string transmission);

protected:

private:
    std::string m_name = "";
    int m_mfd_year = 0;
    std:: string m_color = "";
    float m_price = 0.0;
    int m_doors_qty = 0;
    std:: string m_transmission = "";
};

#endif // _CAR_HPP_

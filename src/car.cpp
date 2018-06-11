#include <string>

#include "../include/car.hpp"

Car::Car() {}

void Car::setName(std::string name)
{
    m_name = name;
}

void Car::setMfdYear(int mfd_year)
{
    m_mfd_year = mfd_year;
}

void Car::setColor(std::string color)
{
    m_color = color;
}

void Car::setPrice(float price)
{
    m_price = price;
}

void Car::setDoorsQty(int doors_qty)
{
    m_doors_qty = doors_qty;
}

void Car::setTransmission(std::string transmission)
{
    m_transmission = transmission;
}

std::string Car::getBriefInfo()
{
    return "Name: " + getName() + "\t\t " + std::to_string(getPrice()) + "€/day";
}

std::string Car::getAllInfo()
{
    return "Name: " + getName() + "\n" +
            "Manufactured year: " + std::to_string(getMfdYear()) + "\n" +
            "Color: " + getColor() + "\n" +
            "Price/day: " + std::to_string(getPrice()) + "€" + "\n" +
            "No. of doors: " + std::to_string(getDoorsQty()) + "\n" +
            "Transmission: " + getTransmission();
}

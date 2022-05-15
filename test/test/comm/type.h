#ifndef TYPE_H
#define TYPE_H

#define AMPTEK_DP5_VENDOR_ID 0x10C4
#define AMPTEK_DP5_PRODUCT_ID 0x842A
#include <iostream>
#include <memory>
#include <thread>
#include <memory>
#include <stdint.h>
#include <stdio.h>
#include <functional>
#include <string>
#include <chrono>

using std::cout;
using std::endl;
using std::make_shared;
using std::shared_ptr;
using std::weak_ptr;
using std::string;

constexpr uint8_t MAXUSBCOUNT = 10;

#endif

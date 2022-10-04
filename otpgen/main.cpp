// SPDX-License-Identifier: BSD-3-Clause
// SPDX-FileCopyrightText: 2022 Avinal Kumar <avinal.xlvii@gmail.com>

#include <stdio.h>

#include <algorithm>
#include <cstring>
#include <ctime>
#include <string>

#include "badger2040.hpp"
#include "common/pimoroni_common.hpp"
#include "pico/platform.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "topt.h"

using namespace pimoroni;

Badger2040 badger;

constexpr int WIDTH = 296;
constexpr int HEIGHT = 128;
constexpr long IST_OFFSET = 19800;

std::string time(const long& st) {
  // Add offset for IST
  std::time_t t = st;
  std::tm* tm_gmt = std::gmtime(&t);
  // return time as string in HH:MM:SS format
  return std::to_string(tm_gmt->tm_hour) + ":" +
         std::to_string(tm_gmt->tm_min) + ":" + std::to_string(tm_gmt->tm_sec) +
         " IST";
}

std::string get_totp(const long& st) {
  // echo "@vinal47Kbadger2FA" | base32
  // IB3GS3TBNQ2DOS3CMFSGOZLSGJDECCQ=
  uint8_t hmacKey[] = {0x40, 0x76, 0x69, 0x6e, 0x61, 0x6c, 0x34,
                       0x37, 0x4b, 0x62, 0x61, 0x64, 0x67, 0x65,
                       0x72, 0x32, 0x46, 0x41, 0x0a};
  TOTP(hmacKey, 19, 10);
  auto code = std::to_string(getCodeFromTimestamp(st));
  if (code.length() < 6) {
    code = std::string(6 - code.length(), '0') + code;
  }
  return code;
}

void draw(const std::string& code, const std::string& time) {
  int32_t hms_width = badger.measure_text(code, 1.8f);
  int hms_offset = WIDTH / 2 - hms_width / 2;
  int32_t time_width = badger.measure_text(time, 0.7f);
  int time_offset = WIDTH / 2 - time_width / 2;
  badger.pen(15);
  badger.clear();
  badger.pen(0);
  badger.thickness(5);
  badger.text(code, hms_offset, 54, 1.8f);
  badger.thickness(2);
  badger.text(time, time_offset, 110, 0.7f);
  badger.update();
}

void draw_timer(int seconds) {
  while (seconds) {
    badger.pen(15);
    badger.clear();
    badger.pen(0);
    badger.thickness(2);
    badger.text(std::to_string(seconds), 143, 8, 0.6f);
    badger.partial_update(140, 0, 16, 16);
    sleep_ms(1000);
    seconds--;
  }
}

void init_draw(const std::string& message) {
  int32_t msg_width = badger.measure_text(message, 0.7f);
  int msg_offset = WIDTH / 2 - msg_width / 2;
  badger.pen(15);
  badger.clear();
  badger.pen(0);
  badger.thickness(3);
  badger.font("sans");
  badger.text(message, msg_offset, 65, 0.7f);
  badger.update();
}

int main() {
  stdio_init_all();

  sleep_ms(20);

  // Unix time starts on 1st January 1970
  long start_time;

  badger.init();
  badger.update_speed(2);
  init_draw("Waiting for RTC sync...");
  scanf("%d", &start_time);
  init_draw("RTC sync complete :)");
  while (true) {
    // Increase unix time by 1 second
    draw(get_totp(start_time), time(start_time + IST_OFFSET));
    draw_timer(10);
    badger.led(100);
    start_time += 10;
    badger.led(0);
  }
}

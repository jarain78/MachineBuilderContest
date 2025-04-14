cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(mbc_nrf9151_core_v1)

target_sources(app PRIVATE
  src/main.c
  src/biosignal.c
  src/ecg.c
  src/i2c_helper.c
  src/spo2.c
  src/max30102.c
)
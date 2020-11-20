#!/usr/bin/expect -f

set timeout -1

spawn ./menu

expect "Enter a number that corresponds to your choice > "
send -- "2\r"

expect "Enter a number that corresponds to your choice > "
send -- "2\r"

expect "Enter dates (YYYY-MM-DD - YYYY-MM-DD) > "
send -- "2003-01-10 - 2003-04-21\r"


# 7933 -> stock for product with id = S10_1678
expect "10102\t| 2003-01-10\t| 2003-01-14"
expect "10103\t| 2003-01-29\t| 2003-02-02"
expect "10104\t| 2003-01-31\t| 2003-02-01"
expect "10105\t| 2003-02-11\t| 2003-02-12"
expect "10106\t| 2003-02-17\t| 2003-02-21"
expect "10107\t| 2003-02-24\t| 2003-02-26"
expect "10108\t| 2003-03-03\t| 2003-03-08"
expect "10109\t| 2003-03-10\t| 2003-03-11"
expect "10110\t| 2003-03-18\t| 2003-03-20"

send "\n\r"

expect "10111\t| 2003-03-25\t| 2003-03-30"
expect "10112\t| 2003-03-24\t| 2003-03-29"
expect "10113\t| 2003-03-26\t| 2003-03-27"
expect "10114\t| 2003-04-01\t| 2003-04-02"
expect "10115\t| 2003-04-04\t| 2003-04-07"
expect "10116\t| 2003-04-11\t| 2003-04-13"
expect "10117\t| 2003-04-16\t| 2003-04-17"
expect "10118\t| 2003-04-21\t| 2003-04-26"

puts "\n--------------------------------OK\n"

expect "Enter a number that corresponds to your choice > "
send -- "4\r"

expect "Enter a number that corresponds to your choice > "
send -- "4\r"

expect eof

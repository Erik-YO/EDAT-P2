#!/usr/bin/expect -f

set timeout -1

spawn ./menu

expect "Enter a number that corresponds to your choice > "
send -- "2\r"

expect "Enter a number that corresponds to your choice > "
send -- "3\r"

expect "Enter ordernumber > "
send -- "10100\r"

# Modificacion
# expect "Order Date=2003-1-6 - status=Shipped"
expect "Total sum = 10223.83"

send -- "\n\r"
#
expect "S24_3969\t| 49\t| 35.29"
expect "S18_2248\t| 50\t| 55.09"
expect "S18_1749\t| 30\t| 136.00"
expect "S18_4409\t| 22\t| 75.46"

send -- "q\r"

puts "\n--------------------------------OK\n"

expect "Enter a number that corresponds to your choice > "
send -- "4\r"

expect "Enter a number that corresponds to your choice > "
send -- "4\r"

expect eof

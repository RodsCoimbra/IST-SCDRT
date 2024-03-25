clear all
clc
s = serialport("COM4", 115200);
configureTerminator(s, "LF");
n = 0;
data = [];
while n<100
    % Read data from serial port
    data_n =readline(s)  % Read data from serial port
    data =  [data, data_n];
    n = n+1;
end
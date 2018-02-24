close all;
clear;
source('PackGen_1514_stats_test_1.mat');


b=vector';

index=b([1],1:end);
delay=b([2],1:end);
inter=b([3],1:end);


plot(index,delay);
grid on;

title ("Packet Travel Time");
xlabel ("packet");
ylabel ("Delay (microseconds)");
hold on;
figure;

plot(index,inter);
grid on;
title ("Packet Interarrival Time");
xlabel ("packet");
ylabel ("Delay (microseconds)");
hold on;

mean(delay);

mean(inter);

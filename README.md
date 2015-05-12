#About

A Mandelbrot set renderer.

Does 40 renders at increasing zoom levels then quits.

Uses OpenMP and SSE intrinsics to decrease the time the render takes.

Written in C++.

#Timings

This program was tested on two systems. The timings for them are:

Intel 2500k CPU with turbo frequency disabled for fair timings (http://ark.intel.com/products/52210/Intel-Core-i5-2500K-Processor-6M-Cache-up-to-3_70-GHz)


    Original program: 35.2 seconds
    Using OpenMP: 14.6 seconds
    Using SSE: 23.5 seconds
    Using OpenMP and SSE: 6.5 seconds

AMD 64 core machine


    Original program: 53.9 seconds
    Using OpenMP: 2.9 seconds
    Using SSE: 39.6 seconds
    Using OpenMP and SSE: 1.7 seconds
  
  
![Render](https://raw.githubusercontent.com/MikeyRooney/Optimised-Mandelbrot-Renderer/master/Screenshot%20from%202015-05-12%2013%3A45%3A15.png)



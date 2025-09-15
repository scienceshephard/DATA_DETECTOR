# DATA_DETECTOR
<p>his is the core of the problem. The final value of shared_value is not 200,000, which is what it should be if two threads each incremented it 100,000 times correctly (2 * 100,000 = 200,000).</p>

<p>Instead, you got 129,184. This is a much lower number because of the data race. The counter++ operation is not atomic, and the threads are interfering with each other, causing many of the increments to be lost.</P>

## Why the value is wrong:
<p>Imagine both threads read the value 100 at the same time. Both increment their local copy to 101. Both then write 101 back to memory. The counter went from 100 to 101, but it should have gone to 102 because of two increments. One increment was lost. This happens thousands of times during the run, resulting in a final count much lower than 200,000.
</p>

# Why Didn't You See "DATA RACE DETECTED!" Messages?

<p>IN the increment method of the race_exaple.cpp file if you uncomment it you will see the data race detected message.</p>
<p>First cd into the build folder and run 'cmake ..' the run 'make' </p>
<p> Then run this command: valgrind --tool=helgrind ./race_example </p>

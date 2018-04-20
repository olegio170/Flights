In this work you will add a new feature to an airline reservation system
Tickets. In particular, the app user will be able to monitor what flights
Is in progress at a specific time of day. For this purpose a structure will be used
A binary tree that stores information related to the departure and arrival times of the planes.
An example of this structure is shown in the figure:

![Task img](/Task.png)


Each flight node is stored in one tree node. In particular, its code is stored
Flight time, departure time and time of arrival of the airplane. Also, each node stores the maximum time
Between all flights stored in the root node of that node. B.C.,
In the Max Af field. Of the root of the tree has been stored 21 because it is the time it lands
Plane of last flight between those stored in the tree. Also, observe that
Application tree is a binary search tree for hours of departure.

At work, you need to implement the specific data structure that will support the following
Functions:
1) Introduction and deletion of flights. After these operations, the tree should be in proper order
State, ie it should again be a binary search tree in terms of departure time and the
Field Max. Should have the correct value on all nodes.
2) Given a time span, a flight that is in progress at
Time. It is noted that the flights started in space are also considered
Or completed during that period. B.C. If the time interval is [12,14], flight SW489,
CA863 and CA345 are among the flights currently underway.
3) Generalizing the previous question, given a period of time, you should
All flights that are in progress at that time are returned.
4) Given one hour h, the time of arrival of that flight time
Departure before hour h and is completed later in relation to all flights that have time
Departure time h. In this example, if h = 11.30 then the answer should be
16.30 is returned, since this is the arrival time of SW489 which has begun earlier than
At 11.30 and is completed later by all flights that started before 11.30.
5) Given one hour h, the nearest departure time should be returned, relative to h,
Flight departing later than h. B.C. For h = 13.30 the nearest departure time is n
14:00 of flight UA987.
In any case, you should minimize the number of comparisons required during execution
Of these functions. It is also noted that you do not need to perform balancing functions after one
Importing or deleting a flight from the tree.

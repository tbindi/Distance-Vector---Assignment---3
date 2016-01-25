Test 1:
	Silo <-> Bobac <-> Degu
	config_4 , config_3 , config_2

./Main config_4 65520 100 9 30 1
./Main config_3 65520 100 9 30 1
./Main config_2 65520 100 9 30 1


Test 2:
	Silo <-> Bobac <-> Degu
	config_4, config_3, config_2

TTL for Degu should expire after 100 seconds in Silo
TTL for Silo should expire after 100 seconds in Degu
Cost will go back to infinity, If neighbour is alive he will send an advertisement and it should do current time + TTL


Count to Infinity Test, Done
Degu goes down
Bobac updates Degu as infi after 100 seconds
Bobac sends update message to Silo saying infinity
Silo sends that he can reach Degu at 2
Bobac updates as 3 to Degu 
Silo updates as 4 to Degu (should check for next hop as bobac and update to 3+1)

Test 3: Done
	Silo <-> Bobac <-> Degu <-> Silo
	config_4, config_3, config_2

3 nodes and they are in a loop. What happens if one node is reachable and the other is not.
3 nodes will stay updated for a while, periodic updates do not conflict with each other
if one node goes down, then the other two will count to infinity and then stay stable.

Mutex Lock & Unlock working with 3 cycle nodes. Should do testing for 4+ nodes.



Working Test cases:
	1. 3 linear 
		A - B - C

		./Main config 65520 30 16 10 1

		Taking out A & C

		Time-to-Live	: 	30 seconds
		Infinity 		:	16
		Period 			: 	10 seconds

		-> With Split Horizon
			Init Time: 0 for B, 11 for A and C
			Conv Time: 30 for C when A was killed but 10 since the last stable update recvd from B,

		-> Without Split Horizon
			Init Time: 0 for B, 16 for C, 14 for B
			Conv Time: 30 for A when C was killed but 6 for A & 10 for B since the last stable update recvd.


	2. 4 Cyclic 
		A - B
		|	|
		D - C

		./Main config 65520 90 16 10 1

		Taking out D & C

		Time-to-Live	: 	90 seconds
		Infinity 		:	16
		Period 			: 	10 seconds

		-> Without Split Horizon
			Init Time: 4 for D, 1 for A, 3 for B & 11 for C
			Conv Time: 90 for A, B & C when D was killed but 10 for A, B & C since the last stable update recvd.

		-> With Split Horizon
			Init Time: 0 for A, 1 for B, 3 for C & 4 for D
			Conv Time: 90 for A, B & D when C was killed but 7 for A, B & D since the last stable update
			recvd.


	3. 5 Cyclic
		A-------B
		|	  / |
		|   E   |
		| /     |
		D-------C

		./Main config 65520 90 16 10 1

		Taking out E 

		-> Without Split Horizon
			Init Time: 3 for A, 5 for B, 6 for C, 6 for D & 13 for E
			Conv Time:  90 for A, B, C & D when E was killed but 12 for A, 11 for B, 10 for C and 0 for D since the last stable update

		-> With Split Horizon
			Init Time: 5 for A, 5 for B, 8 for C, 6 for D & 15 for E
			Conv Time: 90 for A, B, C & D when E was killed but lesser time than without split horizon.


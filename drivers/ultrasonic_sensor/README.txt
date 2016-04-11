Obstacle Dectectance
- The distance of an obstacle is determined by measuring
  the time it takes for a pulse reflected off of the obstacle
  to be returned to the ultrasonic sensor. The sensor outputs
  a pulse who's width corresponds to the distance of the obstacle.
  Therefore, it necessary to measure the width the output pulse.  

             		 <----- Pulse Width ---->
			 +----------------------+
			 |			|
			 |			|
		---------+			+---------> t
			 ^			^
			 |			|
			 |			|
		    Clear Counter 		+---- Stop Timer
		    Start Timer			      Capture Counter Value

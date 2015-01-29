# RB_Capture
This is a stereo video capture demo based on dual webcam under linux.it use two webcam( one acts as left eyes and the other acts as right eye) based on V4l2 Framework to capture images then mix R channel of left image with GB channel of right image to generate a  anaglyph stereo  image buffer and use SDL2 API to Draw it on the Screen.<br /> 
for experience you will need meet two following conditions:<br />
  	  (1)SDL2 Devel packages is installed <br />
	  (2)you also need a REB-BLUE 3D glasses(very cheap) <br /> 
you can execute make command in the directory to start building to generate RB_Capture Execute object,then you can go to experience it.if you feel stetro effect is not good,you can adjust the margin between left image and right image to make the shade  perfect match and it will make a better anaglyph stereo effect !
    

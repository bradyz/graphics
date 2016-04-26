Team Member 1: Daniel Cano Talamas
Team Member 2: Brady Zhou

Description: 

Vertex normals are recomputed lazily on rotation of any bone, this is so
dark spots on the skeleton move correctly - in the solution, if you rotate
the arm, there is a dark spot that moves on the arm aka normals don't change.

We added shadow projection as taught in class to bake shadows in for a plane.
The shadow updates as the ogre moves.

We generated new textures using Perlin noise (for extra credit).
They can be found in "./extra_textures/" , and they are called 
"clouds.jpg", "circles.jpg" and "lines.jpg".
We wrote all the code, from creating the Perlin noise to making the pictures.
The only exception is that we used a hsl2rgb function in matlab that 
we found on internet, and just for the clouds image we got inspired by 
seeing how to do it on this site: http://lodev.org/cgtutor/randomnoise.html 
Furthermore, we used C++ to generate the matrices of noise and textures, 
but then we produced as output the matrices and wrote a program in Matlab 
to actually display the images and save them as jpg (and the cloud texture 
was actually done in Matlab rather than C++ because we needed the hsl2rgb function).
If you want to see the C++ code we used, it is in "perlin.cc" but as I said,
that is not all the code because we passed output to Matlab. 
We also submitted the script in Matlab that generated the clouds, 
it's called "clouds.m"

We use our clouds texture to create a skybox. We followed the tutorial on
"http://learnopengl.com/#!Advanced-OpenGL/Cubemaps".

Finally, there are planar projections displayed on spheres and the ogre,
you can see the warping of angles for the planar.

Disclaimers:
The light position is edited and static so the shadow projection works.
To view, PRESS "E" FOR EXTRA CREDIT MODE, then "t" to toggle textures like normal.

The skybox is by default off, to turn on, press "o".

Only the left ogre can be clicked and dragged.

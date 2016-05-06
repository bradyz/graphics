Team Member 1: Daniel Cano Talamas
Team Member 2: Brady Zhou

Setup:
Build the project with 
cmake .
make

Description: 

Two bins to run
./bin/main boid
for boids
./bin/main balls
for ball simulation
Use "t" to toggle time, "m" to toggle mesh mode, "p" to add food to boids.

In balls, we implemented basic particle physics and collision detection, 
with octree. angular velocity was too difficult, so we stopped at particles.
sphere sphere and sphere plane collision resolution. We used an octtree for 
nearest neighbor queries and our traversal was not efficient so we just ended
up using brute force n^2 checks.

In boids, we implemented boids, where each bird is a particle that depends on 
its neighbors velocity, the herd's center of mass, and other things, like food.

Disclaimers:
We both also filled out the instructor survey for extra credit!

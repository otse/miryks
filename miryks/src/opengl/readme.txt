based on three.js fundamentals

types - forward declares and general  
group - a point in space. can have other groups. think of the sun.  
groupbounded - its polymorphic with group, but you should only use groupbounded together, or your aabb wont coalesce  
material - it basically canonically sets hints and the shader for any given instance that is rendering  
geometry - as it says  
shader - code that i stole  
GroupDrawer - this is an instance of a group. yes.  
drawgroupflatsorted - just use depth sorting  
renderable - old code  
rendertarget - lets you render without disturbing the main screen  
scene - lets you admit drawgroups that will get rendered, as well as lights  
camera - nice axis locked first person camera, supports custom cameras  
texture - messy dds stuff for skyrim  

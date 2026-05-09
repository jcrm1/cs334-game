# cs334-game
## Build process
1. `cmake -B build`
2. `cmake --build build`

## User Stories
1. As a user, I want to see beautiful terrain
2. As a user, I want to see trees
3. As a user, I want to see natural-looking water sources and formations
   1. Waterfalls
   2. Lakes
   3. Rivers
   4. Puddles
4. As a user, I want to see weather
5. As a user, I want to see different biomes of terrain
6. As a user, I want to move on 3 axis in real time  

Water source and sink points are defined maybe arbitrarily, then water is filled between the two  
Terrain generates in phases:  
1. Ground
2. Biomes from noise?
3. Water
4. Nature/decorations

water texture [resources/Water.jpg](https://commons.wikimedia.org/wiki/File:ISS006-E-37356_-_View_of_Pacific_Ocean.jpg)
(public domain NASA image. not protected by copyright)

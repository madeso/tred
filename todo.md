# todo
* Learn OpenGL(11): Advanced OpenGL (blending, fbo, instanced, aa)
* Learn OpenGL(10): Avanced lighning (gamma, shadows, hdr, parralax, bloom, deferred shading, ssao) https://www.slideshare.net/guerrillagames/deferred-rendering-in-killzone-2-9691589
* Learn OpenGL(4): PBR https://artisaverb.info/PBT.html https://marmoset.co/posts/basic-theory-of-physically-based-rendering/
* remove opengl and replace with https://github.com/bkaradzic/bgfx
* Refactor to "engine"
  - https://www.reddit.com/r/gamedev/comments/38045a/structuring_a_rendering_engine/
  - https://www.reddit.com/r/gamedev/comments/iqqlm0/structuring_my_render_engine/
  - https://www.reddit.com/r/GraphicsProgramming/comments/5dvrd1/opengl_renderer_design_how_i_write_opengl_these/
  - https://www.reddit.com/r/gamedev/comments/60a0o1/opengl_engine_core_architecture_and_efficient/
  - https://www.reddit.com/r/cpp/comments/4p5k0r/how_hard_is_coding_an_opengl_game_engine/
  - https://aliceengine.blogspot.com/2020/01/multi-graphics-api-design.html
  - https://www.gamasutra.com/blogs/NiklasGray/20200630/365640/Prototypes_in_The_Machinery.php
* investigare into profiling: https://github.com/bombomby/optick https://github.com/wolfpld/tracy
* wire frame rendering
* emissive material
* Precomputed radiance transfer: https://docs.microsoft.com/en-us/windows/win32/direct3d9/precomputed-radiance-transfer?redirectedfrom=MSDN http://jankautz.com/publications/prtSIG02.pdf
* bsdf https://en.wikipedia.org/wiki/Bidirectional_scattering_distribution_function
* create sphere mesh function
  - https://github.com/caosdoar/spheres/
  - https://gamedev.stackexchange.com/questions/16585/how-do-you-programmatically-generate-a-sphere
* frame buffer distorion materials
* "ue3 light functions": frustum light & sphere/cube map & interpolate between textures based on distance: https://www.youtube.com/watch?v=1m7T5ay_8DI
* light cookies
* breaking glass: https://www.youtube.com/watch?v=nuHg6_IIyK8
* volumetric lightning, mist, fog, godrays: https://www.youtube.com/watch?v=G0sYTrX3VHI
* Iridescence material
* Debug draw lines, spheres etc (dear imgui to render text in 3d?)
* various particle rendering types (point, axis aligned etc)
  - render icon (textured quad always facing camera, same size, either in 2d or 3d)
  - trail rendering
* Model loading (3)
* Culling
  - http://rastergrid.com/blog/2010/10/hierarchical-z-map-based-occlusion-culling/
  - http://rastergrid.com/blog/2010/02/instance-culling-using-geometry-shaders/
  - https://developer.download.nvidia.com/books/HTML/gpugems/gpugems_ch29.html
  - https://en.wikipedia.org/wiki/Quadtree
  - https://gamedevelopment.tutsplus.com/tutorials/quick-tip-use-quadtrees-to-detect-likely-collisions-in-2d-space--gamedev-374
  - https://people.eecs.berkeley.edu/~jrs/274/bsptreefaq.html
* Particle system
  - https://www.youtube.com/watch?v=2UmLB8JUS5k
  - https://www.youtube.com/watch?v=BYL6JtUdEY0
  - https://www.youtube.com/watch?v=FEA1wTMJAR0
* (dynamic) texture projection
* Decals
* cloud rendering https://twitter.com/Vuthric/status/1286796950214307840?s=19
* outlines https://twitter.com/beakfriends/status/1306298799611162624
* per pixel motion blur
* screen space reflection: https://www.gamasutra.com/blogs/JoostVanDongen/20201022/372121/Screen_Space_Reflections_in_Blightbound.php
* sub surface scattering https://developer.download.nvidia.com/books/HTML/gpugems/gpugems_ch16.html
* Simple 2d rendering (rotated and scaled sprites, tilemap) and batching
* Animations
  - skeleton vs skellington: https://twitter.com/joewintergreen/status/1326432419486994434?s=19
* Dynamic/automatic animations and overrides/animation rigging:
  - https://www.youtube.com/watch?v=Htl7ysv10Qs
* Face animation
* Heightmaps
* grass rendering
  - https://developer.download.nvidia.com/books/HTML/gpugems/gpugems_ch07.html
  - https://www.youtube.com/watch?v=L_Bzcw9tqTc
* mud/snow heightmap rendering/dynamic tesselation
* post process effects:
  - https://www.youtube.com/watch?v=9tjYz6Ab0oc
* water color shader
  - https://twitter.com/AlanZucconi/status/1301120184233713665
  - https://www.bruteforce-games.com/post/watercolor-shader-devblog-13
* NPR
  - https://twitter.com/harryh___h/status/1328006632102526976?s=19
  - https://twitter.com/lgions/status/972178238888964097?s=19
  - https://twitter.com/tomrkobayashi/status/1326189102383857664
* get ideas from gpu gems: https://developer.nvidia.com/gpugems/gpugems2/copyright https://developer.nvidia.com/gpugems/gpugems3/contributors
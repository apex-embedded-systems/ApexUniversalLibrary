project(simple)

add_executable(simple  ${CMAKE_SOURCE_DIR}/../../aul_tree/demo/demo_basic.c )
#Note: 
# The demo_basic.c file shown in the project list is a link to the real file located as shown above
# This file is used by both Kdevelop and Eclipse projects.  Eclipse, in general, is more straight forward 
#  to use.  But, if you already have a code base in KDevelop then all this can still be made to work. 

target_link_libraries(simple ${CMAKE_SOURCE_DIR}/../../aul_tree/_Out_Library_Static_Application_Debug/libauldebug.a)
#-lauldebug -L/home/mike/workspace/aul_tree/_Out_Library_Static_Application_Debug

#include "driver_state.h"
#include <cstring>

driver_state::driver_state()
{
}

driver_state::~driver_state()
{
    delete [] image_color;
    delete [] image_depth;
}

// This function should allocate and initialize the arrays that store color and
// depth.  This is not done during the constructor since the width and height
// are not known when this class is constructed.
void initialize_render(driver_state& state, int width, int height)
{
    state.image_width=width;
    state.image_height=height;
    state.image_color = new pixel[width*height];
    state.image_depth = new float[width*height];

int size = width*height;
	for(int i = 0; i < size; i++) 
    		state.image_color[i] = make_pixel(0,0,0);
   
}

// This function will be called to render the data that has been stored in this class.
// Valid values of type are:
//   render_type::triangle - Each group of three vertices corresponds to a triangle.
//   render_type::indexed -  Each group of three indices in index_data corresponds
//                           to a triangle.  These numbers are indices into vertex_data.
//   render_type::fan -      The vertices are to be interpreted as a triangle fan.
//   render_type::strip -    The vertices are to be interpreted as a triangle strip.
void render(driver_state& state, render_type type)
{
    data_geometry * dg[3];
    switch(type){
        case render_type::triangle : 
		
		
		for(int i = 0; i < state.num_vertices; i = i + state.floats_per_vertex ){
		    dg[i]->data = new float[MAX_FLOATS_PER_VERTEX]; 
		    for(int j = 0; j <= state.floats_per_vertex; j++){
		         dg[i]->data[j] = state.vertex_data[j];
		    }//end for
		    
		}//end fori
		
		const  data_geometry * in1 = dg[0];
		const data_geometry * in2 = dg[1];
		const data_geometry * in3 = dg[2];

		const data_geometry* geo[] = {in1, in2, in3};

		rasterize_triangle(state, geo);
	break; 
	/*case render_type::indexed : 
	break;
	case render_type::fan : 
	break;
	case render_type::strip : 
	break;*/
	
	}//end switch	 
}


// This function clips a triangle (defined by the three vertices in the "in" array).
// It will be called recursively, once for each clipping face (face=0, 1, ..., 5) to
// clip against each of the clipping faces in turn.  When face=6, clip_triangle should
// simply pass the call on to rasterize_triangle.
void clip_triangle(driver_state& state, const data_geometry* in[3],int face)
{
    if(face==6)
    {
        rasterize_triangle(state, in);
        return;
    }
    std::cout<<"TODO: implement clipping. (The current code passes the triangle through without clipping them.)"<<std::endl;
    clip_triangle(state,in,face+1);
}

// Rasterize the triangle defined by the three vertices in the "in" array.  This
// function is responsible for rasterization, interpolation of data to
// fragments, calling the fragment shader, and z-buffering.
void rasterize_triangle(driver_state& state, const data_geometry* in[3])
{
    

    for(int i = 0; i < state.num_vertices ; i++){
	vertex_shader(data_vertex, in[i], state.uniform_data);
        //in[i]->gl_Position = in[i]->gl_Position / state.image_width;
    }

    
}


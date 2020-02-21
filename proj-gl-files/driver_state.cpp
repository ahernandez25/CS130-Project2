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
		int tri_count = 0;
		
		for(int i = 0; i < state.num_vertices; i = i + state.floats_per_vertex ){
		    dg[i]->data = new float[MAX_FLOATS_PER_VERTEX]; 
		    for(int j = 0; j <= state.floats_per_vertex; j++){
		         dg[i]->data[j] = state.vertex_data[j];
		    }//end for
		    tri_count++;
		    if((tri_count % 3) == 0){
			const  data_geometry * in1 = dg[0];
                	const data_geometry * in2 = dg[1];
                	const data_geometry * in3 = dg[2];

                	const data_geometry* geo[] = {in1, in2, in3};

                	rasterize_triangle(state, geo);
		    }
		}//end fori
		
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
    data_geometry out[3];
    data_vertex in2[3];
    	
    /*data_geometry * in1 = in[0];
        data_geometry * in2 = in[1];
	data_geometry * in3 = in[2];
	data_geometry* geo[] = {in[0], in[1], in[2]};*/  

    for(int i = 0; i < 3; i++){
	//data_vertex vertex;
	in2[i].data = in[i]->data;
	state.vertex_shader(in2[i], out[i], state.uniform_data);
        out[i].gl_Position = in[i]->gl_Position / state.image_width;
    }

    float areaABC = (0.5)*( ( (out[1].gl_Position[0] * out[2].gl_Position[1]) - (out[2].gl_Position[0] * out[1].gl_Position[1]) ) 
			-   ( (out[0].gl_Position[0] * out[2].gl_Position[1]) - (out[2].gl_Position[0] * out[0].gl_Position[1]) )  
			+   ( (out[0].gl_Position[0] * out[1].gl_Position[1]) - (out[1].gl_Position[0] * out[0].gl_Position[1]) )
 		    );
    vec2 v1 = {(0.5) * state.image_width * (out[0].gl_Position[0] + 1), (0.5)* state.image_height * (out[0].gl_Position[1] + 1)  };
    vec2 v2 = {(0.5) * state.image_width * (out[1].gl_Position[0] + 1), (0.5)* state.image_height * (out[1].gl_Position[1] + 1)  };
    vec2 v3 = {(0.5) * state.image_width * (out[2].gl_Position[0] + 1), (0.5)* state.image_height * (out[2].gl_Position[1] + 1)  };
   
    state.image_color[(state.image_width * static_cast<int>(v1[0])) + (static_cast<int>(v1[1]) % state.image_width)] = make_pixel(255,255,255);
    state.image_color[(state.image_width * static_cast<int>(v2[0])) + ( static_cast<int>(v2[1]) % state.image_width)] = make_pixel(255,255,255);
    state.image_color[(state.image_width * static_cast<int>(v3[0])) + (static_cast<int>(v3[1]) % state.image_width)] = make_pixel(255,255,255);

    int i;
    int j;
    float alpha = 0;
    float beta = 0;
    float gamma = 0;
    for(int w = 0; w < state.image_width; w++){
	i = (0.5)*state.image_width*(w + 1);
	for(int h = 0; h < state.image_height; h++){
	    j = (0.5)*state.image_height * (h + 1);
	
	    //CALCULATE ALPHA
	    alpha = (0.5)*(  ( (out[1].gl_Position[0]*out[2].gl_Position[1]) - (out[2].gl_Position[0]*out[1].gl_Position[1]) )
	    		 -   ( (w * out[2].gl_Position[1]) - (out[2].gl_Position[0] * h) )
			 +   ( (w * out[1].gl_Position[1]) - (out[1].gl_Position[0] * h) ) 
	     		 ) / areaABC;

	    //CALCULATE BETA 
	    beta = (0.5)*(  ( (w * out[2].gl_Position[1]) - (out[2].gl_Position[0] * h ) )
			-   ( (out[0].gl_Position[0] * out[2].gl_Position[1]) - (out[2].gl_Position[0] * out[0].gl_Position[1]) )
                        +   ( (out[0].gl_Position[0] * h ) - ( w * out[0].gl_Position[1]) )
			) / areaABC;
	

	    //CALCULATE GAMMA
	    gamma = (0.5)*(  ( (out[1].gl_Position[0] * h ) - ( w * out[1].gl_Position[1] ) )
			 -   ( (out[0].gl_Position[0] * h ) - ( w * out[0].gl_Position[1] ) )
			 +   ( (out[0].gl_Position[0] * out[0].gl_Position[1] ) - ( out[1].gl_Position[0] * out[0].gl_Position[1]) )
			) / areaABC;       
	
	    if((alpha + beta + gamma) == 1){
		state.image_color[(state.image_width * j) + (i + state.image_width)] = make_pixel(255,255,255);
	    }
	}//end for height
    }//end for width
    
}


#include "driver_state.h"
#include <cstring>
#include <iostream>
#include <limits>

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
	for(int i = 0; i < size; i++){ 
    		state.image_color[i] = make_pixel(0,0,0);
   		state.image_depth[i] = std::numeric_limits<float>::max();
	}
}

// This function will be called to render the data that has been stored in this class.
// Valid values of type are:
//   render_type::triangle - Each group of three vertices corresponds to a triangle.
//   render_type::indexed -  Each group of three indices in index_data corresponds
//                           to a triangle.  These numbers are indices into vertex_data.
//   render_type::fan -      The vertices are to be interpreted as a triangle fan.
//   render_type::strip -    The vertices are to be interpreted as a triangle strip.
void render(driver_state& state, render_type type){


//	data_geometry * dg1;
//	data_geometry * dg2;
//	data_geometry * dg3;
    	switch(type){
        case render_type::triangle : 
		
	for(int i = 0; i < state.num_vertices; i = i + 3){	

    		data_geometry  out[3];
    		data_vertex dv1;
    		data_vertex dv2;
    		data_vertex dv3;
	
		out[0].data = new float[MAX_FLOATS_PER_VERTEX];
                out[1].data =  new float[MAX_FLOATS_PER_VERTEX];
                out[2].data = new float[MAX_FLOATS_PER_VERTEX];

		dv1.data = new float[MAX_FLOATS_PER_VERTEX];
   		for(int j = 0; j < state.floats_per_vertex; j++)  {
                  	dv1.data[j] = state.vertex_data[(i * state.floats_per_vertex) + j]; 
			out[0].data[j] = dv1.data[j];
		 }

		state.vertex_shader(dv1, out[0], state.uniform_data);

		dv2.data = new float[MAX_FLOATS_PER_VERTEX];
                for(int j = state.floats_per_vertex ; j < 2 * state.floats_per_vertex ; j++){
                    dv2.data[j - state.floats_per_vertex] = state.vertex_data[(i * state.floats_per_vertex) + j];
                    out[1].data[j - state.floats_per_vertex] = dv2.data[j - state.floats_per_vertex];
		 }//end dg2

		state.vertex_shader(dv2, out[1], state.uniform_data);

		dv3.data = new float[MAX_FLOATS_PER_VERTEX];
                for(int j = (2 * state.floats_per_vertex ); j < (3 * state.floats_per_vertex ); j++){
                    dv3.data[j - (2 * state.floats_per_vertex)] = state.vertex_data[(i * state.floats_per_vertex) + j];
                    out[2].data[j - (2 * state.floats_per_vertex)] =dv3.data[j - (2 * state.floats_per_vertex)] ;
		}//end dg3
//std::cerr << std::endl;
		state.vertex_shader(dv3, out[2], state.uniform_data);
    
                data_vertex inVertexShader[3] = {dv1, dv2, dv3};
    		
    
                //std::cerr << "\n\nbefore vertex shader: \n\n";
               
/*
		out[0].data = new float[MAX_FLOATS_PER_VERTEX];
		out[1].data =  new float[MAX_FLOATS_PER_VERTEX];
		out[2].data = new float[MAX_FLOATS_PER_VERTEX]; */
	//	 for(int i = 0; i < 3; i++){
    
                  //  state.vertex_shader(inVertexShader[i], out[i], state.uniform_data);
                   
  //             std::cerr << "in loop. i= " << i << "\tout[0][1]: " << out[i].gl_Position[0] <<", " <<  out[i].gl_Position[1] << std::endl; 
          //      } 
//std::cerr << "after vertex shader\n\n";
			data_geometry * dg1 = &out[0];
        data_geometry * dg2 = &out[1];
        data_geometry * dg3 = &out[2];
			const data_geometry* out2[] = {dg1, dg2, dg3};
  			//const  data_geometry * in1 = out[1];
                	//const data_geometry * in2 = out[2];
                	//const data_geometry * in3 = out[3];

                	//const data_geometry* geo[] = {out2[0], out2[1], out2[2]};
//		for(int i = 0; i < 3; i++){

                  
  //                std::cerr << "in loop. i= " << i << "\tout[0][1]: " << out2[i]->gl_Position[0] <<", " <<  out2[i]->gl_Position[1] << std::endl;
    //           }
                	rasterize_triangle(state, out2);

			delete dv1.data;
			delete dv2.data;
			delete dv3.data;
			delete out[0].data;
			delete out[1].data;
			delete out[2].data;
		
		//end fori
	}	
	break; 
	/*case render_type::indexed : 
 * 	break;
 * 		case render_type::fan : 
 * 			break;
 * 				case render_type::strip : 
 * 					break;*/
	
	}//end switch	  
}


// This function clips a triangle (defined by the three vertices in the "in" array).
// It will be called recursively, once for each clipping face (face=0, 1, ..., 5) to
// clip against each of the clipping faces in turn.  When face=6, clip_triangle should
// simply pass the call on to rasterize_triangle.
void clip_triangle(driver_state& state, const data_geometry* in[3],int face)
{
    vec4 v1 = in[0]->gl_Position;
    vec4 v2 = in[1]->gl_Position;
    vec4 v3 = in[2]->gl_Position;

   

    if(face==6)
    {
        rasterize_triangle(state, in);
        return;
    
    }else if(face == 5)
    {
	if((v1[2] < -v1[3]) && (v2[2] < -v2[3]) && (v3[2] < -v3[3]) ){
            clip_triangle(state, in, face+1);                                                                                                                                                                              }
         
    }else if(face == 4){
	if((v1[2] < v1[3]) && (v2[2] < v2[3]) && (v3[2] < v3[3]) ){
            clip_triangle(state, in, face+1);                                                                                                                                                                              }
         
    }else if (face == 3) {
	if((v1[1] < -v1[3]) && (v2[1] < -v2[3]) && (v3[1] < -v3[3]) ){
            clip_triangle(state, in, face+1);                                                                                                                                                                              }
         
    }else if (face == 2) {
	if((v1[1] < v1[3]) && (v2[1] < v2[3]) && (v3[1] < v3[3]) ){
            clip_triangle(state, in, face+1);                                                                                                                                                                              }
         
    }else if (face == 1) {
        if((v1[0] < -v1[3]) && (v2[0] < -v2[3]) && (v3[0] < -v3[3]) ){
            clip_triangle(state, in, face+1);                                                                                                                                                                              }
         
    } else if (face == 0){
	if((v1[0] < v1[3]) && (v2[0] < v2[3]) && (v3[0] < v3[3]) ){
	    clip_triangle(state, in, face+1);
	}
        
    }
    //std::cout<<"TODO: implement clipping. (The current code passes the triangle through without clipping them.)"<<std::endl;
    //clip_triangle(state,in,face+1);
}

// Rasterize the triangle defined by the three vertices in the "in" array.  This
// function is responsible for rasterization, interpolation of data to
// fragments, calling the fragment shader, and z-buffering.
void rasterize_triangle(driver_state& state, const data_geometry* in[3])
{
	
	vec4 vertices[3];
	for(int k = 0; k < 3; k++)
       		vertices[k] = in[k]->gl_Position / in[k]->gl_Position[3];



 /*   	float areaABC = (0.5)*( ( (vertices[1][0] * vertices[2][1]) - ( vertices[2][0] *  vertices[1][1]) ) 
			  -   ( ( vertices[0][0] *  vertices[2][1]) - ( vertices[2][0] *  vertices[0][1]) )  
			  +   ( ( vertices[0][0] *  vertices[1][1]) - ( vertices[1][0] *  vertices[0][1]) )
 		   		 );
*/



    vec2 v1 = {((0.5) * state.image_width * ( vertices[0][0] + 1)) - 0.5, ((0.5)* state.image_height * ( vertices[0][1] + 1)) - 0.5  };
    vec2 v2 = {((0.5) * state.image_width * ( vertices[1][0] + 1)) - 0.5 ,( (0.5)* state.image_height * ( vertices[1][1] + 1)) - 0.5 };
    vec2 v3 = {((0.5) * state.image_width * ( vertices[2][0] + 1)) - 0.5 ,( (0.5)* state.image_height * ( vertices[2][1] + 1) ) - 0.5  };
  

 
float areaABC = (0.5)*( ( (v2[0] * v3[1]) - ( v3[0] *  v2[1]) )
                          -   ( ( v1[0] *  v3[1]) - ( v3[0] *  v1[1]) )
                          +   ( ( v1[0] *  v2[1]) - ( v2[0] *  v1[1]) )
                                 );

/*
    state.image_color[(static_cast<int>(v1[0])) + (static_cast<int>(v1[1]) * state.image_width)] = make_pixel(255,255,255);
    state.image_color[(static_cast<int>(v2[0])) + ( static_cast<int>(v2[1]) * state.image_width)] = make_pixel(255,255,255);
    state.image_color[(static_cast<int>(v3[0])) + (static_cast<int>(v3[1]) * state.image_width)] = make_pixel(255,255,255);
//std::cerr << "after image color set\n\n"; */
   
    float alpha = 0;
    float beta = 0;
    float gamma = 0; 
    for(int w = 0; w < state.image_width; w++){
	

	for(int h = 0; h < state.image_height; h++){
	
	    //CALCULATE ALPHA
	    alpha = (0.5)*(  ( ( v2[0] *  v3[1]) - (v3[0] * v2[1]) )
	    		 -   ( (w * v3[1]) - (v3[0] * h) )
			 +   ( (w * v2[1]) - (v2[0] * h) ) 
	     		 ) / areaABC;

	    //CALCULATE BETA 
	    beta = (0.5)*(  ( (w * v3[1]) - (v3[0] * h ) )
			-   ( (v1[0] * v3[1]) - (v3[0] * v1[1]) )
                        +   ( (v1[0] * h ) - ( w * v1[1]) )
			) / areaABC;
	

	    //CALCULATE GAMMA
	    gamma = (0.5)*(  ( (v2[0] * h ) - ( w * v2[1] ) )
			 -   ( (v1[0] * h ) - ( w * v1[1] ) )
			 +   ( (v1[0] * v2[1] ) - ( v2[0] * v1[1]) )
			) / areaABC;       
//std::cerr << "\nalpha : " << alpha << "\tbeta: " << beta << "\tgamma: " << gamma;	
	    if((alpha >= 0) && (beta >= 0) && (gamma >= 0)){
		data_fragment df;
                df.data = new float[MAX_FLOATS_PER_VERTEX];
		for(int i = 0; i < state.floats_per_vertex; i++){
		    switch(state.interp_rules[i]){        
		        case(interp_type::invalid) : 
                        break;
                        case(interp_type::flat) :  df.data[i] = in[0]->data[i];
		        break;
                        case(interp_type::smooth) : 
			break;
                        case(interp_type::noperspective) : df.data[i] = (in[0]->data[i] * alpha) + (in[1]->data[i] * beta) + (in[2]->data[i] * gamma);
			break;
 

		    }//end switch	
		}//end for switch
		    data_output out;
		    state.fragment_shader(df, out, state.uniform_data ); 
		
		    float newz;
		   
		        newz =  (alpha * vertices[0][2]) + (beta * vertices[1][2]) + (gamma * vertices[2][2]);
			    if( newz < state.image_depth[(state.image_width * h) + (w)] ){
			  	state.image_color[(state.image_width * h) + (w)] = make_pixel(255 * out.output_color[0], 255 * out.output_color[1] ,255 * out.output_color[2]);
			state.image_depth[(state.image_width * h) + (w)] = newz;			
			    }//end if 
		    
		}//end check if in triangle

		//state.image_color[(state.image_width * h) + (w)] = make_pixel(255 * out.output_color[0], 255 * out.output_color[1] ,255 * out.output_color[2]);
	    
	}//end for height
    }//end for width
    
}


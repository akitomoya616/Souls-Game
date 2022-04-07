//  Per Pixel Lighting shader with texture
#version 120

varying vec3 View;
varying vec3 Light;
varying vec3 Normal;
uniform sampler2D tex;

//define a int that will be used in this frag file only (hence it is uniform but not varying) for checking the lighting type
//this value can be tracked and re-defined in project.c 
//using int int id=glGetUniform(shader,"lighting_value") and glUniform1i(id,the value you want to assign)

uniform int lighting_type=0;

void main()
{
   //  N is the object normal
   vec3 N = normalize(Normal);
   //  L is the light vector
   vec3 L = normalize(Light);
   //  R is the reflected light vector R = 2(L.N)N - L
   vec3 R = reflect(-L,N);
   //  V is the view vector (eye vector)
   vec3 V = normalize(View);



   //  Diffuse light is cosine of light and normal vectors
   float Id = max(dot(L,N) , 0.0);
   //  Specular is cosine of reflected and view vectors
   float Is = (Id>0.0) ? pow(max(dot(R,V),0.0) , gl_FrontMaterial.shininess) : 0.0;

   //  Sum color types
   vec4 color;
   //if we are in a special room that requires special lighting, then specular+0.5
   if(lighting_type==1 && Id>0.0 ){
    color = gl_FrontMaterial.emission
              + gl_FrontLightProduct[0].ambient
              + Id*gl_FrontLightProduct[0].diffuse
              + Is*(gl_FrontLightProduct[0].specular+0.5);
   }
   //else, normal lighting setting
   else{
    color = gl_FrontMaterial.emission
              + gl_FrontLightProduct[0].ambient
              + Id*gl_FrontLightProduct[0].diffuse
              + Is*gl_FrontLightProduct[0].specular;
   }

   //  Apply texture
   gl_FragColor = color * texture2D(tex,gl_TexCoord[0].xy);
}

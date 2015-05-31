#version 420 core

	struct material
	{
		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
		float shininess;
	};
	
	struct lightSource {
	 vec3 ambient;              // Acli
	 vec3 diffuse;              // Dcli
	 vec3 specular;             // Scli
	 vec3 position;             // Ppli
	};                                                
	                                                                  
	in vec3 vPos;  
	in vec3 vNorm;  
	smooth out vec4 color;
	uniform mat4 PV; 
	uniform vec3 camPosition; 
	uniform lightSource light1;
	uniform lightSource light2;
	uniform int multipleLight;
	uniform material mat;

	vec4 calcColorForOneLight(lightSource li, material m, vec3 n, vec3 v){
		vec3 l = normalize(li.position-vPos); 
		vec3 r = reflect(-l, n);//((2 * n)*dot(n, l)) - l;
		r = normalize(r); 

		vec3 amb = n*(light1.ambient*mat.ambient); 
		vec3 diff = n*((light1.diffuse*mat.diffuse)) * max(dot(n, l), 0.0); 
		vec3 spe = (light1.specular*mat.specular)*pow(max(dot(r, v), 0.0), mat.shininess)*n;

		return vec4((spe+amb+diff), 1.0); 
	}

	void main(void) 
	{ 
		vec3 n = normalize(vNorm);
		vec3 v = normalize(camPosition- vPos); 
		
		color = color + calcColorForOneLight(light1, mat, n, v);
		//color = color + calcColorForOneLight(light2, mat, n, v);
		gl_Position = PV * vec4(vPos, 1.0f);
	}
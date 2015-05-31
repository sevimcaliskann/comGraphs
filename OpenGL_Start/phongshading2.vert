#version 420 core

	struct material
	{
		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
		float shininess;
	};
	
	struct lightSource {
	 vec3 ambient;
	 vec3 diffuse;
	 vec3 specular;
	 vec3 position;
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

		vec3 amb = n*(li.ambient*m.ambient); 
		float cosAngIncidence = max(dot(n, l), 0.0);
		cosAngIncidence = clamp(cosAngIncidence, 0, 1);
		vec3 diff = n*((li.diffuse*m.diffuse)) * cosAngIncidence; 
		float phongTerm = max(dot(r, v), 0.0);
		phongTerm = clamp(phongTerm, 0, 1);
		phongTerm = cosAngIncidence != 0.0 ? phongTerm : 0.0;
		vec3 spe = (li.specular*m.specular)*phongTerm*n;

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
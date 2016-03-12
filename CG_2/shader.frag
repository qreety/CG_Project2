#version 330 core
struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shine;
};

struct Light{
	vec4 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	bool on;
};

int numLight;
float attenuation;
Light lights[2];
//20
in vec3 vNormal;  //in camera space
flat in int m;
in vec3 vPos; // in world space
in vec4 aColor;

out vec4 color;

uniform vec3 camPos;
uniform vec3 aLight;
uniform Light sLight;
uniform Light dLight;
uniform Material mat[6];

void main(){
    if(sLight.on == true && dLight.on == true){
		lights[0] = sLight;
		lights[1] = dLight;
		numLight = 2;
	}//40
	else if(sLight.on == true && dLight.on == false){
		lights[0] = sLight;
		numLight = 1;
	}
	else if(sLight.on == false && dLight.on == true){
		lights[0] = dLight;
		numLight = 1;
	}
	else
		numLight = 0;

	vec3 totalLighting = vec3(aColor) * vec3(mat[m].ambient);
	vec3 lightDir;

	for(int i = 0; i < numLight; i++){
		attenuation = 1.0;
		if (lights[i].position.w == 0.0) // directional light
			{
			lightDir = normalize(vec3(lights[i].position));
			}
		else // point or spot light //60
			{
			lightDir = normalize(vec3(lights[i].position -vPos));
			}
		

		vec3 ambientC = lights[i].ambient * mat[m].ambient;

		float diff = max(dot(vNormal, lightDir), 0.0);
		vec3 diffuseReflection = lights[i].diffuse * (diff * mat[m].diffuse);

		vec3 specularReflection;
		if(dot(vNormal, lightDir)>0){
		vec3 viewDir = normalize(camPos - vPos);
		vec3 halfDir = normalize( lightDir + camPos );  
		float sp = pow(max(dot(vNormal,halfDir), 0.0), mat[m].shine);
		specularReflection = lights[i].specular * (sp * mat[m].specular); 
		}
		else
			specularReflection = vec3(0.0, 0.0, 0.0);
		
		totalLighting = totalLighting + ambientC + diffuseReflection + specularReflection;
		}

	color = vec4(totalLighting, 1.0);
}

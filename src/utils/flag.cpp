#include "flag.hpp"


Flag::Flag(){
	//generate Particles
	for(int i = 0; i < Flag_HEIGHT; i++){
		for(int j = 0; j < Flag_WIDTH; j++){
			Particle *p = new Particle();
			unsigned int ind = 0;
			p->mass = 0.1;
			p->index = ind++;

			if (i == Flag_HEIGHT - 1) {
				p->fixed = true;
			}
			else {
				p->fixed = false;
			}

			//arbitrary relative position of particle
			p->position = glm::vec3(0.1 * (j - Flag_WIDTH / 2.0), 0.1 * i + 0.5, 0);

			//Add particles and its info to datastructure  
			positions.push_back(p->position);
			normals.push_back(glm::vec3(0.f, 0.f, 0.f));
			particles.push_back(p);
		}
	}

	//generate spring dampers
	for(int i = 0; i < Flag_HEIGHT; i++){
		for(int j = 0; j < Flag_WIDTH; j++){

			//edge case: can't connect anything to the top if at the very top
			if(i < Flag_HEIGHT - 1){

				//first check upper left
				if(j > 0){
					Damper *d1 = new Damper();

					d1->p1 = particles[i * Flag_WIDTH + j];
					d1->p2 = particles[(i+1) * Flag_WIDTH + (j-1)];

					d1->length = glm::length(d1->p1->position - d1->p2->position);
					d1->k = SPRING_CONST;
					d1->d = DAMP_CONST;

					dampers.push_back(d1);
				}

				//next check upper right
				if(j < Flag_WIDTH - 1){
					Damper *d2 = new Damper();

					d2->p1 = particles[i * Flag_HEIGHT + j];
					d2->p2 = particles[(i+1) * Flag_HEIGHT + (j+1)];

					d2->length = glm::length(d2->p1->position - d2->p2->position);
					d2->k = SPRING_CONST;
					d2->d = DAMP_CONST;

					dampers.push_back(d2);
				}

				// lastly directly upper 
				Damper *d3 = new Damper();

				d3->p1 = particles[i * Flag_HEIGHT + j];
				d3->p2 = particles[(i + 1) * Flag_HEIGHT + j];

				d3->length = glm::length(d3->p1->position - d3->p2->position);
				d3->k = SPRING_CONST;
				d3->d = DAMP_CONST;

				dampers.push_back(d3);
			}

			//connect particle to right
			if(j < Flag_WIDTH - 1){
				Damper *d4 = new Damper();

				d4->p1 = particles[i * Flag_HEIGHT + j];
				d4->p2 = particles[i * Flag_HEIGHT + (j+1)];

				d4->length = glm::length(d4->p1->position - d4->p2->position);
				d4->k = SPRING_CONST;
				d4->d = DAMP_CONST;

				dampers.push_back(d4);
			}
		}
	}

	//generate indices for opengl
	for(int i = 0; i < Flag_HEIGHT - 1; i++){
		for(int j = 0; j < Flag_WIDTH - 1; j++){

			int i1 = i * Flag_HEIGHT + j;
			int i2 = i * Flag_HEIGHT + (j+1);
			int i3 = (i + 1) * Flag_HEIGHT + j;

			indices.push_back(i1);
			indices.push_back(i2);
			indices.push_back(i3);

			triangles.push_back(new Triangle(particles[i1], particles[i2], particles[i3]));

			i1 = i * Flag_HEIGHT + (j+1);
			i2 = (i + 1) * Flag_HEIGHT + (j+1);
			i3 = (i + 1) * Flag_HEIGHT + j;

			indices.push_back(i1);
			indices.push_back(i2);
			indices.push_back(i3);

			triangles.push_back(new Triangle(particles[i1], particles[i2], particles[i3]));
		}
	}

	genNormals();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Flag::~Flag(){
	particles.clear();
	normals.clear();
	triangles.clear();

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
}

void Flag::rebind(){
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Flag::update(){
	for(int i = 0; i < SAMPLES_PER_FRAME; i++){
		updateParticles();
		genDamp();
		genWind();
		genNormals();
	}
	rebind();
}

void Flag::updateParticles(){
	for(Particle* p: particles){
		p->update();
		positions[p->index] = p->position;
	}
}

void Flag::translate(glm::vec3 dist){
	for(Particle *p: particles){
		if(p->fixed){
			p->position += dist;

			if(p->position.y <= FLOORHEIGHT)
				p->position -= (dist);
		}
	}
}

void Flag::genDamp(){
	for(Damper* d: dampers){
		d->computeForce();
	}
}

void Flag::genWind(){
	for(Triangle* t: triangles){
		t->applyWind();
	}
}

void Flag::genNormals(){
	//init normals
	for(glm::vec3 v: normals){
		v = glm::vec3(0.f, 0.f, 0.f);
	}

	//get normal from cross product
	for(Triangle* t: triangles){
		Particle* a = t->p1;
		Particle* b = t->p2;
		Particle* c = t->p3;
		
		glm::vec3 norm = glm::normalize(glm::cross((b->position - a->position), (c->position - a->position)));
		normals[a->index] += norm;
		normals[b->index] += norm;
		normals[c->index] += norm;
	}
}

void Flag::draw(){
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
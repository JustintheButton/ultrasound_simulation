#include <vector>
#include <iostream>
#include <cmath>
#include <ctime>
#include <utility>
using namespace std;

#define pdd pair<double, double>

const int numParticles = 100;
const int n_timesteps = 100;
const double timestep = .1;
const double MAX_WIDITH = 3; //not sure of measurements
const double MAX_LENGTH = 3;
const double DRAG_CONST =  0.002;

struct particle{
	pdd pos, vel;
    double dir;
    int ind;
};

//vector<vector<int>> pressure;
vector<particle> particles;

pdd calculate_pressure() {
    pdd ret; int m = rand() % 2; if(m == 0){m--;}
    ret.first = 2 * m * double(rand() % 100) / 100;
    ret.second = 2 * double(rand() % 100) / 100;
    return ret;
}

particle init_particle(int i){
    particle ret;
    ret.dir = 0;
    ret.vel.first = 0;
    ret.vel.first = cos(ret.dir) * (0.6 * (rand() % 100) / 100 + 0.2);
    ret.pos.first = (rand() % 100) / 100;
    ret.pos.second = 0;
    ret.ind = i;
    return ret;
}

void set_particles(){
    particles.resize(1);
    for(int i = 0; i < numParticles; i++){
        particles[i] = init_particle(i);
    }
}

void ts_detail(particle p){
    cout << "Particle id: " << p.ind << "\n";
    cout << "x: " << p.pos.first << ", y: " << p.pos.second << ", x_v: " << p.vel.first << ", y_v: " << p.vel.second << "\n";
}

void ts_desmos(particle p){
    cout << "(" << p.pos.first << ", " << p.pos.second << ")\n";
}

void move_particles(){
    for(auto &i : particles){
        i.pos.first = i.pos.first + i.vel.first * timestep;
        i.pos.second = i.pos.second + i.vel.second * timestep;
        if(i.pos.first < 0){i.pos.first = 0;}
        if(i.pos.second < 0){i.pos.second = 0;}
        if(i.pos.first >= MAX_WIDITH){i.pos.first = MAX_WIDITH;}
        if(i.pos.second >= MAX_LENGTH){i.pos.second = MAX_LENGTH;}
        pdd pressure = calculate_pressure();
        i.vel.first = i.vel.first + pressure.first * timestep;
        i.vel.second = i.vel.second + pressure.second * timestep;
        i.vel.first *= (1- DRAG_CONST); i.vel.second *= (1-DRAG_CONST); //water drag
        ts_detail(i);
        //ts_desmos(i);
    }
}

int main() {
	srand(time(NULL));

    set_particles();
    for(int i = 0; i < n_timesteps; i++){move_particles();}

    /*
    int t = 100;
    while(t--){
        pdd a = calculate_pressure();
        cout << a.first << " " << a.second << "\n";
    }
     */
}
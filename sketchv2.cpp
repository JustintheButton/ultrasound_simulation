#include <vector>
#include <iostream>
#include <cmath>
#include <ctime>
#include <utility>
#include <fstream>
#include <string>
using namespace std;

#define pdd pair<double, double>

const int numParticles = 100;
const int n_timesteps = 100;
int num_digits = log(n_timesteps + 1) - 1; //only used in file export dw about it
const double timestep = .2;
const double MAX_WIDTH = 3; //not sure of measurements
const double MAX_LENGTH = 4;
const double DRAG_CONST =  0.04;

string test_name = "second_try";

struct particle{
	pdd pos, vel;
    double dir;
    int ind;
};

//vector<vector<int>> pressure;
vector<particle> particles;

pdd calculate_pressure() {
    pdd ret; int m = rand() % 2; if(m == 0){m--;}
    ret.first = 4 * m * double(rand() % 100) / 100;
    ret.second = 2 * double(rand() % 100) / 100;
    return ret;
}

particle init_particle(int i){
    particle ret;
    ret.dir = 0;
    ret.vel.first = 0;
    ret.vel.first = cos(ret.dir) * (0.6 * (rand() % 100) / 100 + 0.2);
    ret.pos.first = MAX_WIDTH * (rand() % 100) / 100;
    ret.pos.second = 0;
    ret.ind = i;
    return ret;
}

void set_particles(){
    particles.resize(numParticles);
    for(int i = 0; i < numParticles; i++){
        particles[i] = init_particle(i);
    }
}

void ts_detail(particle p, pdd pr){
    cout << "Particle id: " << p.ind << "\n";
    cout << "x: " << p.pos.first << ", y: " << p.pos.second << ", x_v: " << p.vel.first << ", y_v: " << p.vel.second << ", ";
    cout << "x_p: " << pr.first << ", y_p: " << pr.second << "\n";
}

void ts_desmos(particle p){
    cout << "(" << p.pos.first << ", " << p.pos.second << ")\n";
}

void write_to_csv(particle p, int cur_timestep){
    //string file_name = "./" + test_name + "/" + test_name + ".csv.";
    string numerical_part = to_string(cur_timestep);
    while(numerical_part.length() < num_digits){numerical_part = "0" + numerical_part;}
    //file_name += numerical_part;
    string file_name = "./" + test_name + "/" + test_name + "-" + numerical_part + ".csv";
    ofstream fout(file_name, ios::app); //ios::app appends it instead of creating new file, very important
    fout << p.pos.first << "," << p.pos.second << "\n";
}

void move_particles(){
    //bool stuck = false; //simulates if it gets stuck to the outside of the container
    vector<bool> stuck(numParticles, false);
    for(int j = 0; j < n_timesteps; j++) {
        int ct = 0;
        //ofstream fout("initial_test.csv." + j);
        for(auto &i : particles) {
            if (stuck[ct]) {
                ts_desmos(i);
                ct++;
                continue;
            }
            int sign = 1;
            i.pos.first = i.pos.first + i.vel.first * timestep;
            i.pos.second = i.pos.second + i.vel.second * timestep;
            if (i.pos.first < 0) {
                i.pos.first = 0;
                stuck[ct] = true;
            }
            if (i.pos.second < 0) {
                i.pos.second = 0;
                sign = 1;
            }
            if (i.pos.first >= MAX_WIDTH) {
                i.pos.first = MAX_WIDTH;
                stuck[ct] = true;
            }
            if (i.pos.second >= MAX_LENGTH) {
                i.pos.second = MAX_LENGTH;
                sign = -1;
            }
            pdd pressure = calculate_pressure();
            i.vel.first = (i.vel.first + pressure.first * timestep);
            i.vel.second = (i.vel.second + pressure.second * timestep);
            i.vel.first *= (1 - DRAG_CONST);
            i.vel.second *= (1 - DRAG_CONST) * sign; //water drag
            //testing
            i.vel.first = max(i.vel.first, -0.75);
            i.vel.first = min(i.vel.first, 0.75); //bounding velocity??
            i.vel.second = max(i.vel.second, -1.0);
            i.vel.second = min(i.vel.second, 1.0); //don't think y should be bounded
            //ts_detail(i, pressure);
            ts_desmos(i);
            write_to_csv(i, j);
            ct++;
        }
    }
    cout << "-----------------------------\n";
}

int main() {
	srand(1e9 + 9);

    set_particles();
    move_particles();

    /*
    int t = 100;
    while(t--){
        pdd a = calculate_pressure();
        cout << a.first << " " << a.second << "\n";
    }
     */
}
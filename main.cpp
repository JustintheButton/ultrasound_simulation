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
const int n_timesteps = 1000;
int num_digits = log10(n_timesteps) + 1; //only used in file export dw about it
//int num_digits = 3;
const double timestep = .2;
const double MAX_WIDTH = 3; //not sure of measurements
const double MAX_LENGTH = 4;
const double DRAG_CONST =  0.5;

string test_name = "stagger_test";
string ensight_filename = "particles";

struct particle{
    pdd pos, vel;
    double dir;
    int ind;
    int strk = rand() % 20 + 85;
    int cur = 0;
    bool forwards = true;
    int stagger;
};

//vector<vector<int>> pressure;
vector<particle> particles;

string notation(double d){
    string ret = ""; if(d < 0){ret += "-"; d *= -1;}
    int ct = 0;
    if(d == 0){return "0.00000e+00";}
    while(d >= 10){
        ct++; d /= 10;
    }
    while(d < 1){d *= 10; ct--;}
    bool neg = false; if(ct < 0){neg = true;} ct = abs(ct);
    string temp = to_string(d); temp = temp.substr(0, 7);
    while(temp.length() < 7){temp += "0";}
    ret += temp;
    ret += 'e'; string exp = to_string(ct);
    while(exp.length() < 2){exp = "0" + exp;}
    if(neg){ret+= "-";} else{ret += "+";}
    ret += exp;
    return ret;
}

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
    ret.stagger = i;
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

void setup_ensight_file(int timestep){
    string numerical_part = to_string(timestep);
    while(numerical_part.length() < num_digits){numerical_part = "0" + numerical_part;}
    string filename = "./" + test_name + "/" + ensight_filename + "_" + numerical_part + ".xyz";
    ofstream fout(filename, ios::app);
    fout << "EnSight Model Geometry File\nEnSight 7.4.1\nnode id assign\nelement id assign\nextents\n";
    fout << notation(-1) << " " << notation(MAX_WIDTH + 1) << "\n";
    fout << notation(-1) << " " << notation(MAX_LENGTH + 1) << "\n";
    fout << notation(-1) << " " << notation(1) << "\n";
    fout << "part\n"; int part = 1; string temp = to_string(part); while(temp.length() < 10){temp = " " + temp;}
    fout << temp << "\n";
    fout << "Point field\ncoordinates\n";
    temp = to_string(numParticles); while(temp.length() < 10){temp = " " + temp;}
    fout << temp << "\n";
}

void close_ensight_file(int timestep){
    string numerical_part = to_string(timestep);
    while(numerical_part.length() < num_digits){numerical_part = "0" + numerical_part;}
    string filename = "./" + test_name + "/" + ensight_filename + "_" + numerical_part + ".xyz";
    ofstream fout(filename, ios::app);
    fout << "point" << "\n";
    string temp = to_string(numParticles); while(temp.length() < 10){temp = " " + temp;}
    fout << temp << "\n";
    for(int i = 1; i <= numParticles; i++){
        temp = to_string(i); while(temp.length() < 10){temp = " " + temp;}
        fout << temp << "\n";
    }
}

void write_ensight_file(const vector<pdd> &v, int timestep){
    string numerical_part = to_string(timestep);
    while(numerical_part.length() < num_digits){numerical_part = "0" + numerical_part;}
    string filename = "./" + test_name + "/" + ensight_filename + "_" + numerical_part + ".xyz";
    ofstream fout(filename, ios::app);
    for(auto i : v){
        fout << notation(i.first) << " \n";
    }
    for(auto i : v){
        fout << notation(i.second) << " \n";
    }
    for(int i = 0; i < v.size(); i++){
        fout << notation(0) << " \n";
    }
}

void write_case_file(){
    ofstream fout("./" + test_name + "/" + test_name + ".case");
    fout << "FORMAT\ntype: ensight gold\nGEOMETRY\nmodel: ";
    fout << ensight_filename << "_"; for(int i = 0; i < num_digits; i++){fout << "*";} fout << ".xyz\n";
    fout << "TIME\ntime set: 1\n";
    fout << "number of steps: " << n_timesteps << "\n";
    fout << "filename start number: 0\nfilename increment: 1\n";
    fout << "time values:\n";
    for(int i = 0; i < n_timesteps; i++){
        fout << i << ".0\n";
    }
}

particle calculate_side_bounce(particle p){
    particle after_bounce = p;
    after_bounce.vel.first *= -1;
    return after_bounce;
}


void move_particles(){
    //bool stuck = false; //simulates if it gets stuck to the outside of the container
    vector<bool> stuck(numParticles, false);
    for(int j = 0; j < n_timesteps; j++) {
        int ct = 0;
        vector<pdd> vec(numParticles);
        //ofstream fout("initial_test.csv." + j);
        for(auto &i : particles) {
            /*
            if (stuck[ct]) {
                //ts_desmos(i);
                vec[ct].first = i.pos.first;
                vec[ct].second = i.pos.second;
                ct++;
                continue;
            }
            */
            if(i.stagger > 0){i.stagger--; continue;}
            int sign = 1;
            i.pos.first = i.pos.first + i.vel.first * timestep;
            i.pos.second = i.pos.second + i.vel.second * timestep;
            if (i.pos.first < 0) {
                /*
                i.pos.first = 0;
                stuck[ct] = true;
                */
                i = calculate_side_bounce(i);
                i.pos.first = max(0.0, i.pos.first);
            }
            if (i.pos.second < 0) {
                i.pos.second = 0;
                sign = 1;
            }
            if (i.pos.first >= MAX_WIDTH) {
                /*
                i.pos.first = MAX_WIDTH;
                stuck[ct] = true;
                */
                i = calculate_side_bounce(i);
                i.pos.first = min(i.pos.first, MAX_WIDTH);
            }
            if (i.pos.second >= MAX_LENGTH) {
                i.pos.second = MAX_LENGTH;
                i.forwards = false;
                i.cur = 0;
                i.strk = ceil(double(i.strk) * (double(rand() % 10 + 60)/100));
                sign = -1;
            }
            pdd pressure = calculate_pressure();
            i.vel.first = (i.vel.first + pressure.first * timestep);
            i.vel.second = (i.vel.second + pressure.second * timestep);
            i.vel.first *= (1 - DRAG_CONST);
            i.vel.second *= (1 - DRAG_CONST) * sign; //water drag

            //random stuff
            if(i.forwards){i.vel.second = abs(i.vel.second);}
            else{i.vel.second = -1 * abs(i.vel.second);}
            i.cur++;
            if(i.cur == i.strk){
                i.cur = 0; i.forwards = 1-i.forwards;
                i.strk = ceil(double(i.strk) * (double(rand() % 10 + 60)/100));
            }

            //testing
            i.vel.first = max(i.vel.first, -0.75);
            i.vel.first = min(i.vel.first, 0.75); //bounding velocity??
            i.vel.second = max(i.vel.second, -1.0);
            i.vel.second = min(i.vel.second, 1.0); //don't think y should be bounded
            //ts_detail(i, pressure);
            //ts_desmos(i);
            //write_to_csv(i, j);
            vec[ct].first = i.pos.first;
            vec[ct].second = i.pos.second;
            ct++;
        }
        setup_ensight_file(j);
        write_ensight_file(vec, j);
        close_ensight_file(j);
    }
    cout << "-----------------------------\n";
}

int main() {
    srand(1e9 + 9);
    cout << num_digits;
    //ofstream testfile("./tryfour/test.txt");
    //testfile << "Hi!";
    write_case_file();
    set_particles();
    move_particles();
    //cout << notation(.00000000000000000000254235);
}
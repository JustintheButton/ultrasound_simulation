#include <vector>
#include <iostream>
#include <cmath>
#include <ctime>
#include <utility>
#include <fstream>
#include <string>
using namespace std;

#define pdd pair<double, double>

const int numParticles = 1000;
const int n_timesteps = 1000;
int num_digits = log10(n_timesteps) + 1; //only used in file export dw about it
//int num_digits = 3;
const double timestep = .2;
const double MAX_WIDTH = 3; //not sure of measurements
const double MAX_LENGTH = 10;
const double DRAG_CONST =  0.5; //implemented incorrectly
const double cushion = 0.2;

const double INITIAL_WIDTH = 1; //radius 0.5

string test_name = "kmsv2";
string ensight_filename = "particles";

struct point{
    double x, y, z;
};

struct particle{
    point pos, vel;
    double dir;
    int ind;
    int strk = rand() % 20 + 150;
    int cur = 0;
    bool forwards = true;
    int stagger;
    double curwidth = INITIAL_WIDTH;
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

bool incircle(particle p, double radius){
    //center is always (MAX_WIDTH/2, MAX_WIDTH/2)
    return (pow(p.pos.x - MAX_WIDTH / 2, 2) + pow(p.pos.z - MAX_WIDTH / 2, 2) < pow(radius, 2));
}

point calculate_pressure(particle p) {
    point ret;
    if(incircle(p, p.curwidth / 2 - cushion)) {
        int m = rand() % 2;
        if (m == 0) { m--; }
        ret.x = 4 * m * double(rand() % 100) / 100;
        m = rand() % 2;
        if (m == 0) { m--; }
        ret.z = 4 * m * double(rand() % 100) / 100;
        ret.y = 2 * double(rand() % 100) / 100;
    }
    else{
        ret.y = 2 * double(rand() % 100) / 100;
        pdd tmp; tmp.first = MAX_WIDTH / 2 - p.pos.x; tmp.second = MAX_WIDTH / 2 - p.pos.z;
        double mag = sqrt(pow(tmp.first, 2) + pow(tmp.second, 2));
        tmp.first /= mag; tmp.second /= mag;
        ret.x = tmp.first; ret.z = tmp.second;
    }
    return ret;
}

particle init_particle(int i){
    particle ret;
    while(true) {
        ret.dir = 0;
        ret.vel.x = 0;
        ret.vel.y = cos(ret.dir) * (0.6 * (rand() % 100) / 100 + 0.2);
        ret.vel.z = 0;
        ret.pos.x = ret.curwidth * (rand() % 100) / 100 + MAX_WIDTH / 2;
        ret.pos.z = ret.curwidth * (rand() % 100) / 100 + MAX_WIDTH / 2;
        ret.pos.y = -3;
        ret.ind = i;
        ret.stagger = i;
        if(incircle(ret, ret.curwidth / 2)){return ret;}
    }
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
    cout << "x: " << p.pos.x << ", y: " << p.pos.y << ", x_v: " << p.vel.x << ", y_v: " << p.vel.y << ", ";
    cout << "x_p: " << pr.first << ", y_p: " << pr.second << "\n";
}

void ts_desmos(particle p){
    cout << "(" << p.pos.x << ", " << p.pos.y << ")\n";
}

void write_to_csv(particle p, int cur_timestep){
    //string file_name = "./" + test_name + "/" + test_name + ".csv.";
    string numerical_part = to_string(cur_timestep);
    while(numerical_part.length() < num_digits){numerical_part = "0" + numerical_part;}
    //file_name += numerical_part;
    string file_name = "./" + test_name + "/" + test_name + "-" + numerical_part + ".csv";
    ofstream fout(file_name, ios::app); //ios::app appends it instead of creating new file, very important
    fout << p.pos.x << "," << p.pos.y << "\n";
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

void write_ensight_file(const vector<point> &v, int timestep){
    string numerical_part = to_string(timestep);
    while(numerical_part.length() < num_digits){numerical_part = "0" + numerical_part;}
    string filename = "./" + test_name + "/" + ensight_filename + "_" + numerical_part + ".xyz";
    ofstream fout(filename, ios::app);
    for(auto i : v){
        fout << notation(i.x) << " \n";
    }
    for(auto i : v){
        fout << notation(i.y) << " \n";
    }
    for(auto i : v){
        fout << notation(i.z) << " \n";
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

/*
bool in_circle(particle p){
    return (pow(p.pos.x - MAX_WIDTH / 2, 2) + pow(p.pos.z - MAX_WIDTH/2, 2) < pow(MAX_WIDTH/2, 2));
}
 */

/*
particle bounce_3d(particle p){ //assume that p is still in bounds
    //find intersection point
    pdd intersection = {0.0, 0.0};

}
 */

particle calculate_side_bounce(particle p){
    particle after_bounce = p;
    after_bounce.vel.x *= -1;
    return after_bounce;
}


void move_particles(){
    //bool stuck = false; //simulates if it gets stuck to the outside of the container
    vector<bool> stuck(numParticles, false);
    for(int j = 0; j < n_timesteps; j++) {
        int ct = 0;
        vector<point> vec(numParticles);
        //ofstream fout("initial_test.csv." + j);
        for(auto &i : particles) {
            /*
            if (stuck[ct]) {
                //ts_desmos(i);
                vec[ct].first = i.pos.x;
                vec[ct].second = i.pos.y;
                ct++;
                continue;
            }
            */
            if(i.stagger > 0){i.stagger--; continue;}
            int sign = 1;
            i.pos.x = i.pos.x + i.vel.x * timestep;
            i.pos.y = i.pos.y + i.vel.y * timestep;
            i.pos.z = i.pos.z + i.vel.z * timestep;
            /*
            if (i.pos.x < 0) {

                i.pos.x = 0;
                stuck[ct] = true;

                i = calculate_side_bounce(i);
                i.pos.x = max(0.0, i.pos.x);
            }
            */
            if (i.pos.y < -3) {
                i.pos.y = 0;
                sign = 1;
            }
            /*
            if (i.pos.x >= MAX_WIDTH) {

                i.pos.x = MAX_WIDTH;
                stuck[ct] = true;

                i = calculate_side_bounce(i);
                i.pos.x = min(i.pos.x, MAX_WIDTH);
            }
            */
            if (i.pos.y >= MAX_LENGTH) {
                i.pos.y = MAX_LENGTH;
                i.forwards = false;
                i.cur = 0;
                i.strk = ceil(double(i.strk) * (double(rand() % 10 + 60)/100));
                sign = -1;
            }
            if(i.pos.y > 0){
                i.curwidth = MAX_WIDTH;
            }
            point pressure = calculate_pressure(i);
            i.vel.x = (i.vel.x + pressure.x * timestep);
            i.vel.y = (i.vel.y + pressure.y * timestep);
            i.vel.z = (i.vel.z + pressure.z * timestep);
            i.vel.x *= (1 - DRAG_CONST);
            i.vel.y *= (1 - DRAG_CONST) * sign; //water drag
            i.vel.z *= (1 - DRAG_CONST);
            //random stuff
            if(i.forwards){i.vel.y = abs(i.vel.y);}
            else{i.vel.y = -1 * abs(i.vel.y);}
            i.cur++;
            if(i.cur == i.strk){
                i.cur = 0; i.forwards = 1-i.forwards;
                i.strk = ceil(double(i.strk) * (double(rand() % 10 + 60)/100));
            }

            //testing
            i.vel.x = max(i.vel.x, -0.75);
            i.vel.x = min(i.vel.x, 0.75); //bounding velocity??
            i.vel.z = max(i.vel.z, -0.75);
            i.vel.z = min(i.vel.z, 0.75);
            i.vel.y = max(i.vel.y, -1.0);
            i.vel.y = min(i.vel.y, 1.0); //don't think y should be bounded
            //ts_detail(i, pressure);
            //ts_desmos(i);
            //write_to_csv(i, j);
            vec[ct].x = i.pos.x;
            vec[ct].y = i.pos.y;
            vec[ct].z = i.pos.z;
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
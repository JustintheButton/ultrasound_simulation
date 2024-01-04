#include <iostream>
#include <cmath>

using namespace std;

const double PI = 3.1415926535;

//can be adjusted as needed
const double DIAMETER = 2e-4; //diameter of microplastic particle
const double V = (4 / 3) * (PI) * pow((DIAMETER / 2), 3); //volume of microplastic particle
const double P_0 = 1e5; //pressure amplitude (currently 1 atm?)
const double LAMBDA = 1e-2; //wavelength 
const double BETA_C = 5e-5; //compressibility of microplastic particle
const double BETA_W = 4.5e-10; //compressibility of fluid
const double RHO_C = 950; //density of microplastic particle (kg / m^3)
const double RHO_W = 1000; //density of fluid (kg / m^3)
const double PHI = (5 * RHO_C - 2 * RHO_W) / (2 * RHO_C + RHO_W) - (BETA_C / BETA_W);
const double TIMESTEP = 0.01; //in seconds?
const int N_TIMESTEPS = 100;
const double MASS = V * RHO_C;

struct timestep{
    double x, y, v_x, v_y, a_x, a_y;
};

vector<timestep> test;

double F_r(double x){
    return -1 * (((PI * P_0 * P_0 * V * BETA_W) / (2 * LAMBDA)) * PHI * sin(4 * PI * x / LAMBDA));
}                                               

void fill(double init_x, double init_y, double init_v_x, double init_v_y){
    test[0].x = init_x; test[0].y = init_y; test[0].v_x = init_v_x; test[0].v_y = init_v_y;
    test[0].a_x = F_r(test[0].x) / (MASS);
    for(int i = 1; i < N_TIMESTEPS; i++){
        cout << i << "\n";
        test[i].v_x += TIMESTEP * test[i - 1].a_x;
        test[i].x += TIMESTEP * test[i].v_x;
        test[i].a_x = F_r(test[i].x) / (MASS);
    }
}

void print_results(){
    cout << N_TIMESTEPS << " timesteps with interval " << TIMESTEP << " seconds per step\n";
    for(auto i : test){
        cout << "position: (" << i.x << ", " << i.y << "), velocity: (" << i.v_x << ", " << i.v_y << "), acceleration :(" << i.a_x << ", " << i.a_y << ")\n";
    }
}

int main(){
    test.resize(N_TIMESTEPS);
    fill(2, 3, 0, 0);
    print_results();
}
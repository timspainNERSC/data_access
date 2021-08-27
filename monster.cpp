/*
 * All the relevant arguments are passed to every function. Some may be const &.
 */ 
#include <iostream>
#include <vector>
#include <string>

class ElementData {
public:
  ElementData() : w(0.), x(0.), y(0.), z(0.) { };
  ElementData(double w, double x, double y, double z) : w(w), x(x), y(y), z(z) { };

  double w;
  double x;
  double y;
  double z;

};

void level2Function(const double& x, const double& y, double& dx_dt) {
  const double sigma = 10.;
  
  dx_dt = sigma * (y - x);
}

void level1Function(double& x, double& y, double& z) {
  const double beta = 8./3.;
  const double rho = 28.;

  const double dt = 0.1;

  double dx_dt;
  level2Function(x, y, dx_dt);
  double dy_dt = x * (rho - z) - y;
  double dz_dt = x * y - beta * z;

  x += dx_dt * dt;
  y += dy_dt * dt;
  z += dz_dt * dt;
}  
  

void scalar( ) {
  ElementData e(2., 1., 1., 1.);
  std::cout << "x = " << e.x << ", y = " << e.y << ", z = " << e.z << std::endl;

  level1Function(e.x, e.y, e.z);

  std::cout << "Scalar run" << std::endl;
  std::cout << "x = " << e.x << ", y = " << e.y << ", z = " << e.z << std::endl;
}

void iterated_array(int nxyz, int timesteps) {
  double lorenz_range = 10.;
  double lorenz_start = -lorenz_range/2;
  double lorenz_step = lorenz_range/nxyz;
  
  std::vector<ElementData> modelData = std::vector<ElementData>(nxyz*nxyz*nxyz);

  // Fill the arrays
  for (int i = 0; i < nxyz; i++) {
    for (int j = 0; j < nxyz; j++) {
      for (int k = 0; k < nxyz; k++) {
	modelData[k + nxyz * (j + nxyz * i)] =
	  ElementData(
	   i * 10000. + j * 100. + k,
	   -lorenz_start + lorenz_step * i,
	   -lorenz_start + lorenz_step * j,
	   -lorenz_start + lorenz_step * k
	   );
      }
    }
  }

  // Loop over time and all elements
  for (int t = 0; t < timesteps; t++) {
    for (auto iter = modelData.begin(); iter != modelData.end(); iter++) {
      level1Function(iter->x, iter->y, iter->z);
    }
  }

  // Sum all the elements, so that the compiler cannot omptimize away the main loop as not doing anything
  double sumx = 0.;
  double sumy = 0.;
  double sumz = 0.;
  for (auto iter = modelData.begin(); iter != modelData.end(); iter++) {
    sumx += iter->x;
    sumy += iter->y;
    sumz += iter->z;
  }

  std::cout << "Variable totals: x = " << sumx << ", y = " << sumy << ", z = " << sumz << std::endl;

}

int main(int argc, char* argv[]) {

  scalar();

  int nxyz;
  int ntimesteps;
  if (argc > 1) {
    nxyz = std::stoi(std::string(argv[1]));
  } else {
    nxyz = 100;
  }

  if (argc > 2) {
    ntimesteps = std::stoi(std::string(argv[2]));
  } else {
    ntimesteps = 100;
  }

  iterated_array(nxyz, ntimesteps);
  
  return 0;
}

/*
 * Performing calculations by directly passing the ElementData object
 * to the calculating functions.
 */

#include <iostream>
#include <vector>
#include <string>

class ElementData;

void level1Function(ElementData&);
void level2Function(ElementData&);

class ElementData {
public:
  ElementData() : w(0.), x(0.), y(0.), z(0.) { };
  ElementData(double w, double x, double y, double z) : w(w), x(x), y(y), z(z) { };
  double w;
  double x;
  double y;
  double z;

  double dx_dt;
};

void level1Function(ElementData& data) {
  const double beta = 8./3.;
  const double rho = 28.;

  const double dt = 0.1;
  
  double dy_dt;
  double dz_dt;

  level2Function(data);
  dy_dt = data.x * (rho - data.z) - data.y;
  dz_dt = data.x * data.y - beta * data.z;

  data.x += data.dx_dt * dt;
  data.y += dy_dt * dt;
  data.z += dz_dt * dt;
}  
  
void level2Function(ElementData& data) {
  const double sigma = 10.;
  
  data.dx_dt = sigma * (data.y - data.x);
}

void scalar( ) {
  ElementData e(2., 1., 1., 1.);
  std::cout << "x = " << e.x << ", y = " << e.y << ", z = " << e.z << std::endl;

  level1Function(e);

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
	level1Function(*iter);
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

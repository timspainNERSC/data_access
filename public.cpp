/*
 * All the data members of ElementData are public, allowing the
 * argument class direct access.
 */
#include <iostream>
#include <vector>
#include <string>

// Forward definitions
class Level1IO;
class Level2IO;
void level1Function(Level1IO&);
void level2Function(Level2IO&);

class ElementData {
public:
  ElementData() : w(0.), x(0.), y(0.), z(0.) { };
  ElementData(double w, double x, double y, double z) : w(w), x(x), y(y), z(z) { };
  double w;
  double x;
  double y;
  double z;
};

class Level1IO {
private:
  ElementData& data;
public:
  Level1IO(ElementData& data) : data(data), dx_dt(0.) { };

  double dx_dt;

  double& x() {return data.x;};
  double& y() {return data.y;};
  double& z() {return data.z;};
};

class Level2IO {
private:
  Level1IO& data;
public:
  Level2IO(Level1IO& data) : data(data) { };
  const double& x() {return data.x();};
  const double& y() {return data.y();};
  double& dx_dt() {return data.dx_dt;};
};

void level1Function(Level1IO& data) {
  const double beta = 8./3.;
  const double rho = 28.;

  const double dt = 0.1;
  
  double dy_dt;
  double dz_dt;

  Level2IO l2data(data);
  level2Function(l2data);

  dy_dt = data.x() * (rho - data.z()) - data.y();
  dz_dt = data.x() * data.y() - beta * data.z();

  data.x() += data.dx_dt * dt;
  data.y() += dy_dt * dt;
  data.z() += dz_dt;
}  
  
void level2Function(Level2IO& data) {
  const double sigma = 10.;
  
  data.dx_dt() = sigma * (data.y() - data.x());
}

void scalar( ) {
  ElementData e(2., 1., 1., 1.);
  std::cout << "x = " << e.x << ", y = " << e.y << ", z = " << e.z << std::endl;

  Level1IO l1(e);
  level1Function(l1);

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
        Level1IO l1(*iter);
	level1Function(l1);
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


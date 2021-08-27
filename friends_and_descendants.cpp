/*
 * The auxiliary classes are functors. The constructors are friends of
 * the data class one level higher. The const-ness of the internal
 * variables determines writability.
 *
 * Other implementationsof a given level function are possible.
 */
#include <iostream>
#include <vector>
#include <string>

// Forward definitions
class ElementData;
class Level1;
class Level2;

class Level2 {
public:
  Level2(Level1& data);
protected:
  const double& x;
  const double& y;
  double& dx_dt;
public:
  virtual void operator()();
};

class Level1 {
public:
  Level1(ElementData& data);
  
protected:
  // Level1::operator() will write to all of these 
  double& x;
  double& y;
  double& z;
  // Declare w qualified as const: read only
  const double& w;

  double dx_dt;

  friend Level2::Level2(Level1&);
public:
  virtual void operator()();
};

class ElementData {
public:
  ElementData() : w(0.), x(0.), y(0.), z(0.) { };
  ElementData(double w, double x, double y, double z) : w(w), x(x), y(y), z(z) { };
  const double& getW() {return w;};
  const double& getX() {return x;};
  const double& getY() {return y;};
  const double& getZ() {return z;};
private:
  double w;
  double x;
  double y;
  double z;

  friend Level1::Level1(ElementData&); // An explicit list of what is
				       // allowed write access to
				       // ElementData
};

Level2::Level2(Level1& data) : x(data.x), y(data.y), dx_dt(data.dx_dt) { };

void Level2::operator()() {
  const double sigma = 10.;
  
  dx_dt = sigma * (y - x);
}

Level1::Level1(ElementData& data) : w(data.w), x(data.x), y(data.y), z(data.z), dx_dt(0.) { }

void Level1::operator()() {
  const double beta = 8./3.;
  const double rho = 28.;

  const double dt = 0.1;
  
  double dy_dt;
  double dz_dt;

  Level2 level2(*this);
  level2();

  dy_dt = x * (rho - z) - y;
  dz_dt = x * y - beta * z;

  x += dx_dt * dt;
  y += dy_dt * dt;
  z += dz_dt * dt;

  // w = z; // This would be an error, as w is qualified const.
}  
  
void scalar( ) {
  ElementData e(2., 1., 1., 1.);
  std::cout << "x = " << e.getX() << ", y = " << e.getY() << ", z = " << e.getZ() << std::endl;

  Level1 level1(e);
  level1();

  std::cout << "Scalar run" << std::endl;
  std::cout << "x = " << e.getX() << ", y = " << e.getY() << ", z = " << e.getZ() << std::endl;
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
        Level1 level1(*iter);
	level1();
    }
  }

  // Sum all the elements, so that the compiler cannot omptimize away the main loop as not doing anything
  double sumx = 0.;
  double sumy = 0.;
  double sumz = 0.;
  for (auto iter = modelData.begin(); iter != modelData.end(); iter++) {
    sumx += iter->getX();
    sumy += iter->getY();
    sumz += iter->getZ();
  }

  std::cout << "Variable totals: x = " << sumx << ", y = " << sumy << ", z = " << sumz << std::endl;

}

/*******************************************************************************
 * A Completely Different implementation
 ******************************************************************************/

class Level1a : public Level1 {
public:
  Level1a(ElementData& data) : Level1(data) { };
  void operator()();
};

void Level1a::operator()() {
    const double beta = 8./3.;
  const double rho = 28.;

  const double dt = 0.1;
  
  double dy_dt;
  double dz_dt;

  Level2 level2(*this);
  level2();

  dy_dt = x * (rho - z) - y;
  dz_dt = x * y - beta * z;

  x += dx_dt * dt;
  y += dy_dt * dt;
  z += dz_dt;

}

void scalaa( ) {
  ElementData e(2., 1., 1., 1.);
  std::cout << "x = " << e.getX() << ", y = " << e.getY() << ", z = " << e.getZ() << std::endl;

  Level1a level1a(e);
  level1a();

  std::cout << "Scalar run A" << std::endl;
  std::cout << "x = " << e.getX() << ", y = " << e.getY() << ", z = " << e.getZ() << std::endl;
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

  scalaa();
  
  return 0;
}


#include <iostream>
#include <random>
#include <math.h>
using namespace std;

class Turret {
  public:
    float reloadTime;
    float baseAimVariance;
    float baseDamageMean;

    //base damage is currently calculated using a "sum of uniform distributions" approach
    //looks a lot like a truncated normal distribution
    //max is 2*baseDamageMean, min is 0
    float getBaseDamage() {
      const int numUniformDistributions = 4; //increase to better approximate a normal distribution

      random_device                     rd;
      default_random_engine             generator(rd());
      uniform_real_distribution<double> distribution(0.0,1.0);

      float baseDealtDamage = 0.0;
      for(int i=0;i<numUniformDistributions;i++) {
        baseDealtDamage += baseDamageMean * 2.0 / numUniformDistributions * distribution(generator);
      }

      return baseDealtDamage;
    }

    //gives the ratio of the distance of "collision" from the target and the distance between the two bodies
    //normally distributed -- infinite distances are possible
    float getFiringSkewRatio() {
      random_device              rd;
      default_random_engine      randGenerator(rd());
      normal_distribution<float> distribution(0,baseAimVariance);

      return distribution(randGenerator);
    }

    float shotDistanceFromTargetCenter(float distance) {
      return abs(distance * getFiringSkewRatio());
    }

    bool shotLanded(float targetRadius, float shotDistanceFromTarget) {
      return shotDistanceFromTarget<targetRadius;
    }

    float shotDamage(float targetRadius, float shotDistanceFromTarget) {
      if (shotLanded(targetRadius,shotDistanceFromTarget)) {
        float collisionSlope = shotDistanceFromTarget/sqrt(targetRadius*targetRadius-shotDistanceFromTarget*shotDistanceFromTarget);
        float angleRelativeToSurface   = atan(1/collisionSlope);
        float fractionAimedTowardsCore = sin(angleRelativeToSurface);

        return getBaseDamage() * fractionAimedTowardsCore;
      } else {
        return 0.0;
      }
    }

    float testDamageSingleReport(float targetRadius, float distance) {
      float shotDistanceFromTarget = shotDistanceFromTargetCenter(distance);
      float damage;

      if(shotLanded(targetRadius,shotDistanceFromTarget)) {
        damage = shotDamage(targetRadius,shotDistanceFromTarget);
        cout << "Shot damage: " << damage << " out of a possible " << baseDamageMean * 2 << endl;
      } else {
        cout << "Shot missed." << endl;
      }

      return damage;
    }

    void testDamageMultiReport() {
      const int minDistancePower=3;
      const int maxDistancePower=5;

      const int minRadiusPower=1;
      const int maxRadiusPower=3;

      const int numTrials = 10;

      float totalDamage;

      for (int radiusPower=minRadiusPower;radiusPower<=maxRadiusPower;radiusPower++) {
        cout << endl << "Ship radius: " << pow(10.0,radiusPower) << "m" << endl;
        for (int distancePower=minDistancePower;distancePower<=maxDistancePower;distancePower++) {
          cout << endl << "Firing distance: " << pow(10.0,distancePower) << "m" << endl;

          totalDamage = 0.0;
          for (int i=0;i<numTrials;i++) {
            totalDamage+=testDamageSingleReport(pow(10.0,radiusPower),pow(10.0,distancePower));
          }
          cout << "Total: " << totalDamage << endl;
        }
      }
    }

    Turret(float reloadTime, float baseAimVariance, float baseDamageMean): reloadTime(reloadTime), baseAimVariance(baseAimVariance), baseDamageMean(baseDamageMean) {}
};

int main() {
  const float reloadTime      = 2.0;
  const float baseAimVariance = 0.01;
  const float baseDamageMean  = 1000.0;

  Turret turret(reloadTime,baseAimVariance,baseDamageMean);
  turret.testDamageMultiReport();
}

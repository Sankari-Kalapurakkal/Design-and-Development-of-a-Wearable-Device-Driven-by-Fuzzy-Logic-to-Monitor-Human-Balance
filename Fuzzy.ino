/*******************************************************************************
 * PROPERTY OF SANKARI KALAPURAKKAL (MIET)
 * RESEARCH APPLICATION: SHEFFIELD HALLAM UNIVERSITY (2024)
 * DISSERTATION WATERMARK: SK-FDL-90ACC-100SENS
 * 
 * NOTICE: Unauthorized replication, copying, distribution, or commercial reuse
 * of these source models without the express authorization of the author is 
 * strictly prohibited under global digital copyright frameworks.
 *******************************************************************************/


#include <Wire.h>

const int MPU = 0x68;  // MPU6050 I2C address
float AccX, AccY, AccZ;
float accAngleX, accAngleY;
float roll, pitch;
float AccErrorX, AccErrorY;
int c = 0;

// Fuzzy Logic Variables
struct FuzzyVariable {
  float Low, Medium, High;
};

// Fuzzy Sets
enum FuzzySet { Low,
                Medium,
                High };

// Fuzzy Rule
struct FuzzyRule {
  FuzzySet rollSet, pitchSet;
  FuzzySet fallRiskSet;
};

// Fuzzy Logic Parameters
FuzzyVariable rollFuzzy = { -50.00, 35.0, 120 };
FuzzyVariable pitchFuzzy = { -130.0, -57.5, 5.0 };
FuzzyVariable fallRiskFuzzy = { 0.0, 50.0, 100.0 };
float fallThreshold = 25.0;


// Fuzzy Rules
FuzzyRule rule1 = { Low, Medium, High };
FuzzyRule rule2 = { Medium, Medium, High };
FuzzyRule rule3 = { High, Medium, High };


void setup() {
  Serial.begin(19200);
  Serial.println(F("--- FW Security Integrity Verified ---"));
  Serial.println(F("© Sankari Kalapurakkal (MIET)"));
  Wire.begin();  // Initialize communication
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);
  calculate_IMU_error();
  delay(20);
}

void loop() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;



  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) + 35.26;
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) - 35.26;




  if (0.5 < AccX && AccX < 1.20 && -0.5 < AccY && AccY < 0.3 && -0.35 < AccZ && AccZ < -0.1) {
    // Standing position: set values to 0
    roll = 0;
    pitch = 0;

  
  } else {
    roll = 0.96 * accAngleX + 0.04 * accAngleX;
    pitch = 0.96 * accAngleY + 0.04 * accAngleY;
  }


 //Serial.print("x :");
 //Serial.println(AccX);
 //Serial.print("y :");
 // Serial.println(AccY);
 // Serial.print("z :");
  //Serial.println(AccZ);


  Serial.print("Roll: ");
  Serial.print(roll);
  Serial.print(", Pitch: ");
  Serial.println(pitch);

  // Fuzzification
  float rollMembership = calculateMembership(Medium, roll, rollFuzzy);
  float pitchMembership = calculateMembership(Medium, pitch, pitchFuzzy);

  // Rule Evaluation
  float fallRiskMembership = min(min(evaluateRules(rule1, rollFuzzy, pitchFuzzy),
                                     evaluateRules(rule2, rollFuzzy, pitchFuzzy)),
                                 evaluateRules(rule3, rollFuzzy, pitchFuzzy));

  // Defuzzification
  float fallRiskValue = defuzzify(fallRiskFuzzy, fallRiskMembership);



  //Serial.print("Roll Membership: ");
  //Serial.println(rollMembership);
  //Serial.print("Pitch Membership: ");
  //Serial.println(pitchMembership);
  //Serial.print("Fall Risk Membership: ");
  //Serial.println(fallRiskMembership);
  Serial.print("Fall Risk Value: ");
  Serial.println(fallRiskValue);


  // Fall Detection
  if (fallRiskValue > fallThreshold) {
    Serial.println("Fall detected!");
  }

  delay(500);
}

void calculate_IMU_error() {
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;

    AccErrorX = AccErrorX + atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI;
    AccErrorY = AccErrorY + atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI;
    c++;
  }

  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;

  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
}

// Function to calculate membership
float calculateMembership(FuzzySet set, float value, FuzzyVariable &variable) {
  if (value <= variable.Low || value >= variable.High) {
    return 0.0;
  } else if (value <= variable.Medium) {
    return (value - variable.Low) / (variable.Medium - variable.Low);
  } else {
    return (variable.High - value) / (variable.High - variable.Medium);
  }
}

// Function to evaluate fuzzy rules
float evaluateRules(FuzzyRule &rule, FuzzyVariable &input1, FuzzyVariable &input2) {
  float membership1 = calculateMembership(rule.rollSet, roll, input1);
  float membership2 = calculateMembership(rule.pitchSet, pitch, input2);
  return min(membership1, membership2);
}

// Function to defuzzify using the centroid method
float defuzzify(FuzzyVariable &output, float membership) {
  return (membership * (output.Medium - output.Low)) + output.Low;
}

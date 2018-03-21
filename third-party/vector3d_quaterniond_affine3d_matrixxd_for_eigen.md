# Vector3d   Quaterniond  Affine3d  MatrixXd for Eigen


##Eigen::Vector3d()

```cpp
Eigen::Vector3d pos;
pos << 0.0,-2.0,1.0;
Eigen::Vector3d pos2 = Eigen::Vector3d(1,-3,0.5);
pos = pos+pos2;

Eigen::Vector3d pos3 = 	Eigen::Vector3d::Zero();

pos.normalize();
pos.inverse();

//Dot and Cross Product
Eigen::Vector3d v(1, 2, 3);
Eigen::Vector3d w(0, 1, 2);

double vDotw = v.dot(w); // dot product of two vectors
Eigen::Vector3d vCrossw = v.cross(w); // cross product of two vectors
```

##Eigen::Quaterniond()
```cpp
Eigen::Quaterniond rot;
Eigen::Vector3d pos;
pos << 0.0,-2.0,1.0;


rot.setFromTwoVectors(Eigen::Vector3d(0,1,0), pos);
    
Eigen::Matrix<double,3,3> rotationMatrix;

rotationMatrix = rot.toRotationMatrix();

Eigen::Quaterniond q(2, 0, 1, -3); 
std::cout << "This quaternion consists of a scalar " << q.w() 
<< " and a vector " << std::endl << q.vec() << std::endl;

q.normalize();

std::cout << "To represent rotation, we need to normalize it such 
that its length is " << q.norm() << std::endl;

Eigen::Vector3d vec(1, 2, -1);
Eigen::Quaterniond p;
p.w() = 0;
p.vec() = vec;
Eigen::Quaterniond rotatedP = q * p * q.inverse(); 
Eigen::Vector3d rotatedV = rotatedP.vec();
std::cout << "We can now use it to rotate a vector " << std::endl 
<< vec << " to " << std::endl << rotatedV << std::endl;

// convert a quaternion to a 3x3 rotation matrix:
Eigen::Matrix3d R = q.toRotationMatrix(); 

std::cout << "Compare with the result using an rotation matrix " 
<< std::endl << R * vec << std::endl;

Eigen::Quaterniond a = Eigen::Quaterniond::Identity();
Eigen::Quaterniond b = Eigen::Quaterniond::Identity();
Eigen::Quaterniond c; 
// Adding two quaternion as two 4x1 vectors is not supported by the Eigen API.
//That is, c = a + b is not allowed. The solution is to add each element:

c.w() = a.w() + b.w();
c.x() = a.x() + b.x();
c.y() = a.y() + b.y();
c.z() = a.z() + b.z();
```

##Eigen::Affine3d()

Affine3d is a Pose type message( contains a Vector 3d and Quaterniond/RotationMatrix). It is great for computing several subsequent transformations.

```cpp
Eigen::Affine3d aff = Eigen::Affine3d::Identity();

aff.translation() = pos;
aff.translation() = Eigen::Vector3d(0.7,0.4,1.1);
aff.linear() = rot.toRotationMatrix();

aff.translate(Eigen::Vector3d(0,0.0,0.03));
aff.pretranslate(pos);

aff.rotate(Eigen::AngleAxisd(M_PI, Eigen::Vector3d(0,0,1)));
aff.prerotate(rot.toRotationMatrix());

Eigen::MatrixXd normalMatrix = aff.linear().inverse().transpose();
```

To get the translation and rotation portions:

```cpp
pos = aff.translation();
rot = aff.linear();
aff.linear().col(0);
```

##Eigen::MatrixXd()

```cpp
//Transpose and inverse:
Eigen::MatrixXd A(3, 2);
A << 1, 2,
     2, 3,
     3, 4;

Eigen::MatrixXd B = A.transpose();// the transpose of A is a 2x3 matrix
// computer the inverse of BA, which is a 2x2 matrix:
Eigen::MatrixXd C = (B * A).inverse();
C.determinant(); //compute determinant
Eigen::Matrix3d D = Eigen::Matrix3d::Identity();

Eigen::Matrix3d m = Eigen::Matrix3d::Random();
m = (m + Eigen::Matrix3d::Constant(1.2)) * 50;
Eigen::Vector3d v2(1,2,3);

cout << "m =" << endl << m << endl;
cout << "m * v2 =" << endl << m * v2 << endl;

//Accessing matrices:
Eigen::MatrixXd A2 = Eigen::MatrixXd::Random(7, 9);
std::cout << "The fourth row and 7th column element is " << A2(3, 6) << std::endl;

Eigen::MatrixXd B2 = A2.block(1, 2, 3, 3);
std::cout << "Take sub-matrix whose upper left corner is A(1, 2)" 
<< std::endl << B2 << std::endl;

Eigen::VectorXd a2 = A2.col(1); // take the second column of A
Eigen::VectorXd b2 = B2.row(0); // take the first row of B2

Eigen::VectorXd c2 = a2.head(3);// take the first three elements of a2
Eigen::VectorXd d2 = b2.tail(2);// take the last two elements of b2
```


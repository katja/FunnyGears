// #include "CylindricalGear.h"
// #include "definitions.h"
// #include "helpers.h"

// CylindricalGear::CylindricalGear() : Geometry() {}
// CylindricalGear::~CylindricalGear() {}

// // Um einen Grundschrägungswinkel beta (helixAngle) zu erzielen, muss das Stirnprofil bei
// // Verschiebung um z in Richtung der Zahnradachse um den Winkel(!) z * tan(beta) gedreht werden.
// void CylindricalGear::getTraverseProfile(hpreal z, vector<hpvec2>& gearProfile) {
//     hpuint nPointsPerTooth = gearProfile.size() / getNumberOfTeeth();
//     hpreal rotationHelixAngle = glm::tan(getHelixAngle()) * z;

//     vector<hpvec2> toothProfile(nPointsPerTooth + 1); //TODO: Change the "+1" and let toothProfile return the points only until next tooth starts
//     getToothProfile(toothProfile);
//     hpuint nTeeth = getNumberOfTeeth();

//     //Test if the points of the toothProfile are given in clockwise or counterclockwise direction
//     hpvec2 first = toothProfile[0];
//     hpvec2 last = toothProfile[nPointsPerTooth];
//     int rotDirection = 1;
//     if((first[0] * last[1] - first[1] * last[0]) < 0)
//         rotDirection = -1;

//     hpreal rotationConstant = rotDirection * 180.0f / M_PI * rotationHelixAngle;
//     hpreal rotationPerTooth = rotDirection * 360.0f / nTeeth;

//     hpreal degreeRotation = rotationConstant;
//     for(hpuint i = 0; i < nTeeth; ++i) {
//         for(hpuint j = 0; j < nPointsPerTooth; ++j) {
//             gearProfile[i * nPointsPerTooth + j] = glm::rotate(toothProfile[j], degreeRotation);
//         }
//         degreeRotation += rotationPerTooth;
//     }
// }

// TriangleMesh3D* CylindricalGear::toTriangleMesh(hpuint nPointsPerTooth, hpuint nTraverseProfiles) {
//     vector<hpvec2> profile(nPointsPerTooth * getNumberOfTeeth());
//     // Create vectors for the triangle mesh
//     vector<hpvec3>* verticesAndNormals = new vector<hpvec3>();
//     vector<hpuint>* indices = new vector<hpuint>();
//     verticesAndNormals->reserve((nTraverseProfiles + 1) * profile.size() * 2); // two times for vertices and normals
//     indices->reserve(nTraverseProfiles * profile.size() * 6);

//     hpvec3 wildcardNormal = hpvec3(0.0f, 0.0f, 0.0f);
//     hpreal dz = getFaceWidth() / nTraverseProfiles;

//     for(hpuint i = 0; i <= nTraverseProfiles; ++i) {
//         getTraverseProfile(i * dz, profile);
//         hpuint pointIndex = 0;
//         for(vector<hpvec2>::iterator j = profile.begin(), end = profile.end(); j != end; ++j) {
//             verticesAndNormals->push_back(hpvec3(j->x, j->y, i * dz));
//             verticesAndNormals->push_back(wildcardNormal);

//             if(i != nTraverseProfiles) {
//                 hpuint nextPointIndex = ((*j) == profile.back()) ? 0 : (pointIndex + 1);

//                 indices->push_back(i * profile.size() + nextPointIndex);  //0
//                 indices->push_back(i * profile.size() + pointIndex);  //1
//                 indices->push_back((i + 1) * profile.size() + pointIndex);  //2

//                 indices->push_back(i * profile.size() + nextPointIndex);  //0
//                 indices->push_back((i + 1) * profile.size() + pointIndex);  //2
//                 indices->push_back((i + 1) * profile.size() + nextPointIndex);  //3
//             }
//             ++pointIndex;
//         }
//     }
//     //insert correct smoothed normals:

//     //6 entries per two triangles in indices
//     hpuint indicesInRow = indices->size() / nTraverseProfiles;
//     hpuint trianglePairsInRow  = indicesInRow / 6;

//     //array steps is necessary to walk in the verticesAndNormals array to the right places
//     int steps[] = {0, 3, 4, -(indicesInRow - 3), -2, -3};

//     // go one step further in width direction to reach all points
//     for(hpuint i = 0; i <= nTraverseProfiles; ++i) {
//         for(hpuint j = 0; j < trianglePairsInRow; ++j) {
//             //calculate not normalized normals of the 6
//             //surrounding triangles and sum their area
//             //for every point of the gear profile
//             hpvec3 normal = hpvec3(0.0f);
//             int n = i * indicesInRow + j * 6;
//             for(hpuint k = 0; k < 6; ++k) {
//                 int da, db; //distances in verticesAndNormals array to other two triangle points
//                 if(k < 2) {
//                     da = 2; db = 1;
//                 } else if (k < 4) {
//                     da = -1; db = 1;
//                 } else {
//                     da = -1; db = -2;
//                 }

//                 //TODO: n ist int und wird mit hpuint verglichen! => static_cast???
//                 n += steps[k];
//                 if(k == 2 && j == trianglePairsInRow - 1)
//                     n -= indicesInRow;
//                 //not every point has 6 surrounding triangles. Use only the ones available:
//                 if(n >= 0 && n < indices->size()) {
//                     hpvec3 a = verticesAndNormals->at(2 * indices->at(n + da)) - verticesAndNormals->at(2 * indices->at(n));
//                     hpvec3 b = verticesAndNormals->at(2 * indices->at(n + db)) - verticesAndNormals->at(2 * indices->at(n));
//                     normal = normal + (hpvec3(glm::cross(a, b)));
//                 }
//             }
//             // TODO Something is still worng with the normals
//             //cout<<normal.x<<" "<<normal.y<<" "<<normal.z<<endl;
//             n = i * indicesInRow + j * 6;
//             for(hpuint k = 0; k < 6; ++k) {
//                 n += steps[k];
//                 if(k == 2 && j == trianglePairsInRow - 1)
//                     n -= indicesInRow;
//                 //not every point has 6 surrounding triangles. Use only the ones available:
//                 if(n >= 0 && n < indices->size())
//                     verticesAndNormals->at(2 * indices->at(n) + 1) = -glm::normalize(normal); //insert the normal in the cell after the vertex
//             }
//         }
//     }

//     return new TriangleMesh3D(verticesAndNormals, indices);
// }

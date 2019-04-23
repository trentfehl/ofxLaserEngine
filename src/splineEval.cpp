#include "splineEval.h"

void SplineEval::threadedFunction(){
    int n_evals = 100;
    results.resize(n_evals);

    float range = spline.domain().max() - spline.domain().min();

    for (std::size_t i=0, max=n_evals; i!=max; i++) {
        float location = (i / (double)n_evals * range) + spline.domain().min();
        results[i].point = spline.eval(location).result();
        // std::cout << "i: " << i << ", x: " << results[i].point[0]
        //                         << ", y: " << results[i].point[1]
        //                         << ", z: " << results[i].point[2] << std::endl;
    }

    stopThread();
}

//--------------------------------------------------------------
void SplineEval::updateControlPoints(){

    for (std::size_t i=0, max=controls.size(); i!=max; i++) {

        float step_size = 0.01;
        float heading_offset = (rand() % 10 + 1) * 0.001;

        // If point reaches edge, reflect it.
        if (glm::length(controls[i].point) >= 1) { 
            step_size = 0.02; // Make sure to get back across.

            glm::vec3 normal = {
                controls[i].point[0],
                controls[i].point[1],
                controls[i].point[2],
            };

            controls[i].heading = glm::reflect(controls[i].heading, normal);
        }

        controls[i].point[0] += glm::dot(controls[i].heading, {1 - heading_offset,0,0}) * step_size;
        controls[i].point[1] += glm::dot(controls[i].heading, {0,1 - heading_offset,0}) * step_size;
        controls[i].point[2] += glm::dot(controls[i].heading, {0,0,1 - heading_offset}) * step_size;
    }

    spline = tinyspline::BSpline(
        controls.size(),
        3,
        3,
        TS_CLAMPED
    );

    ctrlp = spline.controlPoints();

    // Setup control points.
    for (std::size_t i=0, max=controls.size(); i!=max; i++) {
        for (std::size_t j=0, max=3; j!=max; j++) {
            int index = i*3 + j;
            ctrlp[index] = controls[i].point[j];
        }
    }

    spline.setControlPoints(ctrlp);

    startThread();
}

//--------------------------------------------------------------
void SplineEval::setupControlPoints(){ 
    // Set number of control points.
    controls.resize(6);

    // Initialize random seed.
    srand (time(NULL));

    for (std::size_t i=0, max=controls.size(); i!=max; i++) {
        // Initialize unit vector position.
        controls[i].point = {
            rand() % 200 - 100,
            rand() % 200 - 100,
            rand() % 200 - 100,
        };
        controls[i].point = glm::normalize(controls[i].point);

        // Initialize unit vector heading.
        controls[i].heading = {
            rand() % 200 - 100,
            rand() % 200 - 100,
            rand() % 200 - 100,
        };
        controls[i].heading = glm::normalize(controls[i].heading);
    }
}
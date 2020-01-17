// Copyright 2019 Alexander Liniger

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#include "Tests/spline_test.h"
#include "Tests/model_integrator_test.h"
#include "Tests/constratins_test.h"
#include "Tests/cost_test.h"

#include "MPC/mpc.h"
#include "Model/integrator.h"
#include "Params/track.h"
#include "Plotting/plotting.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

int main() {

    using namespace mpcc;

    Param param = Param("Params/model.json");
    CostParam cost_param = CostParam("Params/cost.json");
    BoundsParam bounds_param = BoundsParam("Params/bounds.json");
    Track track = Track("Params/track.json");

    std::ifstream iConfig("Params/config.json");
    json jsonConfig;
    iConfig >> jsonConfig;

    Integrator integrator = Integrator(param);
    Model model = integrator.getModel();

//    std::cout << testSpline() << std::endl;
//    std::cout << testArcLengthSpline() << std::endl;
//
//    std::cout << testIntegrator(integrator) << std::endl;
//    std::cout << testLinModel(integrator) << std::endl;
//
//    std::cout << testAlphaConstraint(param, model) << std::endl;
//    std::cout << testTireForceConstraint(param, model) << std::endl;
//    std::cout << testTrackConstraint(param) << std::endl;
//
//    std::cout << testCost(cost_param) << std::endl;
//    return 0;


    Plotting plotter = Plotting(model);

    TrackPos track_xy = track.getTrack();

    std::list<MPCReturn> log;
    MPC mpc(jsonConfig["n_sqp"],jsonConfig["n_reset"],jsonConfig["sqp_mixing"], param, cost_param, bounds_param);
    mpc.setTrack(track_xy.X,track_xy.Y);
    State x0 = {track_xy.X(0),track_xy.Y(0),-1*M_PI/4.0,0.05,0,0,0,1.0,0,1.0};
    for(int i=0;i<jsonConfig["n_sim"];i++)
    {
        MPCReturn mpc_sol = mpc.runMPC(x0);
        x0 = integrator.simTimeStep(x0,mpc_sol.u0,0.02);
        log.push_back(mpc_sol);
    }
    plotter.plotRun(log,track_xy);
    plotter.plotSim(log,track_xy);

    double mean_time = 0.0;
    double max_time = 0.0;
    for(MPCReturn log_i : log)
    {
        mean_time += log_i.time_total;
        if(log_i.time_total > max_time)
            max_time = log_i.time_total;
    }
    std::cout << "mean nmpc time " << mean_time/double(jsonConfig["n_sim"]) << std::endl;
    std::cout << "max nmpc time " << max_time << std::endl;
    return 0;
}



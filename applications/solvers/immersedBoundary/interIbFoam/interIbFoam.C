/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | foam-extend: Open Source CFD
   \\    /   O peration     | Version:     4.0
    \\  /    A nd           | Web:         http://www.foam-extend.org
     \\/     M anipulation  | For copyright notice see file Copyright
-------------------------------------------------------------------------------
License
    This file is part of foam-extend.

    foam-extend is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    foam-extend is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with foam-extend.  If not, see <http://www.gnu.org/licenses/>.

Application
    interIbFoam

Description
    Solver for 2 incompressible, isothermal immiscible fluids using a VOF
    (volume of fluid) phase-fraction based interface capturing approach,
    with immersed boundary support

    The momentum and other fluid properties are of the "mixture" and a single
    momentum equation is solved.

    Turbulence modelling is generic, i.e. laminar, RAS or LES may be selected.

    For a two-fluid approach see twoPhaseEulerFoam.

Author
    Hrvoje Jasak, Wikki Ltd.  All rights reserved

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "interfaceProperties.H"
#include "twoPhaseMixture.H"
#include "turbulenceModel.H"
#include "pimpleControl.H"

#include "immersedBoundaryFvPatch.H"
#include "immersedBoundaryAdjustPhi.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
#   include "setRootCase.H"
#   include "createTime.H"
#   include "createMesh.H"

    pimpleControl pimple(mesh);

#   include "readGravitationalAcceleration.H"
#   include "initContinuityErrs.H"
#   include "createFields.H"
#   include "createIbMasks.H"
#   include "createTimeControls.H"
#   include "correctPhi.H"
#   include "CourantNo.H"
#   include "setInitialDeltaT.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "\nStarting time loop\n" << endl;

    while (runTime.run())
    {
#       include "readTimeControls.H"
#       include "immersedBoundaryCourantNo.H"
#       include "setDeltaT.H"

        runTime++;

        Info<< "Time = " << runTime.timeName() << nl << endl;

        // Pressure-velocity corrector
        while (pimple.loop())
        {
            twoPhaseProperties.correct();

#           include "UEqn.H"

            // --- PISO loop
            while (pimple.correct())
            {
#               include "pEqn.H"
            }

#           include "immersedBoundaryContinuityErrs.H"

#           include "limitU.H"

            p = pd + rho*gh;
            p.correctBoundaryConditions();

#           include "alphaEqn.H"

            turbulence->correct();
        }

        runTime.write();

        Info<< "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
            << "  ClockTime = " << runTime.elapsedClockTime() << " s"
            << nl << endl;
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //

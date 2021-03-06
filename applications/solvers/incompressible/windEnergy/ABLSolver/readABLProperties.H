    IOdictionary ABLProperties
    (
        IOobject
        (
            "ABLProperties",
            runTime.time().constant(),
            runTime,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        )
    );

    // PROPERTIES CONCERNING THE SOURCE TERMS.

       // Specify the type of source to use for momentum and temperature.  The
       // possible types are "given" and "computed".  
       // - The "given" type means that the source values are directly given
       //   and the momentum and temperature fields will react accordingly.  
       // - The "computed" type means that the mean velocity and temperature
       //   are given and the source terms that maintain them are computed. 
       word momentumSourceType(ABLProperties.lookup("momentumSourceType"));
       word temperatureSourceType(ABLProperties.lookup("temperatureSourceType"));

       // If giving the velocity and computing the sources, specify how the velocity
       // is given.  "component" means you enter the x, y, anc z components.
       // "speedAndDirection" means that you enter the horizontal wind speed, horizontal
       // direction, and vertical component.
       word velocityInputType(ABLProperties.lookup("velocityInputType"));

       // Read in the heights at which the sources are given.
       List<scalar> sourceHeightsMomentumSpecified(ABLProperties.lookup("sourceHeightsMomentum"));
       List<scalar> sourceHeightsTemperatureSpecified(ABLProperties.lookup("sourceHeightsTemperature"));

       // Read in the table of momentum source vs. time and height.
       List<List<scalar> > sourceTableMomentumX(ABLProperties.lookup("sourceTableMomentumX"));
       List<List<scalar> > sourceTableMomentumY(ABLProperties.lookup("sourceTableMomentumY"));
       List<List<scalar> > sourceTableMomentumZ(ABLProperties.lookup("sourceTableMomentumZ"));

       // Read in the table of temperature source vs. time and height.
       List<List<scalar> > sourceTableTemperature(ABLProperties.lookup("sourceTableTemperature"));


       // Check sizes of source tables.
       label nSourceMomentumHeights = sourceHeightsMomentumSpecified.size();
       label nSourceTemperatureHeights = sourceHeightsTemperatureSpecified.size();
       label nSourceMomentumXTimes = sourceTableMomentumX.size();
       label nSourceMomentumYTimes = sourceTableMomentumY.size();
       label nSourceMomentumZTimes = sourceTableMomentumZ.size();
       label nSourceTemperatureTimes = sourceTableTemperature.size();
       forAll(sourceTableMomentumX,i)
       {
           if (sourceTableMomentumX[i].size()-1 != nSourceMomentumHeights)
           {
               FatalErrorIn
               (
                   "Number of sourceTableMomentumX heights does not match sourceHeightsMomentum"
               )   << abort(FatalError);
           }
       }
       forAll(sourceTableMomentumY,i)
       {
           if (sourceTableMomentumY[i].size()-1 != nSourceMomentumHeights)
           {
               FatalErrorIn
               (
                   "Number of sourceTableMomentumY heights does not match sourceHeightsMomentum"
               )   << abort(FatalError);
           }
       }
       forAll(sourceTableMomentumZ,i)
       {
           if (sourceTableMomentumZ[i].size()-1 != nSourceMomentumHeights)
           {
               FatalErrorIn
               (
                   "Number of sourceTableMomentumZ heights does not match sourceHeightsMomentum"
               )   << abort(FatalError);
           }
       }
       forAll(sourceTableTemperature,i)
       {
           if (sourceTableTemperature[i].size()-1 != nSourceTemperatureHeights)
           {
               FatalErrorIn
               (
                   "Number of sourceTableTemperature heights does not match sourceHeightsTemperature"
               )   << abort(FatalError);
           }
       }


       // Break the source tables into interpolation tables.
       List<scalar> sourceMomentumXTimesSpecified(nSourceMomentumXTimes,0.0);
       List<scalar> sourceMomentumYTimesSpecified(nSourceMomentumYTimes,0.0);
       List<scalar> sourceMomentumZTimesSpecified(nSourceMomentumZTimes,0.0);
       List<scalar> sourceTemperatureTimesSpecified(nSourceTemperatureTimes,0.0);

       List<List<scalar> > sourceMomentumXSpecified(nSourceMomentumXTimes,List<scalar>(nSourceMomentumHeights,0.0));
       List<List<scalar> > sourceMomentumYSpecified(nSourceMomentumYTimes,List<scalar>(nSourceMomentumHeights,0.0));
       List<List<scalar> > sourceMomentumZSpecified(nSourceMomentumZTimes,List<scalar>(nSourceMomentumHeights,0.0));
       List<List<scalar> > sourceTemperatureSpecified(nSourceTemperatureTimes,List<scalar>(nSourceTemperatureHeights,0.0));

       for(int i = 0; i < nSourceMomentumXTimes; i++)
       {
           sourceMomentumXTimesSpecified[i] = sourceTableMomentumX[i][0];
           for(int j = 0; j < nSourceMomentumHeights; j++)
           {
               sourceMomentumXSpecified[i][j] = sourceTableMomentumX[i][j+1];
           }
       }

       for(int i = 0; i < nSourceMomentumYTimes; i++)
       {
           sourceMomentumYTimesSpecified[i] = sourceTableMomentumY[i][0];
           for(int j = 0; j < nSourceMomentumHeights; j++)
           {
               sourceMomentumYSpecified[i][j] = sourceTableMomentumY[i][j+1];
           }
       }

       for(int i = 0; i < nSourceMomentumZTimes; i++)
       {
           sourceMomentumZTimesSpecified[i] = sourceTableMomentumZ[i][0];
           for(int j = 0; j < nSourceMomentumHeights; j++)
           {
               sourceMomentumZSpecified[i][j] = sourceTableMomentumZ[i][j+1];
           }
       }

       for(int i = 0; i < nSourceTemperatureTimes; i++)
       {
           sourceTemperatureTimesSpecified[i] = sourceTableTemperature[i][0];
           for(int j = 0; j < nSourceTemperatureHeights; j++)
           {
               sourceTemperatureSpecified[i][j] = sourceTableTemperature[i][j+1];
           }
       }


       // If the desired mean wind or temperature is given at only one height, then revert to
       // the old way of specifying the source term.  Find the two grid levels that bracket
       // the given level and interpolate in between them to compute the source term.  So,
       // here, find those two levels.
       label hLevelsWind1I = 0;
       label hLevelsWind2I = 0;
       scalar hLevelsWind1 = 0;
       scalar hLevelsWind2 = 0;
       if ((momentumSourceType == "computed") && (nSourceMomentumHeights == 1))
       {
           #include "findWindHeight.H"
       }

       label hLevelsTemp1I = 0;
       label hLevelsTemp2I = 0;
       scalar hLevelsTemp1 = 0;
       scalar hLevelsTemp2 = 0;
       if ((temperatureSourceType == "computed") && (nSourceTemperatureHeights == 1))
       {
           #include "findTemperatureHeight.H"
       }


       // Relaxation factor on the source term application.
       scalar alphaMomentum(ABLProperties.lookupOrDefault<scalar>("alphaMomentum",1.0));
       scalar alphaTemperature(ABLProperties.lookupOrDefault<scalar>("alphaTemperature",1.0));




    // PROPERTIES CONCERNING CORIOLIS FORCES

       // Planetary rotation period (hours)
       scalar planetaryRotationPeriod(readScalar(ABLProperties.lookup("planetaryRotationPeriod")));

       // Latitude on the planetary body (degrees)
       scalar latitude(readScalar(ABLProperties.lookup("latitude")));

       // Compute the planetar rotation vector
       vector Omega_;
       Omega_.x() = 0.0;
       Omega_.y() = ((2.0 * Foam::constant::mathematical::pi) / (max(1.0E-5,planetaryRotationPeriod)*3600.0)) * Foam::cos(latitude*Foam::constant::mathematical::pi/180.0);
       Omega_.z() = ((2.0 * Foam::constant::mathematical::pi) / (max(1.0E-5,planetaryRotationPeriod)*3600.0)) * Foam::sin(latitude*Foam::constant::mathematical::pi/180.0);
       uniformDimensionedVectorField Omega
       (
           IOobject
           (
               "Omega",
               runTime.constant(),
               mesh,
               IOobject::NO_READ,
               IOobject::NO_WRITE
           ),
           dimensionedVector("Omega",dimensionSet(0, 0, -1, 0, 0, 0, 0),Omega_)
       );

       Info << Omega << endl;       



    // PROPERTIES CONCERNING THE WAY IN WHICH PERTURBATION PRESSURE IS DEFINED

       // Options for defining the perturbation pressure:
       // - noSplit:   do not split out hydrostatic part; pressure is then perturbation pressure.
       // - rho0Split: split out the hydrostatic part; define hydrostatic as rho_0 * g * z.
       // - rhokSplit: split out the hydrostatic part; define hydrostatic as rho_k * g * z.
       word perturbationPressureType(ABLProperties.lookupOrDefault<word>("perturbationPressureType","rhokSplit"));
       word perturbationOutput;
       if (perturbationPressureType == "noSplit")
       {
           perturbationOutput = "nothing";
       }
       else if (perturbationPressureType == "rho0Split")
       {
           perturbationOutput = "rho_0 * g * z";
       }
       else if (perturbationPressureType == "rhokSplit")
       {
           perturbationOutput = "rho_k * g * z";
       }
       Info << "Defining background hydrostatic pressure to be " << perturbationOutput << endl;


       // This switch dictates whether or not the pressure reference cell is set explicitly
       // in the p_rgh solve.  If true, pressure is constrained at the pressure reference
       // cell by manipulating the matrix row for that cell to remove the null space.  This
       // assures that the pressure level is constrained, but it also means the continuity
       // equation is no longer solved at that cell, so the divergence error can be significant
       // enough there to cause localized oscillations.  Iterative solvers can still converge
       // even with a null space, but more iterations may be needed, so this switch can be 
       // set to false.
       bool activatePressureRefCell(ABLProperties.lookupOrDefault<bool>("activatePressureRefCell", true));
       if (activatePressureRefCell)
       {
            Info << "Pressure reference cell matrix row modification enabled" << endl;
       }
       else
       {
            Info << "Pressure reference cell matrix row modification not enabled" << endl;
       }



     
    // PROPERTIES CONCERNING SPONGE LAYER

       // Specify the type of sponge layer to use.  The
       // possible types are "Rayleigh", "viscous" or "none".  
       // - The "Rayleigh" type means that the damping term is computed as nu*(u_ref-u)
       //   The viscosity coefficient nu has dimensions of 1/s
       // - The "viscous" type means that the damping term is computed as nu * Lapl(u)
       //   The viscosity coefficient nu has dimensions of m**2/s
       // - The "none" type means no damping is added
       word spongeLayerType(ABLProperties.lookupOrDefault<word>("spongeLayerType","none"));
       
       // Sponge layer base height
       scalar spongeLayerBaseHeight(ABLProperties.lookupOrDefault<scalar>("spongeLayerBaseHeight",0.0));

       // Sponge layer top height
       scalar spongeLayerTopHeight(ABLProperties.lookupOrDefault<scalar>("spongeLayerTopHeight",10000.0));

       // Sponge layer viscosity at the top boundary
       scalar spongeLayerViscosityTop(ABLProperties.lookupOrDefault<scalar>("spongeLayerViscosityTop",0.0));


       // Create sponge layer reference velocity
       scalar spongeLayerUx(ABLProperties.lookupOrDefault<scalar>("spongeLayerUx",0.0));
       scalar spongeLayerUy(ABLProperties.lookupOrDefault<scalar>("spongeLayerUy",0.0));
       vector Uref_;
       Uref_.x() = spongeLayerUx;
       Uref_.y() = spongeLayerUy;
       Uref_.z() = 0.0;
       uniformDimensionedVectorField spongeLayerReferenceVelocity
       (
           IOobject
           (
               "spongeLayerReferenceVelocity",
               runTime.constant(),
               mesh,
               IOobject::NO_READ,
               IOobject::NO_WRITE
           ),
           dimensionedVector("spongeLayerReferenceVelocity",dimensionSet(0, 1, -1, 0, 0, 0, 0),Uref_)
       );
       
       if (spongeLayerType == "Rayleigh")
       {
           Info << spongeLayerReferenceVelocity << endl;
       }


    // PROPERTIES CONCERNING GATHERING STATISTICS

       // Gather/write statistics?
       bool statisticsOn(ABLProperties.lookupOrDefault<bool>("statisticsOn", false));

       // Statistics gathering/writing frequency?
       int statisticsFreq(int(readScalar(ABLProperties.lookup("statisticsFrequency"))));

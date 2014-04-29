#include <string>
using namespace std;

#include "common.h"

#include <QApplication>
#include "DGtal/io/viewers/Viewer3D.h"
#include "DGtal/io/boards/Board2D.h"
#include "DGtal/io/colormaps/GradientColorMap.h"
#include "DGtal/io/readers/GenericReader.h"
using namespace DGtal;

void solve2d_laplacian()
{
    trace.beginBlock("2d discrete exterior calculus solve laplacian");

    const Z2i::Domain domain(Z2i::Point(0,0), Z2i::Point(9,9));

    // create discrete exterior calculus from set
    //! [calculus_creation]
    typedef DiscreteExteriorCalculus<Z2i::Domain, EigenSparseLinearAlgebraBackend> Calculus;
    Calculus calculus(generateRingSet(domain));
    //! [calculus_creation]
    trace.info() << calculus << endl;

    //! [laplacian_definition]
    Calculus::DualDerivative0 d0 = calculus.derivative<0, DUAL>();
    Calculus::PrimalDerivative1 d1p = calculus.derivative<1, PRIMAL>();
    Calculus::DualHodge1 hodge1 = calculus.dualHodge<1>();
    Calculus::PrimalHodge2 hodge2p = calculus.primalHodge<2>();
    Calculus::DualIdentity0 laplacian = hodge2p * d1p * hodge1 * d0 + 0.01 * calculus.identity<0, DUAL>();
    //! [laplacian_definition]
    trace.info() << "d0 = " << d0 << endl;
    trace.info() << "hodge1 = " << hodge1 << endl;
    trace.info() << "d1p = " << d1p << endl;
    trace.info() << "hodge2p = " << hodge2p << endl;
    trace.info() << "laplacian = " << laplacian << endl;

    //! [dirac_definition]
    Calculus::DualForm0 dirac(calculus);
    dirac.myContainer(calculus.getSCellIndex(calculus.kspace.sSpel(Z2i::Point(2,5)))) = 1;
    //! [dirac_definition]

    {
        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(0,1);
        Board2D board;
        board << domain;
        Calculus::Accum accum(calculus);
        dirac.applyToAccum(accum);
        accum.display2D(board, colormap);
        board.saveSVG("solve_laplacian_calculus.svg");
    }

    { // simplicial llt
        trace.beginBlock("simplicial llt");

        //! [solve_llt]
        typedef EigenSparseLinearAlgebraBackend::SolverSimplicialLLT LinearAlgebraSolver;
        typedef DiscreteExteriorCalculusSolver<Calculus, LinearAlgebraSolver, 0, DUAL, 0, DUAL> Solver;

        Solver solver;
        solver.compute(laplacian);
        Calculus::DualForm0 solution = solver.solve(dirac);

        trace.info() << solver.isValid() << " " << solver.solver.info() << endl;
        //! [solve_llt]
        trace.info() << solution << endl;
        trace.endBlock();

        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(solution.myContainer.minCoeff(),solution.myContainer.maxCoeff());
        Board2D board;
        board << domain;
        Calculus::Accum accum(calculus);
        solution.applyToAccum(accum);
        accum.display2D(board, colormap);
        board.saveSVG("solve_laplacian_simplicial_llt.svg");
    }

    { // simplicial ldlt
        trace.beginBlock("simplicial ldlt");

        //! [solve_ldlt]
        typedef EigenSparseLinearAlgebraBackend::SolverSimplicialLDLT LinearAlgebraSolver;
        typedef DiscreteExteriorCalculusSolver<Calculus, LinearAlgebraSolver, 0, DUAL, 0, DUAL> Solver;

        Solver solver;
        solver.compute(laplacian);
        Calculus::DualForm0 solution = solver.solve(dirac);

        trace.info() << solver.isValid() << " " << solver.solver.info() << endl;
        //! [solve_ldlt]
        trace.info() << solution << endl;
        trace.endBlock();

        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(solution.myContainer.minCoeff(),solution.myContainer.maxCoeff());
        Board2D board;
        board << domain;
        Calculus::Accum accum(calculus);
        solution.applyToAccum(accum);
        accum.display2D(board, colormap);
        board.saveSVG("solve_laplacian_simplicial_ldlt.svg");
    }

    { // conjugate gradient
        trace.beginBlock("conjugate gradient");

        //! [solve_conjugate_gradient]
        typedef EigenSparseLinearAlgebraBackend::SolverConjugateGradient LinearAlgebraSolver;
        typedef DiscreteExteriorCalculusSolver<Calculus, LinearAlgebraSolver, 0, DUAL, 0, DUAL> Solver;

        Solver solver;
        solver.compute(laplacian);
        Calculus::DualForm0 solution = solver.solve(dirac);
        //! [solve_conjugate_gradient]

        trace.info() << solver.isValid() << " " << solver.solver.info() << endl;
        trace.info() << solution << endl;
        trace.endBlock();

        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(solution.myContainer.minCoeff(),solution.myContainer.maxCoeff());
        Board2D board;
        board << domain;
        Calculus::Accum accum(calculus);
        solution.applyToAccum(accum);
        accum.display2D(board, colormap);
        board.saveSVG("solve_laplacian_conjugate_gradient.svg");
    }

    { // biconjugate gradient stabilized
        trace.beginBlock("biconjugate gradient stabilized (bicgstab)");

        //! [solve_biconjugate_gradient]
        typedef EigenSparseLinearAlgebraBackend::SolverBiCGSTAB LinearAlgebraSolver;
        typedef DiscreteExteriorCalculusSolver<Calculus, LinearAlgebraSolver, 0, DUAL, 0, DUAL> Solver;

        Solver solver;
        solver.compute(laplacian);
        Calculus::DualForm0 solution = solver.solve(dirac);
        //! [solve_biconjugate_gradient]

        trace.info() << solver.isValid() << " " << solver.solver.info() << endl;
        trace.info() << solution << endl;
        trace.endBlock();

        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(solution.myContainer.minCoeff(),solution.myContainer.maxCoeff());
        Board2D board;
        board << domain;
        Calculus::Accum accum(calculus);
        solution.applyToAccum(accum);
        accum.display2D(board, colormap);
        board.saveSVG("solve_laplacian_bicgstab.svg");
    }

    { // sparselu
        trace.beginBlock("sparse lu");

        //! [solve_sparse_lu]
        typedef EigenSparseLinearAlgebraBackend::SolverSparseLU LinearAlgebraSolver;
        typedef DiscreteExteriorCalculusSolver<Calculus, LinearAlgebraSolver, 0, DUAL, 0, DUAL> Solver;

        Solver solver;
        solver.compute(laplacian);
        Calculus::DualForm0 solution = solver.solve(dirac);
        //! [solve_sparse_lu]

        trace.info() << solver.isValid() << " " << solver.solver.info() << endl;
        trace.info() << solution << endl;
        trace.endBlock();

        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(solution.myContainer.minCoeff(),solution.myContainer.maxCoeff());
        Board2D board;
        board << domain;
        Calculus::Accum accum(calculus);
        solution.applyToAccum(accum);
        accum.display2D(board, colormap);
        board.saveSVG("solve_laplacian_sparse_lu.svg");
    }

    { // sparseqr
        trace.beginBlock("sparse qr");

        //! [solve_sparse_qr]
        typedef EigenSparseLinearAlgebraBackend::SolverSparseQR LinearAlgebraSolver;
        typedef DiscreteExteriorCalculusSolver<Calculus, LinearAlgebraSolver, 0, DUAL, 0, DUAL> Solver;

        Solver solver;
        solver.compute(laplacian);
        Calculus::DualForm0 solution = solver.solve(dirac);
        //! [solve_sparse_qr]

        trace.info() << solver.isValid() << " " << solver.solver.info() << endl;
        trace.info() << solution << endl;
        trace.endBlock();

        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(solution.myContainer.minCoeff(),solution.myContainer.maxCoeff());
        Board2D board;
        board << domain;
        Calculus::Accum accum(calculus);
        solution.applyToAccum(accum);
        accum.display2D(board, colormap);
        board.saveSVG("solve_laplacian_sparse_qr.svg");
    }

    trace.endBlock();
}

void solve2d_dual_decomposition()
{
    trace.beginBlock("2d discrete exterior calculus solve dual helmoltz decomposition");

    const Z2i::Domain domain(Z2i::Point(0,0), Z2i::Point(44,29));

    // create discrete exterior calculus from set
    typedef DiscreteExteriorCalculus<Z2i::Domain, EigenSparseLinearAlgebraBackend> Calculus;
    Calculus calculus(generateDoubleRingSet(domain));
    trace.info() << calculus << endl;

    // choose linear solver
    typedef EigenSparseLinearAlgebraBackend::SolverSparseQR LinearAlgebraSolver;

    //! [2d_dual_decomposition_operator_definition]
    const Calculus::DualDerivative0 d0 = calculus.derivative<0, DUAL>();
    const Calculus::DualDerivative1 d1 = calculus.derivative<1, DUAL>();
    const Calculus::PrimalDerivative0 d0p = calculus.derivative<0, PRIMAL>();
    const Calculus::PrimalDerivative1 d1p = calculus.derivative<1, PRIMAL>();
    const Calculus::DualHodge1 h1 = calculus.dualHodge<1>();
    const Calculus::DualHodge2 h2 = calculus.dualHodge<2>();
    const Calculus::PrimalHodge1 h1p = calculus.primalHodge<1>();
    const Calculus::PrimalHodge2 h2p = calculus.primalHodge<2>();
    const LinearOperator<Calculus, 1, DUAL, 0, DUAL> ad1 = h2p * d1p * h1;
    const LinearOperator<Calculus, 2, DUAL, 1, DUAL> ad2 = h1p * d0p * h2;
    //! [2d_dual_decomposition_operator_definition]

    //! [2d_dual_decomposition_input_field_definition]
    Calculus::DualVectorField input_vector_field(calculus);
    for (Calculus::Index ii=0; ii<calculus.kFormLength(0, DUAL); ii++)
    {
        const Z2i::RealPoint cell_center = Z2i::RealPoint(calculus.getSCell(0, DUAL, ii).myCoordinates)/2.;
        input_vector_field.myCoordinates[0](ii) = cos(-.5*cell_center[0]+ .3*cell_center[1]);
        input_vector_field.myCoordinates[1](ii) = cos(.4*cell_center[0]+ .8*cell_center[1]);
    }

    const Calculus::DualForm1 input_one_form = calculus.flat(input_vector_field);
    const Calculus::DualForm0 input_one_form_anti_derivated = ad1 * input_one_form;
    const Calculus::DualForm2 input_one_form_derivated = d1 * input_one_form;
    //! [2d_dual_decomposition_input_field_definition]

    {
        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(-1,1);
        Board2D board;
        board << domain;
        Calculus::Accum accum(calculus);
        input_one_form.applyToAccum(accum);
        accum.display2D(board, colormap);
        input_vector_field.display2D(board,.75);
        board.saveSVG("solve_2d_dual_decomposition_calculus.svg");
    }


    Calculus::DualForm0 solution_curl_free(calculus);
    { // solve curl free problem
        trace.beginBlock("solving curl free component");

        //! [2d_dual_decomposition_curl_free_solve]
        typedef DiscreteExteriorCalculusSolver<Calculus, LinearAlgebraSolver, 0, DUAL, 0, DUAL> Solver;
        Solver solver;
        solver.compute(ad1 * d0);
        solution_curl_free = solver.solve(input_one_form_anti_derivated);
        //! [2d_dual_decomposition_curl_free_solve]

        trace.info() << solver.isValid() << " " << solver.solver.info() << endl;
        trace.info() << "min=" << solution_curl_free.myContainer.minCoeff() << " max=" << solution_curl_free.myContainer.maxCoeff() << endl;
        trace.endBlock();
    }

    {
        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(solution_curl_free.myContainer.minCoeff(),solution_curl_free.myContainer.maxCoeff());
        Board2D board;
        board << domain;
        Calculus::Accum accum(calculus);
        solution_curl_free.applyToAccum(accum);
        accum.display2D(board, colormap);
        calculus.sharp(d0*solution_curl_free).display2D(board,.75);
        board.saveSVG("solve_2d_dual_decomposition_curl_free.svg");
    }

    Calculus::DualForm2 solution_div_free(calculus);
    { // solve divergence free problem
        trace.beginBlock("solving divergence free component");

        //! [2d_dual_decomposition_div_free_solve]
        typedef DiscreteExteriorCalculusSolver<Calculus, LinearAlgebraSolver, 2, DUAL, 2, DUAL> Solver;
        Solver solver;
        solver.compute(d1 * ad2);
        solution_div_free = solver.solve(input_one_form_derivated);
        //! [2d_dual_decomposition_div_free_solve]

        trace.info() << solver.isValid() << " " << solver.solver.info() << endl;
        trace.info() << "min=" << solution_div_free.myContainer.minCoeff() << " max=" << solution_div_free.myContainer.maxCoeff() << endl;
        trace.endBlock();
    }

    {
        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(solution_div_free.myContainer.minCoeff(),solution_div_free.myContainer.maxCoeff());
        Board2D board;
        board << domain;
        Calculus::Accum accum(calculus);
        solution_div_free.applyToAccum(accum);
        accum.display2D(board, colormap);
        calculus.sharp(ad2*solution_div_free).display2D(board,1.5);
        board.saveSVG("solve_2d_dual_decomposition_div_free.svg");
    }

    //! [2d_dual_decomposition_solution]
    const Calculus::DualForm1 solution_harmonic = input_one_form - d0*solution_curl_free - ad2*solution_div_free;
    //! [2d_dual_decomposition_solution]
    trace.info() << "min=" << solution_harmonic.myContainer.minCoeff() << " max=" << solution_harmonic.myContainer.maxCoeff() << endl;

    {
        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(solution_harmonic.myContainer.minCoeff(),solution_harmonic.myContainer.maxCoeff());
        Board2D board;
        board << domain;
        Calculus::Accum accum(calculus);
        solution_harmonic.applyToAccum(accum);
        accum.display2D(board, colormap);
        calculus.sharp(solution_harmonic).display2D(board, 20.);
        board.saveSVG("solve_2d_dual_decomposition_harmonic.svg");
    }

    trace.endBlock();
}

void solve2d_primal_decomposition()
{
    trace.beginBlock("2d discrete exterior calculus solve primal helmoltz decomposition");

    const Z2i::Domain domain(Z2i::Point(0,0), Z2i::Point(44,29));

    // create discrete exterior calculus from set
    typedef DiscreteExteriorCalculus<Z2i::Domain, EigenSparseLinearAlgebraBackend> Calculus;
    Calculus calculus(generateDoubleRingSet(domain));
    trace.info() << calculus << endl;

    // choose linear solver
    typedef EigenSparseLinearAlgebraBackend::SolverSparseQR LinearAlgebraSolver;

    //! [2d_primal_decomposition_operator_definition]
    const Calculus::PrimalDerivative0 d0 = calculus.derivative<0, PRIMAL>();
    const Calculus::PrimalDerivative1 d1 = calculus.derivative<1, PRIMAL>();
    const Calculus::DualDerivative0 d0p = calculus.derivative<0, DUAL>();
    const Calculus::DualDerivative1 d1p = calculus.derivative<1, DUAL>();
    const Calculus::PrimalHodge1 h1 = calculus.primalHodge<1>();
    const Calculus::PrimalHodge2 h2 = calculus.primalHodge<2>();
    const Calculus::DualHodge1 h1p = calculus.dualHodge<1>();
    const Calculus::DualHodge2 h2p = calculus.dualHodge<2>();
    const LinearOperator<Calculus, 1, PRIMAL, 0, PRIMAL> ad1 = h2p * d1p * h1;
    const LinearOperator<Calculus, 2, PRIMAL, 1, PRIMAL> ad2 = h1p * d0p * h2;
    //! [2d_primal_decomposition_operator_definition]

    //! [2d_primal_decomposition_input_field_definition]
    Calculus::PrimalVectorField input_vector_field(calculus);
    for (Calculus::Index ii=0; ii<calculus.kFormLength(0, PRIMAL); ii++)
    {
        const Z2i::RealPoint cell_center = Z2i::RealPoint(calculus.getSCell(0, PRIMAL, ii).myCoordinates)/2.;
        input_vector_field.myCoordinates[0](ii) = cos(-.5*cell_center[0]+ .3*cell_center[1]);
        input_vector_field.myCoordinates[1](ii) = cos(.4*cell_center[0]+ .8*cell_center[1]);
    }

    const Calculus::PrimalForm1 input_one_form = calculus.flat(input_vector_field);
    const Calculus::PrimalForm0 input_one_form_anti_derivated = ad1 * input_one_form;
    const Calculus::PrimalForm2 input_one_form_derivated = d1 * input_one_form;
    //! [2d_primal_decomposition_input_field_definition]

    {
        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(-1,1);
        Board2D board;
        board << domain;
        Calculus::Accum accum(calculus);
        input_one_form.applyToAccum(accum);
        accum.display2D(board, colormap);
        input_vector_field.display2D(board,.75);
        board.saveSVG("solve_2d_primal_decomposition_calculus.svg");
    }


    Calculus::PrimalForm0 solution_curl_free(calculus);
    { // solve curl free problem
        trace.beginBlock("solving curl free component");

        //! [2d_primal_decomposition_curl_free_solve]
        typedef DiscreteExteriorCalculusSolver<Calculus, LinearAlgebraSolver, 0, PRIMAL, 0, PRIMAL> Solver;
        Solver solver;
        solver.compute(ad1 * d0);
        solution_curl_free = solver.solve(input_one_form_anti_derivated);
        //! [2d_primal_decomposition_curl_free_solve]

        trace.info() << solver.isValid() << " " << solver.solver.info() << endl;
        trace.info() << "min=" << solution_curl_free.myContainer.minCoeff() << " max=" << solution_curl_free.myContainer.maxCoeff() << endl;
        trace.endBlock();
    }

    {
        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(solution_curl_free.myContainer.minCoeff(),solution_curl_free.myContainer.maxCoeff());
        Board2D board;
        board << domain;
        Calculus::Accum accum(calculus);
        solution_curl_free.applyToAccum(accum);
        accum.display2D(board, colormap);
        calculus.sharp(d0*solution_curl_free).display2D(board,.75);
        board.saveSVG("solve_2d_primal_decomposition_curl_free.svg");
    }

    Calculus::PrimalForm2 solution_div_free(calculus);
    { // solve divergence free problem
        trace.beginBlock("solving divergence free component");

        //! [2d_primal_decomposition_div_free_solve]
        typedef DiscreteExteriorCalculusSolver<Calculus, LinearAlgebraSolver, 2, PRIMAL, 2, PRIMAL> Solver;
        Solver solver;
        solver.compute(d1 * ad2);
        solution_div_free = solver.solve(input_one_form_derivated);
        //! [2d_primal_decomposition_div_free_solve]

        trace.info() << solver.isValid() << " " << solver.solver.info() << endl;
        trace.info() << "min=" << solution_div_free.myContainer.minCoeff() << " max=" << solution_div_free.myContainer.maxCoeff() << endl;
        trace.endBlock();
    }

    {
        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(solution_div_free.myContainer.minCoeff(),solution_div_free.myContainer.maxCoeff());
        Board2D board;
        board << domain;
        Calculus::Accum accum(calculus);
        solution_div_free.applyToAccum(accum);
        accum.display2D(board, colormap);
        calculus.sharp(ad2*solution_div_free).display2D(board,1.5);
        board.saveSVG("solve_2d_primal_decomposition_div_free.svg");
    }

    //! [2d_primal_decomposition_solution]
    const Calculus::PrimalForm1 solution_harmonic = input_one_form - d0*solution_curl_free - ad2*solution_div_free;
    //! [2d_primal_decomposition_solution]
    trace.info() << "min=" << solution_harmonic.myContainer.minCoeff() << " max=" << solution_harmonic.myContainer.maxCoeff() << endl;

    {
        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(solution_harmonic.myContainer.minCoeff(),solution_harmonic.myContainer.maxCoeff());
        Board2D board;
        board << domain;
        Calculus::Accum accum(calculus);
        solution_harmonic.applyToAccum(accum);
        accum.display2D(board, colormap);
        calculus.sharp(solution_harmonic).display2D(board, 30.);
        board.saveSVG("solve_2d_primal_decomposition_harmonic.svg");
    }

    trace.endBlock();
}

void solve3d_decomposition()
{
    trace.beginBlock("3d discrete exterior calculus solve helmoltz decomposition");

    const Z3i::Domain domain(Z3i::Point(0,0,0), Z3i::Point(19,19,9));

    // choose linear solver
    typedef EigenSparseLinearAlgebraBackend::SolverSparseQR LinearAlgebraSolver;

    //! [3d_decomposition_structure]
    // create discrete exterior calculus from set
    typedef DiscreteExteriorCalculus<Z3i::Domain, EigenSparseLinearAlgebraBackend> Calculus;
    Calculus calculus(domain);

    // outer ring
    for (int kk=2; kk<=18; kk++)
        for (int ll=4; ll<=36; ll++)
        {
            Calculus::SCell cell;
            Calculus::Scalar weight;

            cell = calculus.kspace.sCell(Z3i::Point(ll,4,kk));
            weight = 1;
            calculus.insertSCell( cell, weight ); // may be flipped

            cell = calculus.kspace.sCell(Z3i::Point(ll,36,kk));
            weight = 1;
            calculus.insertSCell( cell, weight );

            cell = calculus.kspace.sCell(Z3i::Point(4,ll,kk));
            weight = 1;
            calculus.insertSCell( cell, weight ); // may be flipped

            cell = calculus.kspace.sCell(Z3i::Point(36,ll,kk));
            weight = 1;
            calculus.insertSCell( cell, weight );
        }

    // inner ring
    for (int kk=2; kk<=18; kk++)
        for (int ll=16; ll<=24; ll++)
        {
            Calculus::SCell cell;
            Calculus::Scalar weight;

            cell = calculus.kspace.sCell(Z3i::Point(ll,16,kk));
            weight = 1;
            calculus.insertSCell( cell, weight );

            cell = calculus.kspace.sCell(Z3i::Point(ll,24,kk));
            weight = 1;
            calculus.insertSCell( cell, weight ); // may be flipped

            cell = calculus.kspace.sCell(Z3i::Point(16,ll,kk));
            weight = 1;
            calculus.insertSCell( cell, weight );

            cell = calculus.kspace.sCell(Z3i::Point(24,ll,kk));
            weight = 1;
            calculus.insertSCell( cell, weight ); // may be flipped
        }

    // top and bottom
    for (int kk=4; kk<=36; kk++)
        for (int ll=0; ll<=12; ll++)
        {
            Calculus::SCell cell;
            Calculus::Scalar weight;

            cell = calculus.kspace.sCell(Z3i::Point(4+ll,kk,2));
            weight = 1;
            calculus.insertSCell( cell, weight ); // may be flipped

            cell = calculus.kspace.sCell(Z3i::Point(4+ll,kk,18));
            weight = 1;
            calculus.insertSCell( cell, weight );

            cell = calculus.kspace.sCell(Z3i::Point(24+ll,kk,2));
            weight = 1;
            calculus.insertSCell( cell, weight ); // may be flipped

            cell = calculus.kspace.sCell(Z3i::Point(24+ll,kk,18));
            weight = 1;
            calculus.insertSCell( cell, weight );
        }

    // top and bottom
    for (int kk=0; kk<=12; kk++)
        for (int ll=16; ll<=24; ll++)
        {
            Calculus::SCell cell;
            Calculus::Scalar weight;

            cell = calculus.kspace.sCell(Z3i::Point(ll,4+kk,2));
            weight = 1;
            calculus.insertSCell( cell, weight ); // may be flipped

            cell = calculus.kspace.sCell(Z3i::Point(ll,4+kk,18));
            weight = 1;
            calculus.insertSCell( cell, weight );

            cell = calculus.kspace.sCell(Z3i::Point(ll,24+kk,2));
            weight = 1;
            calculus.insertSCell( cell, weight ); // may be flipped

            cell = calculus.kspace.sCell(Z3i::Point(ll,24+kk,18));
            weight = 1;
            calculus.insertSCell( cell, weight );
        }
    //! [3d_decomposition_structure]

    trace.info() << calculus << endl;

    {
        typedef Viewer3D<Z3i::Space, Z3i::KSpace> Viewer;
        Viewer* viewer = new Viewer();
        viewer->show();
        viewer->setWindowTitle("structure");
        (*viewer) << CustomColors3D(DGtal::Color(255,0,0), DGtal::Color(0,0,0));
        (*viewer) << domain;
        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(0, 1);
        Calculus::Accum accum(calculus);
        accum.display3D(*viewer, colormap);
        (*viewer) << Viewer::updateDisplay;
    }

    //! [3d_decomposition_operator_definition]
    const Calculus::PrimalDerivative0 d0 = calculus.derivative<0, PRIMAL>();
    const Calculus::PrimalDerivative1 d1 = calculus.derivative<1, PRIMAL>();
    const Calculus::DualDerivative1 d1p = calculus.derivative<1, DUAL>();
    const Calculus::DualDerivative2 d2p = calculus.derivative<2, DUAL>();
    const Calculus::PrimalHodge1 h1 = calculus.primalHodge<1>();
    const Calculus::PrimalHodge2 h2 = calculus.primalHodge<2>();
    const Calculus::DualHodge2 h2p = calculus.dualHodge<2>();
    const Calculus::DualHodge3 h3p = calculus.dualHodge<3>();
    const LinearOperator<Calculus, 1, PRIMAL, 0, PRIMAL> ad1 = h3p * d2p * h1;
    const LinearOperator<Calculus, 2, PRIMAL, 1, PRIMAL> ad2 = h2p * d1p * h2;
    //! [3d_decomposition_operator_definition]

    //! [3d_decomposition_input_field_definition]
    Calculus::PrimalVectorField input_vector_field(calculus);
    for (Calculus::Index ii=0; ii<calculus.kFormLength(0, PRIMAL); ii++)
    {
        const Z3i::RealPoint cell_center = Z3i::RealPoint(calculus.getSCell(0, PRIMAL, ii).myCoordinates)/2.;
        input_vector_field.myCoordinates[0](ii) = -cos(-.3*cell_center[0] + .6*cell_center[1] + .8*cell_center[2]);
        input_vector_field.myCoordinates[1](ii) = sin(.8*cell_center[0] + .3*cell_center[1] - .4*cell_center[2]);
        input_vector_field.myCoordinates[2](ii) = -cos(cell_center[2]*.5);
    }

    const Calculus::PrimalForm1 input_one_form = calculus.flat(input_vector_field);
    //! [3d_decomposition_input_field_definition]
    const Calculus::PrimalForm0 input_one_form_anti_derivated = ad1 * input_one_form;
    const Calculus::PrimalForm2 input_one_form_derivated = d1 * input_one_form;

    {
        typedef Viewer3D<Z3i::Space, Z3i::KSpace> Viewer;
        Viewer* viewer = new Viewer();
        viewer->show();
        viewer->setWindowTitle("input vector field");
        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(-2,2);
        Calculus::Accum accum(calculus);
        input_one_form.applyToAccum(accum);
        input_one_form_anti_derivated.applyToAccum(accum);
        input_one_form_derivated.applyToAccum(accum);
        accum.display3D(*viewer, colormap);
        input_vector_field.display3D(*viewer);
        (*viewer) << Viewer::updateDisplay;
    }

    Calculus::PrimalForm0 solution_curl_free(calculus);
    { // solve curl free problem
        trace.beginBlock("solving curl free component");

        //! [3d_decomposition_curl_free_solve]
        typedef DiscreteExteriorCalculusSolver<Calculus, LinearAlgebraSolver, 0, PRIMAL, 0, PRIMAL> Solver;
        Solver solver;
        solver.compute(ad1 * d0);
        solution_curl_free = solver.solve(input_one_form_anti_derivated);
        //! [3d_decomposition_curl_free_solve]

        trace.info() << solver.isValid() << " " << solver.solver.info() << endl;
        trace.info() << "min=" << solution_curl_free.myContainer.minCoeff() << " max=" << solution_curl_free.myContainer.maxCoeff() << endl;
        trace.endBlock();
    }

    {
        typedef Viewer3D<Z3i::Space, Z3i::KSpace> Viewer;
        Viewer* viewer = new Viewer();
        viewer->show();
        viewer->setWindowTitle("curl free solution");
        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(solution_curl_free.myContainer.minCoeff(), solution_curl_free.myContainer.maxCoeff());
        Calculus::Accum accum(calculus);
        solution_curl_free.applyToAccum(accum);
        accum.display3D(*viewer, colormap);
        calculus.sharp(d0*solution_curl_free).display3D(*viewer);
        (*viewer) << Viewer::updateDisplay;
    }

    Calculus::PrimalForm2 solution_div_free(calculus);
    { // solve divergence free problem
        trace.beginBlock("solving divergence free component");

        //! [3d_decomposition_div_free_solve]
        typedef DiscreteExteriorCalculusSolver<Calculus, LinearAlgebraSolver, 2, PRIMAL, 2, PRIMAL> Solver;
        Solver solver;
        solver.compute(d1 * ad2);
        solution_div_free = solver.solve(input_one_form_derivated);
        //! [3d_decomposition_div_free_solve]

        trace.info() << solver.isValid() << " " << solver.solver.info() << endl;
        trace.info() << "min=" << solution_div_free.myContainer.minCoeff() << " max=" << solution_div_free.myContainer.maxCoeff() << endl;
        trace.endBlock();
    }

    {
        typedef Viewer3D<Z3i::Space, Z3i::KSpace> Viewer;
        Viewer* viewer = new Viewer();
        viewer->show();
        viewer->setWindowTitle("div free solution");
        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(solution_div_free.myContainer.minCoeff(), solution_div_free.myContainer.maxCoeff());
        Calculus::Accum accum(calculus);
        solution_div_free.applyToAccum(accum);
        accum.display3D(*viewer, colormap);
        calculus.sharp(ad2*solution_div_free).display3D(*viewer);
        (*viewer) << Viewer::updateDisplay;
    }

    //! [3d_decomposition_solution]
    const Calculus::PrimalForm1 solution_harmonic = input_one_form - d0*solution_curl_free - ad2*solution_div_free;
    //! [3d_decomposition_solution]
    trace.info() << "min=" << solution_harmonic.myContainer.minCoeff() << " max=" << solution_harmonic.myContainer.maxCoeff() << endl;

    {
        typedef Viewer3D<Z3i::Space, Z3i::KSpace> Viewer;
        Viewer* viewer = new Viewer();
        viewer->show();
        viewer->setWindowTitle("harmonic");
        typedef GradientColorMap<double, CMAP_JET> Colormap;
        Colormap colormap(solution_harmonic.myContainer.minCoeff(), solution_harmonic.myContainer.maxCoeff());
        Calculus::Accum accum(calculus);
        solution_harmonic.applyToAccum(accum);
        accum.display3D(*viewer, colormap);
        calculus.sharp(solution_harmonic).display3D(*viewer, 10);
        (*viewer) << Viewer::updateDisplay;
    }

    trace.endBlock();
}

int main(int argc, char* argv[])
{
    QApplication app(argc,argv);

    solve2d_laplacian();
    solve2d_dual_decomposition();
    solve2d_primal_decomposition();
    solve3d_decomposition();

    return app.exec();
}


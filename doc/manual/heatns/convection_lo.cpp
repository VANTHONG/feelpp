/* -*- mode: c++; coding: utf-8; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; show-trailing-whitespace: t -*- vim:fenc=utf-8:ft=tcl:et:sw=4:ts=4:sts=4

  This file is part of the Feel library

  Author(s): Christophe Prud'homme <christophe.prudhomme@ujf-grenoble.fr>
       Date: 2009-03-12

  Copyright (C) 2009 Universite Joseph Fourier (Grenoble I)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 3.0 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
/**
   \file convection_lo.cpp
   \author Christophe Prud'homme <christophe.prudhomme@ujf-grenoble.fr>
   \date 2009-03-12
 */
#include "convection.hpp"

// variational formulation language
#include <feel/feelvf/vf.hpp>

// <int Order_s, int Order_p, int Order_t>
void Convection ::initLinearOperator( sparse_matrix_ptrtype& L )
{
    boost::timer ti;
    Log() << "[initLinearOperator] start\n";

    mesh_ptrtype mesh = Xh->mesh();
    element_type U( Xh, "u" );
    element_type Un( Xh, "un" );
    element_type V( Xh, "v" );
    element_0_type u = U. element<0>(); // fonction vitesse
    element_0_type un = Un. element<0>(); // fonction vitesse
    element_0_type v = V. element<0>(); // fonction test vitesse
    element_1_type p = U. element<1>(); // fonction pression
    element_1_type pn = Un. element<1>(); // fonction pression
    element_1_type q = V. element<1>(); // fonction test pression
    element_2_type t = U. element<2>(); // fonction temperature
    element_2_type tn = Un. element<2>(); // fonction temperature
    element_2_type s = V. element<2>(); // fonction test temperature
    element_3_type xi = U. element<3>(); // fonction multipliers
    element_3_type eta = V. element<3>(); // fonction test multipliers

    //M_oplin = oplin_ptrtype( new oplin_type( Xh, Xh, M_backend ) );
    //M_oplin->mat().zero();

    double gr= M_current_Grashofs;
    double sqgr( 1/math::sqrt( gr ) );
    double pr = M_current_Prandtl;
    double sqgrpr( 1/( pr*math::sqrt( gr ) ) );
    double gamma( this->vm()["penalbc"]. as<double>() );
    double k=this->vm()["k"]. as<double>();
    double nu=this->vm()["nu"]. as<double>();
    double rho=this->vm()["rho"]. as<double>();

    int adim=this->vm()["adim"]. as<int>();


    //choix de la valeur des paramètres dimensionnés ou adimensionnés
    double a=0.0,b=0.0,c=0.0;

    if ( adim==1 )
    {
        a=1;
        b=sqgr;
        c=sqgrpr;
    }

    else
    {
        a=rho;
        b=nu;
        c=k;
    }

    // Fluid
    // diffusion

    form2( Xh, Xh, L ) =integrate( elements( mesh ), cst( b )*trace( gradt( u )*trans( grad( v ) ) )  );
    Log() << "[initLinearOperator] Fluid Diffusion terms done\n";
    // pressure-velocity terms
    form2( Xh, Xh, L )  += integrate ( elements( mesh ), - idt( p ) * div( v ) );
    form2( Xh, Xh, L )  += integrate ( elements( mesh ),   divt( u ) * id( q ) );

    Log() << "[initLinearOperator] Fluid Pressure-Velocity terms done\n";
    // multipliers for zero-mean pressure
    form2( Xh, Xh, L )  += integrate ( elements( mesh ), id( q )*idt( xi ) );
    form2( Xh, Xh, L )  += integrate ( elements( mesh ), idt( p )*id( eta ) );
    //form2( Xh, Xh, L )  += integrate ( elements( mesh ), 0*idt( xi )*id( eta ),  _Q<0>() );
    Log() << "[initLinearOperator] Fluid Pressure-Multipliers terms done\n";
    int weakdir( this->vm()["weakdir"]. as<int>() );

    if ( weakdir==1 )
    {
        // weak Dirichlet condition at the walls (u=0)
        AUTO( SigmaNt, ( -idt( p )*N()-cst( b )*gradt( u )*N() ) );
        AUTO( SigmaN, ( -id( q )*N()-cst( b )*grad( v )*N() ) );
        form2( Xh, Xh, L )  += integrate ( boundaryfaces( mesh ), -trans( SigmaNt )*id( v ) );
        form2( Xh, Xh, L )  += integrate ( boundaryfaces( mesh ), -trans( SigmaN )*idt( u ) );
        form2( Xh, Xh, L )  += integrate ( boundaryfaces( mesh ), +gamma*trans( idt( u ) )*id( v )/hFace()   );
    }

    Log() << "[initLinearOperator] Fluid Dirichlet weak BC terms done\n";

    Log() << "[initLinearOperator] done in " << ti.elapsed() << "s\n";


}

// instantiation
// class Convection<2,1,2>;
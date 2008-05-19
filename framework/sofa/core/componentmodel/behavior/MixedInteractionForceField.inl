/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#ifndef SOFA_CORE_COMPONENTMODEL_BEHAVIOR_MIXEDINTERACTIONFORCEFIELD_INL
#define SOFA_CORE_COMPONENTMODEL_BEHAVIOR_MIXEDINTERACTIONFORCEFIELD_INL

#include <sofa/core/objectmodel/DataPtr.h>
#include "MixedInteractionForceField.h"

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace behavior
{

template<class DataTypes1, class DataTypes2>
MixedInteractionForceField<DataTypes1, DataTypes2>::MixedInteractionForceField(MechanicalState<DataTypes1> *mm1, MechanicalState<DataTypes2> *mm2)
    : mstate1(mm1), mstate2(mm2)
{
}

template<class DataTypes1, class DataTypes2>
MixedInteractionForceField<DataTypes1, DataTypes2>::~MixedInteractionForceField()
{
}

template<class DataTypes1, class DataTypes2>
void MixedInteractionForceField<DataTypes1, DataTypes2>::init()
{
    InteractionForceField::init();
}

template<class DataTypes1, class DataTypes2>
void MixedInteractionForceField<DataTypes1, DataTypes2>::addForce()
{
    if (mstate1 && mstate2)
        addForce(*mstate1->getF(), *mstate2->getF(),
                *mstate1->getX(), *mstate2->getX(),
                *mstate1->getV(), *mstate2->getV());
}

template<class DataTypes1, class DataTypes2>
void MixedInteractionForceField<DataTypes1, DataTypes2>::addDForce(double kFactor, double bFactor)
{
    if (mstate1 && mstate2)
        addDForce(*mstate1->getF(),  *mstate2->getF(),
                *mstate1->getDx(), *mstate2->getDx(),
                kFactor, bFactor);
}

template<class DataTypes1, class DataTypes2>
void MixedInteractionForceField<DataTypes1, DataTypes2>::addDForceV(double kFactor, double bFactor)
{
    if (mstate1 && mstate2)
        addDForce(*mstate1->getF(),  *mstate2->getF(),
                *mstate1->getV(), *mstate2->getV(),
                kFactor, bFactor);
}

template<class DataTypes1, class DataTypes2>
void MixedInteractionForceField<DataTypes1, DataTypes2>::addDForce(VecDeriv1& /*df1*/, VecDeriv2& /*df2*/, const VecDeriv1& /*dx1*/, const VecDeriv2& /*dx2*/)
{
    std::cerr << "ERROR("<<getClassName()<<"): addDForce not implemented.\n";
}

template<class DataTypes1, class DataTypes2>
void MixedInteractionForceField<DataTypes1, DataTypes2>::addDForce(VecDeriv1& df1, VecDeriv2& df2, const VecDeriv1& dx1, const VecDeriv2& dx2, double kFactor, double /*bFactor*/)
{
    if (kFactor == 1.0)
        addDForce(df1, df2, dx1, dx2);
    else if (kFactor != 0.0)
    {
        BaseMechanicalState::VecId vtmp1(BaseMechanicalState::VecId::V_DERIV,BaseMechanicalState::VecId::V_FIRST_DYNAMIC_INDEX);
        mstate1->vAvail(vtmp1);
        mstate1->vAlloc(vtmp1);
        BaseMechanicalState::VecId vdx1(BaseMechanicalState::VecId::V_DERIV,0);
        /// @TODO: Add a better way to get the current VecId of dx
        for (vdx1.index=0; vdx1.index<vtmp1.index; ++vdx1.index)
            if (mstate1->getVecDeriv(vdx1.index) == &dx1)
                break;
        mstate1->vOp(vtmp1,BaseMechanicalState::VecId::null(),vdx1,kFactor);

        BaseMechanicalState::VecId vtmp2(BaseMechanicalState::VecId::V_DERIV,BaseMechanicalState::VecId::V_FIRST_DYNAMIC_INDEX);
        mstate2->vAvail(vtmp2);
        mstate2->vAlloc(vtmp2);
        BaseMechanicalState::VecId vdx2(BaseMechanicalState::VecId::V_DERIV,0);
        /// @TODO: Add a better way to get the current VecId of dx
        for (vdx2.index=0; vdx2.index<vtmp2.index; ++vdx2.index)
            if (mstate2->getVecDeriv(vdx2.index) == &dx2)
                break;
        mstate2->vOp(vtmp2,BaseMechanicalState::VecId::null(),vdx2,kFactor);

        addDForce(df1, df2, *mstate1->getVecDeriv(vtmp1.index), *mstate2->getVecDeriv(vtmp2.index));

        mstate1->vFree(vtmp1);
        mstate2->vFree(vtmp2);
    }
}

template<class DataTypes1, class DataTypes2>
double MixedInteractionForceField<DataTypes1, DataTypes2>::getPotentialEnergy()
{
    if (mstate1 && mstate2)
        return getPotentialEnergy(*mstate1->getX(), *mstate2->getX());
    else return 0;
}

} // namespace behavior

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
